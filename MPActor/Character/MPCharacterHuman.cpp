#include "MPCharacterHuman.h"

#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

#include "../../CommonEnum.h"
#include "../../CommonStruct.h"
#include "../../HighLevel/MPLogManager.h"

#include "../Player/MPControllerPlayer.h"
#include "../Player/MPPlayerState.h"
#include "MPCharacterCat.h"
#include "../Item/MPItem.h"
#include "MotionWarpingComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "Engine/SkeletalMeshSocket.h"

AMPCharacterHuman::AMPCharacterHuman()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Set up camera for first-person view
	cameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	cameraSpringArm->SetupAttachment(RootComponent);
	cameraSpringArm->TargetArmLength = 0.0f; // First person
	cameraSpringArm->bUsePawnControlRotation = true;

	characterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CharacterCamera"));
	characterCamera->SetupAttachment(cameraSpringArm, USpringArmComponent::SocketName);
	characterCamera->bUsePawnControlRotation = true;

	currentHealth = maxHealth;
	isDead = false;

	// Initialize animation state
	animState.curPosture = EHumanPosture::Standing;
	animState.curMove = EMoveState::Idle;
	animState.curAir = EAirState::Grounded;
	animState.curContext = EHumanContext::None;
	animState.curInteraction = EHumanInteractionState::None;

	holdAnimMontage = nullptr;
	catHolding = nullptr;
}

// 1. all actor class methods
void AMPCharacterHuman::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMPCharacterHuman, currentHealth);
	DOREPLIFETIME(AMPCharacterHuman, isDead);
	DOREPLIFETIME(AMPCharacterHuman, animState);
}

void AMPCharacterHuman::BeginPlay()
{
	Super::BeginPlay();
}

void AMPCharacterHuman::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	updateHoldAnimHandTargets();
}

// 2. interface
// 2.1 interactable
bool AMPCharacterHuman::IsInteractable(AMPCharacter* player)
{
	if (!IsValid(player))
	{
		return false;
	}

	// Check if player is cat and human is not already holding a cat
	if (AMPCharacterCat* catPlayer = Cast<AMPCharacterCat>(player))
	{
		return !IsHoldingCat() && !IsBeingRubbed();
	}

	return false;
}

void AMPCharacterHuman::BeInteracted(AMPCharacter* player)
{
	if (!IsValid(player))
	{
		UMPLogManager::LogWarning(TEXT("Invalid player provided for interaction"), TEXT("MPCharacterHuman"));
		return;
	}

	if (AMPCharacterCat* catPlayer = Cast<AMPCharacterCat>(player))
	{
		StartHoldingCat(catPlayer);
	}
}

// 2.2 sound
// 3. components
// 3.1 camera component
// 4. common properties
// 4.1 health system
void AMPCharacterHuman::TakeHealthDamage(int32 damageAmount)
{
	if (isDead) return;

	currentHealth = FMath::Max(0, currentHealth - damageAmount);
	
	UMPLogManager::LogInfo(FString::Printf(TEXT("Human took %d damage! Health: %d/%d"), damageAmount, currentHealth, maxHealth), TEXT("MPCharacterHuman"));
	
	if (currentHealth <= 0)
	{
		Die();
	}
}

void AMPCharacterHuman::HealHealth(int32 healAmount)
{
	if (isDead) return;

	currentHealth = FMath::Min(maxHealth, currentHealth + healAmount);
	
	UMPLogManager::LogInfo(FString::Printf(TEXT("Human healed %d health! Health: %d/%d"), healAmount, currentHealth, maxHealth), TEXT("MPCharacterHuman"));
}

void AMPCharacterHuman::Die()
{
	if (isDead) return;

	isDead = true;
	
	// Release any held cat when dying
	if (IsValid(catHolding))
	{
		ForceReleaseCat();
	}
	
	// Stop any rubbing interaction
	if (IsValid(catRubbing))
	{
		StopToBeRubbed();
	}
	
	UMPLogManager::LogInfo(TEXT("Human player died"), TEXT("MPCharacterHuman"));
	
	// Notify game mode about player death
	if (AGameModeBase* gameMode = UGameplayStatics::GetGameMode(GetWorld()))
	{
		if (AMPGMGameplay* mpGameMode = Cast<AMPGMGameplay>(gameMode))
		{
			if (AMPControllerPlayer* playerController = Cast<AMPControllerPlayer>(GetController()))
			{
				// Handle player death in game mode
				// mpGameMode->HandlePlayerDeath(playerController);
			}
		}
	}
}

// Replication callbacks
void AMPCharacterHuman::OnRep_Health()
{
	UMPLogManager::LogInfo(FString::Printf(TEXT("Client: Human health updated to %d/%d"), currentHealth, maxHealth), TEXT("MPCharacterHuman"));
}
void AMPCharacterHuman::OnRep_IsDead()
{
	if (isDead)
	{
		UMPLogManager::LogInfo(TEXT("Client: Human player died"), TEXT("MPCharacterHuman"));
	}
}

// 5. controller/ input reaction
void AMPCharacterHuman::Move(FVector2D direction)
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

void AMPCharacterHuman::Run()
{
	if (!CheckIfIsAbleToRun()) return;

	curSpeed = runSpeed + extraSpeed;
	UpdateSpeed();
	SetMove(EMoveState::Run);
}

void AMPCharacterHuman::RunStop()
{
	curSpeed = moveSpeed + extraSpeed;
	UpdateSpeed();
	SetMove(EMoveState::Walk);
}

void AMPCharacterHuman::CrouchStart()
{
	if (!CheckIfIsAbleToCrouch()) return;

	curSpeed = crouchSpeed + extraSpeed;
	UpdateSpeed();
	SetPosture(EHumanPosture::Crouching);
}

void AMPCharacterHuman::CrouchEnd()
{
	curSpeed = moveSpeed + extraSpeed;
	UpdateSpeed();
	SetPosture(EHumanPosture::Standing);
}

void AMPCharacterHuman::JumpStart()
{
	if (!CheckIfIsAbleToJump()) return;

	Jump();
	SetAir(EAirState::Jump);
}

void AMPCharacterHuman::JumpEnd()
{
	StopJumping();
}

void AMPCharacterHuman::Interact()
{
	if (!CheckIfIsAbleToInteract()) return;

	// Handle cat interaction
	if (detectInteractableActor.IsValid())
	{
		detectInteractableActor->BeInteracted(this);
	}
}

// 5.1 controller enable/disable
bool AMPCharacterHuman::CheckIfIsAbleToLook()
{
	return Super::CheckIfIsAbleToLook() && !isDead;
}

bool AMPCharacterHuman::CheckIfIsAbleToMove()
{
	return Super::CheckIfIsAbleToMove() && !isDead;
}

bool AMPCharacterHuman::CheckIfIsAbleToInteract()
{
	return Super::CheckIfIsAbleToInteract() && !isDead;
}

bool AMPCharacterHuman::CheckIfIsAbleToUseItems()
{
	return Super::CheckIfIsAbleToUseItems() && !isDead;
}

bool AMPCharacterHuman::CheckIfIsAbleToRun()
{
	return Super::CheckIfIsAbleToRun() && !isDead;
}

bool AMPCharacterHuman::CheckIfIsAbleToCrouch()
{
	return Super::CheckIfIsAbleToCrouch() && !isDead;
}

bool AMPCharacterHuman::CheckIfIsAbleToJump()
{
	return Super::CheckIfIsAbleToJump() && !isDead;
}

// 5.2 movement related
// 5.3 jump  related
// 5.4 air related
void AMPCharacterHuman::UpdateMovingControlsPerTick(float deltaTime)
{
	Super::UpdateMovingControlsPerTick(deltaTime);
	
	// Update air state tracking
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		bool bIsGrounded = MovementComp->IsMovingOnGround();
		
		if (bIsGrounded)
		{
			SetAir(EAirState::Grounded);
		}
		else
		{
			if (MovementComp->Velocity.Z > 0)
			{
				SetAir(EAirState::Jump);
			}
			else
			{
				SetAir(EAirState::Falling);
			}
		}
	}
}

// 5.5 interaction related
void AMPCharacterHuman::StartHoldingCat(AMPCharacterCat* catToHold)
{
	if (!IsValid(catToHold))
	{
		UMPLogManager::LogWarning(TEXT("Invalid cat provided for holding"), TEXT("MPCharacterHuman"));
		return;
	}

	// Release any currently held cat
	if (IsValid(catHolding))
	{
		StopHoldingCat();
	}

	catHolding = catToHold;
	SetInteraction(EHumanInteractionState::HoldingCat);
	
	// Notify the cat that it's being held
	catToHold->StartedToBeHold(this);

	catHolding = catToHold;
	// Play the hold context animation montage
	SetContext(EHumanContext::HoldCat, true);

	UMPLogManager::LogInfo(TEXT("Human started holding cat"), TEXT("MPCharacterHuman"));
}

void AMPCharacterHuman::StopHoldingCat()
{
    if (IsValid(catHolding))
    {
        // Play stop hold animation montage and defer cleanup to montage event
        SetContext(EHumanContext::StopHoldCat, true);
        // The rest of the cleanup will be handled by a BlueprintCallable function triggered from the montage
    }
}

void AMPCharacterHuman::PutCatInCage()
{
    SetContext(EHumanContext::PutCatInCage, true);
}

void AMPCharacterHuman::ForceReleaseCat()
{
	if (IsValid(catHolding))
	{
		UMPLogManager::LogWarning(TEXT("Human forced to release cat due to max struggle"), TEXT("MPCharacterHuman"));
		
		catHolding->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        
		catHolding->EndToBeHold();
		catHolding = nullptr;
		SetInteraction(EHumanInteractionState::None);
	}
}

void AMPCharacterHuman::StartToBeRubbed(AMPCharacterCat* rubbedCat)
{
	if (!IsValid(rubbedCat))
	{
		UMPLogManager::LogWarning(TEXT("Invalid cat provided for rubbing"), TEXT("MPCharacterHuman"));
		return;
	}

	catRubbing = rubbedCat;
	SetInteraction(EHumanInteractionState::BeingRubbed);
	
	UMPLogManager::LogInfo(TEXT("Human started being rubbed by cat"), TEXT("MPCharacterHuman"));
}

void AMPCharacterHuman::StopToBeRubbed()
{
	catRubbing = nullptr;
	SetInteraction(EHumanInteractionState::None);
	
	UMPLogManager::LogInfo(TEXT("Human stopped being rubbed by cat"), TEXT("MPCharacterHuman"));
}

// 6. animation system
// 6.1 animation state
void AMPCharacterHuman::SetPosture(EHumanPosture newPosture)
{
	if (animState.curPosture != newPosture)
	{
		animState.curPosture = newPosture;
	}
}

void AMPCharacterHuman::SetMove(EMoveState newMove)
{
	if (animState.curMove != newMove)
	{
		animState.curMove = newMove;
	}
}

void AMPCharacterHuman::SetAir(EAirState newAir)
{
	if (animState.curAir != newAir)
	{
		animState.curAir = newAir;
	}
}

void AMPCharacterHuman::SetContext(EHumanContext newContext, bool bMandatory)
{
    // Only allow context change if not currently playing an animation montage, unless mandatory
    if (bMandatory || !isDoingAnAnimation)
    {
		animState.curContext = newContext;
		if (newContext != EHumanContext::None)
		{
			PlayHumanContextAnimMontage(newContext);
		}
    }
}

void AMPCharacterHuman::SetInteraction(EHumanInteractionState newInteraction)
{
	if (animState.curInteraction != newInteraction)
	{
		animState.curInteraction = newInteraction;
	}
}

void AMPCharacterHuman::OnRep_AnimState()
{
	// Handle animation state replication on clients
	// This will be called when the server updates the animation state
}

// 6.2 animation context/ montage
void AMPCharacterHuman::PlayHumanContextAnimMontage(EHumanContext context)
{
    UAnimMontage* montageToPlay = nullptr;
    float playRate = 1.0f;

    switch (context)
    {
    case EHumanContext::BeStunned:
        montageToPlay = beStunned_Montage;
		playRate = beStunnedMontagePlayRate;
        break;
    case EHumanContext::HoldCat:
        montageToPlay = holdAnimMontage;
        playRate = holdAnimMontagePlayRate;
        if (montageToPlay && catHolding)
        {
            setupHoldAnimmotionWarping(catHolding);
        }
        break;
    case EHumanContext::StopHoldCat:
        montageToPlay = stopHoldAnimMontage;
        playRate = stopHoldAnimMontagePlayRate;
        break;
	case EHumanContext::BeStruggleOut:
        montageToPlay = beStruggleOut_Montage;
        playRate = beStruggleOutMontagePlayRate;
        break;
    case EHumanContext::PutCatInCage:
        montageToPlay = putCatInCage_Montage;
        playRate = putCatInCageMontagePlayRate;
        break;
    case EHumanContext::DieEffect:
        montageToPlay = dieEffect_Montage;
        playRate = dieEffectMontagePlayRate;
        break;
	case EHumanContext::TurnOnPower:
        montageToPlay = turnOnPower_Montage;
        playRate = turnOnPowerMontagePlayRate;
        break;
    default:
        break;
    }

    if (montageToPlay)
    {
        PlaySelectedMontage(montageToPlay, playRate);
    }
}
// Override OnMontageEndedContextClear to handle hold context
void AMPCharacterHuman::OnMontageEndedContextClear(UAnimMontage* montage, bool bInterrupted)
{
    animState.curContext = EHumanContext::None;
	switch (montage)
	{
		case EHumanContext::StopHoldCat:
			// no need to call stopHoldAnimCleanup beacuse should be called in blueprint	
			break; 
		default:
			break;
	}
}

// 6.3 warp hold animation
void AMPCharacterHuman::holdAnimSnapping()
{
    if (!catHolding) return;
    USkeletalMeshComponent* humanMesh = GetMesh();
    if (!humanMesh) return;
    catHolding->AttachToComponent(
        humanMesh,
        FAttachmentTransformRules::KeepRelativeTransform,
        holdAnimChestSocketName
    );
    catHolding->SetActorRelativeLocation(holdAnimChestSocketOffset);
    catHolding->SetActorRelativeRotation(FRotator::ZeroRotator);
}

// BlueprintCallable function to update hand IK targets
void AMPCharacterHuman::updateHoldAnimHandTargets()
{
	if (animState.curContext == EHumanContext::HoldCat && catHolding)
	{
		holdAnimLeftHandTarget = catHolding->GetHoldAnimLeftHandPosition();
		holdAnimRightHandTarget = catHolding->GetHoldAnimRightHandPosition();
	}
	else 
	{
		holdAnimLeftHandTarget = FVector::ZeroVector;
		holdAnimRightHandTarget = FVector::ZeroVector;
	}
}

void AMPCharacterHuman::setupHoldAnimmotionWarping(AMPCharacterCat* catActor)
{
    if (!catActor || !motionWarpingComponent) return;
    FVector catLocation = catActor->GetActorLocation();
    FVector humanLocation = GetActorLocation();
    FVector directionToCat = (catLocation - humanLocation).GetSafeNormal();
    float distanceToCat = FVector::Dist(catLocation, humanLocation);
    float targetDistance = distanceToCat - holdAnimWarpDistanceOffset;
    FVector warpTargetLocation = humanLocation + directionToCat * targetDistance;
    FRotator warpTargetRotation = directionToCat.Rotation();
    FmotionWarpingTarget warpTarget;
    warpTarget.Name = holdAnimWarpTargetName;
    warpTarget.Location = warpTargetLocation;
    warpTarget.Rotation = warpTargetRotation;
    motionWarpingComponent->AddOrUpdateWarpTarget(warpTarget);
    catHolding = catActor;
}

// BlueprintCallable function to clean up after stop hold montage
// can be called for release also
void AMPCharacterHuman::stopHoldAnimCleanup()
{
    // Detach cat, reset transforms, clear hand IK, and reset state
    if (catHolding)
    {
        catHolding->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        catHolding = nullptr;
    }
    if (motionWarpingComponent)
    {
        motionWarpingComponent->RemoveWarpTarget(holdAnimWarpTargetName);
    }
    if (IsValid(catHolding))
    {
        catHolding->EndToBeHold();
        catHolding = nullptr;
    }
    SetInteraction(EHumanInteractionState::None);
}

// 7. special condition
void AMPCharacterHuman::BeStunned(int32 stunDuration)
{
    // Calculate playrate based on stunDuration (example: base duration / stunDuration)
    float baseStunAnimDuration = 1.0f; // Set this to your base animation duration in seconds
    float beStunnedMontagePlayRate = (stunDuration > 0) ? (baseStunAnimDuration / stunDuration) : 1.0f;
    SetContext(EHumanContext::BeStunned, true);
}

void AMPCharacterHuman::StopStunned()
{
	Super::StopStunned();
}
