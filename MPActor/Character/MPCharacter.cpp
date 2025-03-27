#include "MPCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "../../CommonEnum.h"
#include "../../CommonStruct.h"

#include "../../HighLevel/MPGMGameplay.h"
#include "../Player/MPControllerPlayer.h"
#include "../Player/MPPlayerState.h"

#include "../Item/MPItem.h"

AMPCharacter::AMPCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
}

// common class methods
void AMPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME(AMPCharacter, selectedMaterial);
}

void AMPCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeItems();
}

void AMPCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	Detect();
	UpdateMovingControls();
}

// interactable interface
bool AMPCharacter::IsInteractable(AMPCharacter* player)
{
	return false;
}

FText AMPCharacter::GetInteractHintText(AMPCharacter* player)
{
	return FText::FromString(TEXT("Interactable Actor"));
}

void AMPCharacter::BeInteracted(AMPCharacter* player)
{
	return;
}

// detect 
void AMPCharacter::Detect()
{
	if (IsValid(characterCamera)) {
		detectStart = characterCamera->GetComponentLocation();
		detectDirection = characterCamera->GetForwardVector();
		detectEnd = ((detectDirection * detectDistance) + detectStart);
		
		bool bHit = GetWorld()->LineTraceSingleByChannel(detectHit, detectStart, detectEnd, ECC_Visibility, DefaultComponentQueryParams, DefaultResponseParam);
		
		FColor LineColor = bHit ? FColor::Green : FColor::Red;
		DrawDebugLine(
			GetWorld(),
			detectStart,
			detectEnd,
			LineColor,
			false,       // Persistent lines? False means temporary
			0.5f,        // Duration in seconds
			0,           // Depth priority (default)
			2.0f         // Thickness
		);

		DetectReaction();
	}
}
void AMPCharacter::DetectReaction()
{
	if (detectHit.bBlockingHit)
	{
		detectedActor = detectHit.GetActor();
		UClass* detectedClass = detectedActor->GetClass();

		if (detectedActor && detectedClass->ImplementsInterface(UMPInteractable::StaticClass()))
		{
			// safely retrieve the interface
            IMPInteractable* detectedInterface = Cast<IMPInteractable>(detectedActor);
            if (detectedInterface)
            {
                // create TScriptInterface for interaction
                TScriptInterface<IMPInteractable> scriptInterface;
				scriptInterface.SetObject(detectedActor);
				scriptInterface.SetInterface(detectedInterface);

				// update inspected actor
				detectInteractableActor = scriptInterface;

				ableToInteractCurrently = true;
				curDetectHintText = detectInteractableActor->GetInteractHintText(this);
            }
			else 
			{
				ableToInteractCurrently = false;
				curDetectHintText = invalidDetectHintText;
			}
		}
		else 
		{
			ableToInteractCurrently = false;
			curDetectHintText = invalidDetectHintText;
		}
	}
	else 
	{
		ableToInteractCurrently = false;
		curDetectHintText = invalidDetectHintText;
	}
}

// inventory
void AMPCharacter::InitializeItems()
{
	inventory.Empty();
	for (EItem eachItemTag : initItems)
	{
		AddAnItem(eachItemTag);
	}
}

void AMPCharacter::AddAnItem(EItem aItemTag)
{
	if (IsAbleToAddItem())
	{
		if (!GetWorld()) { return; }

		AGameModeBase* curGameMode = UGameplayStatics::GetGameMode(GetWorld());
		AMPGMGameplay* curMPGameMode = Cast<AMPGMGameplay>(curGameMode);

		if (curMPGameMode)
		{
			AMPItem* newItem = curMPGameMode->SpawnItem(aItemTag, GetActorLocation(), GetActorRotation());
			if (newItem)
			{
				newItem->BeInitialized(this);
				newItem->BePickedUp(this);
				
				inventory.Add(newItem);
			}
		}
	}
}

bool AMPCharacter::IsAbleToAddItem()
{
	return !IsInventoryFull();
}

bool AMPCharacter::IsInventoryFull()
{
	return inventoryCapacity == inventory.Num();
}

void AMPCharacter::PickupAnItem(AMPItem* aItem)
{
	if (IsAbleToAddItem())
	{
		aItem->BePickedUp(this);
		
		inventory.Add(aItem);
	}
}
void AMPCharacter::DeleteItem(AMPItem* itemToDelete)
{
	bool deleteItemFound = false;
	int deleteItemIndex = 0;
	
	for (int i = 0; i < inventory.Num(); i++)
	{
		if (inventory[i] == itemToDelete)
		{
			deleteItemFound = true;
			deleteItemIndex = i;
		}
	}

	if (deleteItemFound)
	{
		inventory[deleteItemIndex]->Destroy();
		// or itemToDelete->Destroy();
		inventory[deleteItemIndex] = nullptr;
	}
}

// controller/ input reaction
void AMPCharacter::PossessedBy(AController* newController)
{
	Super::PossessedBy(newController);
}
void AMPCharacter::UpdateMovingControls()
{
	if (GetCharacterMovement()->IsFalling())
	{
		if (GetVelocity().Z > 0)
		{
			isJumpGotInterupted = false;
			isJumping = true;
			isFalling = false;
		}
		else if (GetVelocity().Z <= 0)
		{
			isJumpGotInterupted = false;
			isJumping = false;
			isFalling = true;
		}
	}
	else if (!GetCharacterMovement()->IsFalling())
	{
		if (isJumping)
		{
			isJumpGotInterupted = true;
		}
		isJumping = false;
		isFalling = false;

	}
}
bool AMPCharacter::CheckIfIsAbleToLook()
{
	return true;
}
bool AMPCharacter::CheckIfIsAbleToMove()
{
	return true;
}
void AMPCharacter::UpdateSpeed()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = curSpeed;
	}
}
bool AMPCharacter::CheckIfIsAbleToInteract()
{
	return true;
}

void AMPCharacter::Look(FVector2D direction)
{
	bool testValue = CheckIfIsAbleToLook();
	if (CheckIfIsAbleToLook())
	{
		AddControllerYawInput(direction.X * GetWorld()->GetDeltaSeconds());
		AddControllerPitchInput(-1 * direction.Y * GetWorld()->GetDeltaSeconds());
	}
}
void AMPCharacter::Move(FVector2D direction)
{
	if (CheckIfIsAbleToMove())
	{
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Yellow, TEXT("Character: Move"));

		isMoving = true;
		AddMovementInput(GetActorForwardVector(), direction.Y);
		AddMovementInput(GetActorRightVector(), direction.X);
	}
}
void AMPCharacter::MoveStop()
{
	GEngine->AddOnScreenDebugMessage(2, 5.0f, FColor::Yellow, TEXT("Character: Move Stop"));
	isMoving = false;
}

void AMPCharacter::Run()
{
	if (isAbleToRun)
	{
		GEngine->AddOnScreenDebugMessage(3, 5.0f, FColor::Yellow, TEXT("Character: Run"));
		isRunning = true;
		curSpeed = moveSpeed + runSpeed + extraSpeed;
		UpdateSpeed();
	}
}
void AMPCharacter::RunStop()
{
	GEngine->AddOnScreenDebugMessage(4, 5.0f, FColor::Yellow, TEXT("Character: Run Stop"));
	isRunning = false;
	curSpeed = moveSpeed + extraSpeed;
	UpdateSpeed();
}
void AMPCharacter::JumpStart()
{
	if (isAbleToJump)
	{
		GEngine->AddOnScreenDebugMessage(5, 5.0f, FColor::Yellow, TEXT("Character: Jump"));
		Jump();
	}
}
void AMPCharacter::JumpEnd()
{
	GEngine->AddOnScreenDebugMessage(6, 5.0f, FColor::Yellow, TEXT("Character: Jump End"));
	StopJumping();
}

void AMPCharacter::Interact()
{
	return;
}

void AMPCharacter::SelectItem(int itemIndex)
{
	if (curHoldingItemIndex == itemIndex)
	{
		// double select -> unselect
		UnselectCurItem();
	}
	else 
	{
		if (itemIndex >= 0 && itemIndex < inventory.Num())
		{
			curHoldingItemIndex = itemIndex;
			curHoldingItem = inventory[itemIndex];
		}
		else
		{
			UnselectCurItem();
		}
	}
}
void AMPCharacter::UnselectCurItem()
{
	curHoldingItemIndex = -1;
	curHoldingItem = nullptr;
}
void AMPCharacter::UseCurItem()
{
	if (curHoldingItem)
	{
		if (IsAbleToUseCurItem())
		{
			curHoldingItem->BeUsed(detectedActor);
		}
	}
}
bool AMPCharacter::IsAbleToUseCurItem()
{
	if (curHoldingItem)
	{
		return curHoldingItem->IsAbleToBeUsed(detectedActor);
	}

	return false;
}
void AMPCharacter::DropCurItem()
{
	if (curHoldingItem)
	{
		curHoldingItem->BeDroped(this);
		inventory[curHoldingItemIndex] = nullptr;
		curHoldingItemIndex = -1;
	}
}

ETeam AMPCharacter::GetCharacterTeam()
{
	AMPControllerPlayer* mpPC = Cast<AMPControllerPlayer>(GetController());
	if (mpPC)
	{
		AMPPlayerState* mpPS = Cast<AMPPlayerState>(mpPC->PlayerState);
		if (mpPS)
		{
			return mpPS->playerTeam;
		}		
	}

	return ETeam::ECat;
}