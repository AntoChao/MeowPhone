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

	// GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void AMPCharacterCat::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeAllAbility();
}

void AMPCharacterCat::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
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
	return (curAirState == EMovementAirStatus::EJump 
		|| curAirState == EMovementAirStatus::EFalling) 
		&& IsFootNearWall();
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

	// Convert 2D input into a normalized 3D world direction.
	FVector inputDir = (direction.X * GetActorRightVector()) + (direction.Y * GetActorForwardVector());
	inputDir.Z = 0.0f;
	inputDir.Normalize();

	// Store desired movement direction for use in Tick().

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
/*	Interact() -> curCatInteractionAction
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