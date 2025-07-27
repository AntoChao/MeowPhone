#include "MPCharacterCat.h"
#include "Net/UnrealNetwork.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../Player/MPPlayerState.h"
#include "Components/CapsuleComponent.h"

#include "../../CommonEnum.h"
#include "../../CommonStruct.h"
#include "../../HighLevel/Managers/ManagerLog.h"

#include "../../HighLevel/MPGMGameplay.h"
#include "MPCharacterHuman.h"
#include "../Ability/MPAbility.h"

// 1. Common class methods
AMPCharacterCat::AMPCharacterCat()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Set up camera for third-person view
	cameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
    cameraSpringArm->SetupAttachment(RootComponent);
	cameraSpringArm->TargetArmLength = 400.0f;
	cameraSpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));
    cameraSpringArm->bUsePawnControlRotation = true;
		
	characterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CharacterCamera"));
    characterCamera->SetupAttachment(cameraSpringArm, USpringArmComponent::SocketName);
	characterCamera->bUsePawnControlRotation = false;

	// Initialize animation state
	animState.curPosture = ECatPosture::Standing;
	animState.curMove = EMoveState::Idle;
	animState.curAir = EAirState::Grounded;
	animState.curContext = ECatContext::None;
	animState.curInteraction = ECatInteractionState::None;
}

// 1. all actor class methods
void AMPCharacterCat::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMPCharacterCat, struggleBar);
	DOREPLIFETIME(AMPCharacterCat, animState);
}

void AMPCharacterCat::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeAllAbility();
	BeginIdlePoseTimer();
}

void AMPCharacterCat::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
    
	// Update air state tracking
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		bool bIsGrounded = MovementComp->IsMovingOnGround();
		
		if (bIsGrounded)
		{
			if (!bWasGroundedLastTick)
			{
				// Just landed
            SetAir(EAirState::Grounded);
        airTime = 0.0f;
        bIsFalling = false;
			}
		}
		else
		{
			airTime += deltaTime;
			
			if (MovementComp->Velocity.Z > 0)
			{
            SetAir(EAirState::Jump);
        }
			else
			{
            SetAir(EAirState::Falling);
				
				// Check for long falling
				if (!bIsFalling)
				{
            fallStartZ = GetActorLocation().Z;
            bIsFalling = true;
        }
				
				float fallDistance = fallStartZ - GetActorLocation().Z;
				if (fallDistance > longFallingHeightThreshold)
				{
					SetContext(ECatContext::LongFalling);
				}
			}
		}
		
		bWasGroundedLastTick = bIsGrounded;
	}
}

// 2.interface
// 2.1 interactable
// 2. Interface
bool AMPCharacterCat::IsInteractable(AMPCharacter* player)
{
	if (!IsValid(player))
	{
		return false;
	}

	// Check if player is human and cat is not already being held
	if (AMPCharacterHuman* humanPlayer = Cast<AMPCharacterHuman>(player))
	{
		return !IsBeingHeld() && !IsRubbing();
	}

	return false;
}

void AMPCharacterCat::BeInteracted(AMPCharacter* player)
{
    if (!IsValid(player))
    {
		UManagerLog::LogWarning(TEXT("Invalid player provided for interaction"), TEXT("MPCharacterCat"));
        return;
    }

	if (AMPCharacterHuman* humanPlayer = Cast<AMPCharacterHuman>(player))
	{
		humanPlayer->StartHoldingCat(this);
    }
}

// 3. components
// 3,1 camera component 
bool AMPCharacterCat::IsFootNearWall(FVector& OutWallNormal)
{
	FVector start = GetActorLocation();
	FVector end = start + GetActorForwardVector() * wallDetectRadius;
	
	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, queryParams);
	
    if (bHit)
    {
		OutWallNormal = hitResult.Normal;
		LastWallNormal = OutWallNormal;
		return true;
    }
	
    return false;
}

// 4. common properties

// 5. controller/ input reaction
void AMPCharacterCat::Move(FVector2D direction)
{
	if (!CheckIfIsAbleToMove()) return;

	// Calculate movement direction
	FVector forward = GetActorForwardVector();
	FVector right = GetActorRightVector();
	FVector movement = forward * direction.Y + right * direction.X;
	movement.Normalize();

	// Apply movement
	AddMovementInput(movement, 1.0f);
    
	// Update animation state
	if (movement.SizeSquared() > 0.1f)
	{
        SetMove(EMoveState::Walk);
    }
	else
	{
		SetMove(EMoveState::Idle);
    }
}
void AMPCharacterCat::MoveStop()
{
	AddMovementInput(FVector::ZeroVector, 0.0f);
    SetMove(EMoveState::Idle);
}
void AMPCharacterCat::Run()
{
	if (!CheckIfIsAbleToRun()) return;

	curSpeed = runSpeed + extraSpeed;
	UpdateSpeed();
	SetMove(EMoveState::Run);
}

void AMPCharacterCat::RunStop()
{
	curSpeed = moveSpeed + extraSpeed;
	UpdateSpeed();
	SetMove(EMoveState::Walk);
}
void AMPCharacterCat::CrouchStart()
{
	if (!CheckIfIsAbleToCrouch()) return;
    
	curSpeed = crouchSpeed + extraSpeed;
	UpdateSpeed();
    SetPosture(ECatPosture::Crouching);
}
void AMPCharacterCat::CrouchEnd()
{
	curSpeed = moveSpeed + extraSpeed;
	UpdateSpeed();
    SetPosture(ECatPosture::Standing);
}
void AMPCharacterCat::JumpStart()
{
	if (!CheckIfIsAbleToJump()) return;

	Jump();
    SetAir(EAirState::Jump);
	SetContext(ECatContext::VerticalJump);
}

void AMPCharacterCat::JumpEnd()
{
	StopJumping();
}

void AMPCharacterCat::PerformDoubleJump()
{
    if (!CheckIfIsAbleToDoubleJump()) return;

	// Perform wall jump
	FVector jumpDirection = LastWallNormal + FVector(0, 0, 1);
	jumpDirection.Normalize();
	
	LaunchCharacter(jumpDirection * jumpStrength, true, true);
    doubleJumpCount++;
	
	SetAir(EAirState::Jump);
	SetContext(ECatContext::VerticalJump);
	
	UManagerLog::LogInfo(TEXT("Cat performed double jump"), TEXT("MPCharacterCat"));
}

void AMPCharacterCat::Interact()
{
	if (!CheckIfIsAbleToInteract()) return;

	if (detectInteractableActor)
        {
            detectInteractableActor->BeInteracted(this);
    }
}

// 5.1 controller enable/disable
bool AMPCharacterCat::CheckIfIsAbleToLook()
{	
	return Super::CheckIfIsAbleToLook() && !IsBeingHeld();
}
bool AMPCharacterCat::CheckIfIsAbleToMove()
{
	return Super::CheckIfIsAbleToMove() && !IsBeingHeld();
}
bool AMPCharacterCat::CheckIfIsAbleToInteract()
{
	return Super::CheckIfIsAbleToInteract() && !IsBeingHeld();
}

bool AMPCharacterCat::CheckIfIsAbleToUseItems()
{
	return Super::CheckIfIsAbleToUseItems() && !IsBeingHeld();
}

bool AMPCharacterCat::CheckIfIsAbleToRun()
{
	return Super::CheckIfIsAbleToRun() && !IsBeingHeld();
}

bool AMPCharacterCat::CheckIfIsAbleToCrouch()
{
	return Super::CheckIfIsAbleToCrouch() && !IsBeingHeld();
}

bool AMPCharacterCat::CheckIfIsAbleToJump()
{
	return Super::CheckIfIsAbleToJump() && !IsBeingHeld();
}
bool AMPCharacterCat::CheckIfIsAbleToDoubleJump()
{
	// Cats can double jump when near a wall
	FVector wallNormal;
	return IsFootNearWall(wallNormal) && doubleJumpCount < 1;
}
// 5.2 movement related
// 5.3 jump related
// 5.4 air related
void AMPCharacterCat::UpdateMovingControlsPerTick(float deltaTime)
{
	Super::UpdateMovingControlsPerTick(deltaTime);
	
	// Update hold time if being held
	if (IsBeingHeld() && IsValid(humanHolding))
	{
		curHoldTime += deltaTime;
		curHoldPercentage = FMath::Clamp(curHoldTime / holdTotalTime, 0.0f, 1.0f);
	}
}

// 5.5 interaction related
void AMPCharacterCat::OnRep_StruggleBar() {}

void AMPCharacterCat::StartedToBeHold(AMPCharacter* humanPlayer)
{
    if (!IsValid(humanPlayer))
    {
		UManagerLog::LogWarning(TEXT("Invalid human player provided for holding"), TEXT("MPCharacterCat"));
        return;
    }

	humanHolding = Cast<AMPCharacterHuman>(humanPlayer);
	if (IsValid(humanHolding))
	{
    SetInteraction(ECatInteractionState::BeingHeld);
    struggleBar = 0.0f;
		curHoldTime = 0.0f;
    
		UManagerLog::LogInfo(TEXT("Cat started being held"), TEXT("MPCharacterCat"));
	}
}

void AMPCharacterCat::Straggle()
{
	if (!IsBeingHeld()) return;

	struggleBar += struggleBarPerInput;
	if (struggleBar >= struggleBarMax)
	{
		struggleBar = struggleBarMax;
		if (IsValid(humanHolding))
		{
			humanHolding->ForceReleaseCat();
		}
		
		UManagerLog::LogInfo(TEXT("Cat struggled free"), TEXT("MPCharacterCat"));
	}
}

void AMPCharacterCat::EndToBeHold()
{
    if (IsValid(humanHolding))
    {
		humanHolding = nullptr;
		SetInteraction(ECatInteractionState::None);
		
		UManagerLog::LogInfo(TEXT("Cat stopped being held"), TEXT("MPCharacterCat"));
    }
}

void AMPCharacterCat::StartToBeRubbed(AMPCharacterHuman* humanToRub)
{
    if (!IsValid(humanToRub))
    {
		UManagerLog::LogWarning(TEXT("Invalid human provided for rubbing"), TEXT("MPCharacterCat"));
        return;
    }

	humanRubbing = humanToRub;
    SetInteraction(ECatInteractionState::BeingRubbed);
	
	UManagerLog::LogInfo(TEXT("Cat started being rubbed"), TEXT("MPCharacterCat"));
}

void AMPCharacterCat::StopToBeRubbed()
{
	humanRubbing = nullptr;
	SetInteraction(ECatInteractionState::None);
	
	UManagerLog::LogInfo(TEXT("Cat stopped being rubbed"), TEXT("MPCharacterCat"));
}

FVector AMPCharacterCat::GetHoldAnimLeftHandPosition() const
{
    if (GetMesh() && GetMesh()->DoesSocketExist(holdAnimLeftHandSocketName))
    {
        return GetMesh()->GetSocketLocation(holdAnimLeftHandSocketName);
    }
    return FVector::ZeroVector;
}

FVector AMPCharacterCat::GetHoldAnimRightHandPosition() const
{
    if (GetMesh() && GetMesh()->DoesSocketExist(holdAnimRightHandSocketName))
    {
        return GetMesh()->GetSocketLocation(holdAnimRightHandSocketName);
    }
    return FVector::ZeroVector;
}

float AMPCharacterCat::GetStruggleBarPercentage() 
{ 
	return struggleBarMax > 0.0f ? struggleBar / struggleBarMax : 0.0f; 
}

// State getters
bool AMPCharacterCat::IsBeingHeld()
{ 
	return animState.curInteraction == ECatInteractionState::BeingHeld; 
}
bool AMPCharacterCat::IsRubbing()
{ 
	return animState.curInteraction == ECatInteractionState::BeingRubbed; 
}
AMPCharacterHuman* AMPCharacterCat::GetHumanHolding()
{ 
	return IsValid(humanHolding) ? humanHolding : nullptr; 
}
AMPCharacterHuman* AMPCharacterCat::GetHumanRubbing() 
{ 
	return IsValid(humanRubbing) ? humanRubbing : nullptr; 
}


// 5.6 ability related
void AMPCharacterCat::InitializeAllAbility()
{
	// Initialize passive abilities
	for (EAbility abilityType : initPassiveAbilities)
	{
		// This would create ability instances based on the ability type
		// Implementation depends on your ability factory system
	}
	
	InitializeActiveAbility();
}

void AMPCharacterCat::InitializeActiveAbility()
{
	// Initialize the active ability
	// Implementation depends on your ability system
}

void AMPCharacterCat::InitializeAllPassiveAbility()
{
	// Initialize all passive abilities
	// Implementation depends on your ability system
}

void AMPCharacterCat::UseActiveAbility()
{
	if (IsValid(activeAbility))
	{
		activeAbility->BeUsed(detectedActor);
	}
}

void AMPCharacterCat::UsePassiveAbility(EAbility abilityType)
{
	// Use a specific passive ability
	// Implementation depends on your ability system
}

// 6. animation system
// 6.1 animation state
void AMPCharacterCat::SetPosture(ECatPosture newPosture)
{
	if (animState.curPosture != newPosture)
{
    animState.curPosture = newPosture;
	}
}
void AMPCharacterCat::SetMove(EMoveState newMove)
{
	if (animState.curMove != newMove)
{
    animState.curMove = newMove;
	}
}
void AMPCharacterCat::SetAir(EAirState newAir)
{
	if (animState.curAir != newAir)
{
    animState.curAir = newAir;
	}
}

void AMPCharacterCat::SetContext(ECatContext newContext, bool bMandatory)
{
	if (bMandatory || !isDoingAnAnimation)
	{
		animState.curContext = newContext;
		if (newContext != ECatContext::None)
		{
			PlayCatContextAnimMontage(newContext);
		}
    }
}

void AMPCharacterCat::SetInteraction(ECatInteractionState newInteraction)
{
	if (animState.curInteraction != newInteraction)
{
    animState.curInteraction = newInteraction;
	}
}

void AMPCharacterCat::BeginIdlePoseTimer()
{
	float randomTime = FMath::RandRange(idlePoseMinTime, idlePoseMaxTime);
	GetWorld()->GetTimerManager().SetTimer(idlePoseTimerHandle, this, &AMPCharacterCat::IdlePoseTimeout, randomTime, false);
}
void AMPCharacterCat::IdlePoseTimeout()
{
	// Randomly change to a different idle pose
	TArray<ECatPosture> idlePoses = { ECatPosture::Sitting, ECatPosture::Lying };
	ECatPosture newPose = idlePoses[FMath::RandRange(0, idlePoses.Num() - 1)];
	
	SetPosture(newPose);
	
	// Start the timer again
	BeginIdlePoseTimer();
}
void AMPCharacterCat::OnRep_AnimState()
{
	// Handle animation state replication on clients
	// This will be called when the server updates the animation state
}

// 6.2 animation context/ montage
void AMPCharacterCat::PlayCatContextAnimMontage(ECatContext context)
{
	UAnimMontage* montageToPlay = nullptr;
    float playRate = 1.0f;

    switch (context)
    {
        case ECatContext::BeStunned:
			montageToPlay = beStunned_Montage;
			playRate = beStunnedMontagePlayRate;
			break;
		case ECatContext::VerticalJump:
			montageToPlay = verticalJump_Montage;
			playRate = verticalJump_MontagePlayRate;
			break;
		case ECatContext::LongFalling:
			montageToPlay = longFalling_Montage;
			playRate = longFalling_MontagePlayRate;
			break;
		case ECatContext::StandStretch:
			montageToPlay = standStretch_Montage;
			playRate = standStretch_MontagePlayRate;
			break;
		case ECatContext::SitScratch:
			montageToPlay = sitScratch_Montage;
			playRate = sitScratch_MontagePlayRate;
			break;
		case ECatContext::SitPaw:
			montageToPlay = sitPaw_Montage;
			playRate = sitPaw_MontagePlayRate;
			break;
		case ECatContext::SitBiscuit:
			montageToPlay = sitBiscuit_Montage;
			playRate = sitBiscuit_MontagePlayRate;
			break;
		case ECatContext::SitDrinkWater:
			montageToPlay = sitDrinkWater_Montage;
			playRate = sitDrinkWater_MontagePlayRate;
			break;
		case ECatContext::LyingSleepSpot:
			montageToPlay = lyingSleepSpot_Montage;
			playRate = lyingSleepSpot_MontagePlayRate;
			break;
		default:
			break;
    }

	if (montageToPlay)
	{
		PlaySelectedMontage(montageToPlay, playRate);
	}
}

void AMPCharacterCat::OnMontageEndedContextClear(UAnimMontage* montage, bool bInterrupted)
{
	animState.curContext = ECatContext::None;
}

// 7. special condition
void AMPCharacterCat::BeStunned(int32 stunDuration)
{
	Super::BeStunned(stunDuration);
	SetContext(ECatContext::BeStunned);
}
void AMPCharacterCat::StopStunned()
{
	Super::StopStunned();
	SetContext(ECatContext::None);
}
