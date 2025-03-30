#include "MPCharacterCat.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../PLayer/MPPlayerState.h"
#include "Components/CapsuleComponent.h"

#include "../../CommonEnum.h"
#include "../../CommonStruct.h"

#include "../../HighLevel/MPGMGameplay.h"
#include "MPCharacterHuman.h"
#include "../Ability/MPAbility.h"

AMPCharacterCat::AMPCharacterCat()
{
	bUseControllerRotationYaw = false;

    cameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Character SpringArm"));
    cameraSpringArm->SetupAttachment(RootComponent);
    cameraSpringArm->TargetArmLength = 300.0f; 
    cameraSpringArm->bUsePawnControlRotation = true;
		
    characterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Character Camera"));
    characterCamera->SetupAttachment(cameraSpringArm, USpringArmComponent::SocketName);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void AMPCharacterCat::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeAllAbility();
}

void AMPCharacterCat::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	UpdateDriftStatus(deltaTime);
}

// cat move function 
void AMPCharacterCat::UpdateDriftStatus(float deltaTime)
{
	// Update current speed and record last velocity direction (for drift momentum)
	FVector velocity = GetVelocity();
	CurrentSpeed = velocity.Size();
	if (CurrentSpeed > KINDA_SMALL_NUMBER)
	{
		LastVelocityDir = velocity.GetSafeNormal();
	}

	// --- Drift Detection: Compare LastVelocityDir with DesiredMoveDirection ---
	float angleBetween = 0.0f;
	if (!LastVelocityDir.IsNearlyZero() && !DesiredMoveDirection.IsNearlyZero())
	{
		float dot = FVector::DotProduct(LastVelocityDir, DesiredMoveDirection);
		dot = FMath::Clamp(dot, -1.0f, 1.0f);
		angleBetween = FMath::RadiansToDegrees(FMath::Acos(dot));
	}
	
	bool bShouldDrift = (CurrentSpeed > MinSpeedForDrift && angleBetween > DriftStartAngle);
	
	// --- Movement Input Smoothing ---
	// Determine target strength: 1.0 if input exists, else 0.0.
	float targetInputStrength = bHasMoveInput ? 1.0f : 0.0f;
	
	// Choose interpolation rate (accelerate if increasing, decelerate if decreasing).
	float interpRate = (CurrentMoveInputStrength < targetInputStrength) ? AccelerationRate : DecelerationRate;
	CurrentMoveInputStrength = FMath::FInterpTo(CurrentMoveInputStrength, targetInputStrength, deltaTime, interpRate);
	
	// --- Apply Drift or Normal Movement ---
	if (bShouldDrift)
	{
		// Initialize drift on the first frame of drifting.
		if (!bIsDrifting)
		{
			DriftElapsedTime = 0.0f;
			DriftRecoveryAlpha = DriftSlowdownMultiplier;
			bIsDrifting = true;
		}
		
		// Update drift recovery over time (recovering from slowdown).
		DriftElapsedTime += deltaTime;
		float t = FMath::Clamp(DriftElapsedTime / DriftRecoveryTime, 0.0f, 1.0f);
		DriftRecoveryAlpha = FMath::Lerp(DriftSlowdownMultiplier, 1.0f, t);
		
		// Apply movement input using the momentum direction (LastVelocityDir)
		// multiplied by the smoothed input strength and drift recovery factor.
		AddMovementInput(LastVelocityDir, DriftRecoveryAlpha * CurrentMoveInputStrength);
		
		// Rotate slowly toward the desired input direction.
		FRotator targetRot = DesiredMoveDirection.Rotation();
		FRotator newRot = FMath::RInterpConstantTo(GetActorRotation(), targetRot, deltaTime, DriftTurnRate * 100.0f);
		SetActorRotation(newRot);
	}
	else
	{
		// If we were drifting but now conditions are cleared, reset drift state.
		if (bIsDrifting)
		{
			DriftElapsedTime = 0.0f;
			DriftRecoveryAlpha = 1.0f;
			bIsDrifting = false;
		}
		
		// Normal movement: apply input in the desired direction.
		if (CurrentMoveInputStrength > KINDA_SMALL_NUMBER)
		{
			AddMovementInput(DesiredMoveDirection, CurrentMoveInputStrength);
		}
		else
		{
			// If fully decelerated, set locomotion state to idle.
			SetLocomotionState(EMovementLocomotion::EIdle);
		}
		
		// Rotate quickly toward the desired direction.
		FRotator targetRot = DesiredMoveDirection.Rotation();
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), targetRot, deltaTime, 8.0f));
	}
	
	// (Optional) Update other states or trigger animations based on bIsDrifting, CurrentSpeed, etc.
}

void AMPCharacterCat::MoveStop()
{
	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Yellow, TEXT("Character: Move Stop"));
	bHasMoveInput = false;
	// SetLocomotionState(EMovementLocomotion::EIdle);
}

// double jump
bool AMPCharacterCat::IsFootNearWall()
{
	const float capsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    const FVector start = GetActorLocation() - FVector(0, 0, capsuleHalfHeight * wallDetectionHeightPercentage); // near the feet

    const FVector forward = GetActorForwardVector();
    const FVector end = start + forward * 60.0f + FVector(0, 0, wallDetectDownward); // slight downward slope

    const float sphereRadius = wallDetectRadius;
    FHitResult hit;
    FCollisionQueryParams params;
    params.AddIgnoredActor(this);

    const bool bHit = GetWorld()->SweepSingleByChannel(
        hit,
        start,
        end,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeSphere(sphereRadius),
        params
    );

		// Optional: visual debug
	#if WITH_EDITOR
		DrawDebugLine(GetWorld(), start, end, FColor::Blue, false, 1.0f);
		DrawDebugSphere(GetWorld(), end, sphereRadius, 12, bHit ? FColor::Green : FColor::Red, false, 1.0f);
	#endif

    // If we hit, check the surface normal to ensure it's a mostly vertical wall
    if (bHit)
    {
        const FVector wallNormal = hit.ImpactNormal;
        const float verticalDot = FVector::DotProduct(wallNormal, FVector::UpVector);
        return verticalDot < wallDetectionAngleTolerance; // Surface is mostly vertical
    }

    return false;
}

bool AMPCharacterCat::CheckIfIsAbleToDoubleJump()
{
	return curAirState == EMovementAirStatus::EJump && IsFootNearWall();
}

// ability
void AMPCharacterCat::InitializeAllAbility()
{
	InitializeActiveAbility();
    InitializeAllPassiveAbility();
}

void AMPCharacterCat::InitializeActiveAbility()
{
	if (GetWorld()) { return; }

	activeAbility = nullptr;
	AMPPlayerState* curPlayerState = Cast<AMPPlayerState>(GetPlayerState());
	AGameModeBase* curGameMode = UGameplayStatics::GetGameMode(GetWorld());
	AMPGMGameplay* curMPGameMode = Cast<AMPGMGameplay>(curGameMode);

	if (curPlayerState && curMPGameMode)
	{
		activeAbility = curMPGameMode->SpawnAbility(this, curPlayerState->playerSelectedAbility);
		if (activeAbility)
		{
			activeAbility->BeInitialized(this);
		}
	}
}
void AMPCharacterCat::InitializeAllPassiveAbility()
{
	allPassiveAbilities.Empty();
	AMPGMGameplay* curGameMode = Cast<AMPGMGameplay>(UGameplayStatics::GetGameMode(GetWorld()));
	
	if (curGameMode)
	{
		int i = 0;
		for (EAbility eachAbilityTag : initPassiveAbilities)
		{
			allPassiveAbilities[i] = curGameMode->SpawnAbility(this, eachAbilityTag);
			if (allPassiveAbilities[i])
			{
				allPassiveAbilities[i]->BeInitialized(this);
				i ++;
			}
		}
	}
}

void AMPCharacterCat::UseActiveAbility()
{
	if (activeAbility)
	{
		activeAbility->BeUsed(detectedActor);
	}
}

void AMPCharacterCat::UsePassiveAbility(EAbility abilityType)
{
	for (UMPAbility* eachPassiveAbility : allPassiveAbilities)
	{
		if (eachPassiveAbility->GetAbilityTag() == abilityType)
		{
			eachPassiveAbility->BeUsed(detectedActor);
		}
	}
}

// interactable interface
bool AMPCharacterCat::IsInteractable(AMPCharacter* player)
{
	return !isBeingHolded;
}

FText AMPCharacterCat::GetInteractHintText(AMPCharacter* player)
{
	if (IsInteractable(player))
	{
		switch(player->GetCharacterTeam())
		{
			case ETeam::EHuman :
			{
				return humanInteractHintText;
			}
			case ETeam::ECat :
			{
				return catInteractHintText;
			}
		}
	}
	else
	{
		switch(player->GetCharacterTeam())
		{
			case ETeam::EHuman :
			{
				return uninteractableHumanHintText;
			}
			case ETeam::ECat :
			{
				return uninteractableCatHintText;
			}
		}
	}

	return uninteractableHumanHintText;
}

void AMPCharacterCat::BeInteracted(AMPCharacter* player)
{
	if (IsInteractable(player))
	{
		switch(player->GetCharacterTeam())
		{
			case ETeam::EHuman :
			{
				StartedToBeHold(player);
				
				// notify humanPlayer
				humanHolding->StartHoldingCat(this);
			}
			case ETeam::ECat :
			{
				return;
				// what should cat interact with other cat
			}
		}
	}
}

bool AMPCharacterCat::CheckIfIsAbleToLook()
{	
	return true;
}
bool AMPCharacterCat::CheckIfIsAbleToMove()
{
	return !isBeingHolded && !isRubbing;
}
bool AMPCharacterCat::CheckIfIsAbleToInteract()
{
	return true;
}

void AMPCharacterCat::Move(FVector2D direction)
{
	// normally move
	if (direction.IsNearlyZero())
	{
		bHasMoveInput = false;
		return;
	}
	if (!CheckIfIsAbleToMove())
	{
		return;
	}


	// Mark that we have move input.
	bHasMoveInput = true;

	// Convert 2D input into a normalized 3D world direction.
	FVector inputDir = (direction.X * GetActorRightVector()) + (direction.Y * GetActorForwardVector());
	inputDir.Z = 0.0f;
	inputDir.Normalize();

	// Store desired movement direction for use in Tick().
	DesiredMoveDirection = inputDir;


	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Yellow, TEXT("Character: Move"));
	SetLocomotionState(EMovementLocomotion::EWalk);

	FRotator ControlRotation = GetControlRotation();
	ControlRotation.Pitch = 0;

	FVector CameraForwardVector = UKismetMathLibrary::GetForwardVector(ControlRotation);
	FVector CameraRightVector = UKismetMathLibrary::GetRightVector(ControlRotation);
	CameraForwardVector.Z = 0; // Flatten the vector to the horizontal plane
	CameraRightVector.Z = 0;
	CameraForwardVector.Normalize();
	CameraRightVector.Normalize();

	FVector MoveDirection = CameraForwardVector * direction.Y + CameraRightVector * direction.X;
	MoveDirection.Normalize();

	AddMovementInput(MoveDirection, 1.0f);

	if (!MoveDirection.IsNearlyZero())
	{
		FRotator TargetRotation = MoveDirection.Rotation();
		TargetRotation.Pitch = 0;
		TargetRotation.Roll = 0;

		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), rotationRate);

		SetActorRotation(NewRotation);
	}
}

void AMPCharacterCat::StartedToBeHold(AMPCharacter* humanPlayer)
{
	// update cat stats
	isBeingHolded = true;
	humanHolding = Cast<AMPCharacterHuman>(humanPlayer);
	curHoldTime = 0;
}
void AMPCharacterCat::EndToBeHold()
{
	humanHolding->StopHoldingCat();

	isBeingHolded = false;
	humanHolding = nullptr;
	curHoldTime = 0;
}
void AMPCharacterCat::Straggle()
{
	curHoldTime = curHoldTime + holdModifier;
	curHoldPercentage = curHoldTime / holdTotalTime;

	if (curHoldTime >= holdTotalTime)
	{
		EndToBeHold();
	}
}

void AMPCharacterCat::StartToRub(AMPCharacterHuman* humanToRub)
{
	isRubbing = true;
    humanRubbing = humanToRub;
}
void AMPCharacterCat::StopToRub()
{
	isRubbing = false;
    humanRubbing = nullptr;
}

// controller/ input reaction
/*	Interact() -> curCatAction
*		beingHold -> straggle
*		detectActor -> human -> interactHuman
* 		detectActor -> cat -> interactCat
* 		detectActor -> envActor -> envActor
* 		detectActor -> item -> pickup	
*/
void AMPCharacterCat::Interact()
{
	GEngine->AddOnScreenDebugMessage(5, 5.0f, FColor::Yellow, TEXT("Cat Interact 1"));

	if (CheckIfIsAbleToInteract())
	{
		GEngine->AddOnScreenDebugMessage(5, 5.0f, FColor::Yellow, TEXT("Cat Interact 2"));

		if (isRubbing)
		{
			StopToRub();
		}
		else if (isBeingHolded)
		{
			Straggle();
		}
		else if (detectInteractableActor)
		{
			GEngine->AddOnScreenDebugMessage(5, 5.0f, FColor::Yellow, TEXT("Cat Interact 3"));

			detectInteractableActor->BeInteracted(this);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(5, 5.0f, FColor::Yellow, TEXT("Cat Interact 4"));
		}
	}
}

// animation
void AMPCharacterCat::PlayCatAnimMontage(ECatAnimMontage aMontage)
{
	
}