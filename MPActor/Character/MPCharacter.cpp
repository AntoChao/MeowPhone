#include "MPCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "../../CommonEnum.h"
#include "../../CommonStruct.h"
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
	Super::Tick(DeltaTime);

	Detect();
}

// detect 

void AMPCharacter::Detect()
{
	if (IsValid(characterCamera)) {
		detectStart = characterCamera->GetComponentLocation();
		detectDirection = characterCamera->GetForwardVector();
		detectEnd = ((detectDirection * detectDistance) + detectStart);
		
		GetWorld()->LineTraceSingleByChannel(detectHit, detectStart, detectEnd, ECC_Visibility, DefaultComponentQueryParams, DefaultResponseParam);
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
	for (EMPItem eachItemTag : initItems)
	{
		AddAnItem(eachItemTag);
	}
}

void AMPCharacter::AddAnItem(EMPItem aItemTag)
{
	if (IsAbleToAddItem())
	{
		AGameModeBase* curGameMode = UGameplayStatics::GetGameMode(GetWorld());
		AMPGMGameplay* curMPGameMode = Cast<AMPGMGameplay>(curGameMode);

		if (curMPGameMode)
		{
			AMPItem* newItem = curMPGameMode->SpawnItem(this, aItemTag, GetActorLocation(), GetActorRotation());
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
	return;
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

void AMPCharacter::Look(FVector direction)
{
	if (isAbleToLook)
	{
		AddControllerYawInput(direction.X * GetWorld()->GetDeltaSeconds());
		AddControllerPitchInput(-1 * direction.Y * GetWorld()->GetDeltaSeconds());
	}
}
void AMPCharacter::Move(FVector direction)
{
	if (isAbleToMove)
	{
		AddMovementInput(GetActorForwardVector(), direction.Y);
		AddMovementInput(GetActorRightVector(), direction.X);
	}
}

void AMPCharacter::Run()
{
	if (isAbleToRun)
	{
		isRunning = true;
		curSpeed = moveSpeed + runSpeed + extraSpeed;
		UpdateSpeed();
	}
}
void AMPCharacter::RunStop()
{
	isRunning = false;
	curSpeed = moveSpeed + extraSpeed;
	UpdateSpeed();
}
void AMPCharacter::Jump()
{
	if (isAbleToJump)
	{
		Jump();
	}
}
void AMPCharacter::JumpEnd()
{
	StopJumping();
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
			curHoldingItem->BeUsed(this, detectedActor);
		}
	}
}
bool AMPCharacter::IsAbleToUseCurItem()
{
	if (curHoldingItem)
	{
		return curHoldingItem->IsAbleToBeUsed(this, detectedActor);
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
		AMPPlayerState* mpPS = Cast<AMPPlayerState>(mpPC->GetPlayerState());
		if (mpPS)
		{
			return playerTeam;
		}		
	}

	return ETeam::ECat;
}