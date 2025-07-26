#include "MPCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"

#include "../../CommonEnum.h"
#include "../../CommonStruct.h"
#include "../../HighLevel/MPLogManager.h"
#include "../../HighLevel/MPGMGameplay.h"

#include "../Player/MPControllerPlayer.h"
#include "../Player/MPPlayerState.h"
#include "../Item/MPItem.h"


AMPCharacter::AMPCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Preview camera setup
	PreviewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PreviewCamera"));
	PreviewCamera->SetupAttachment(RootComponent);
	PreviewCamera->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	PreviewCamera->bUsePawnControlRotation = false;
	PreviewCamera->SetAutoActivate(false); // Only activate when needed

	motionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("motionWarpingComponent"));
	motionWarpingComponent->SetupAttachment(RootComponent);
}

// common class methods
void AMPCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate critical properties for multiplayer
	DOREPLIFETIME(AMPCharacter, inventory);
	DOREPLIFETIME(AMPCharacter, curHoldingItemIndex);
	DOREPLIFETIME(AMPCharacter, curHoldingItem);
	DOREPLIFETIME(AMPCharacter, curSpeed);
	DOREPLIFETIME(AMPCharacter, isDoingAnAnimation);
	DOREPLIFETIME(AMPCharacter, bIsStunned);
}

void AMPCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeItems();
}

void AMPCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	// Only perform detection on the controlling client or server
	if (HasAuthority() || IsLocallyControlled())
	{
		Detect();
	}
	
	UpdateMovingControlsPerTick(deltaTime);
}

// interactable interface
bool AMPCharacter::IsInteractable(AMPCharacter* player)
{
	return false;
}

FText AMPCharacter::GetInteractHintText(AMPCharacter* player)
{
	if (IsInteractable(player))
	{
		return UMPLocalizationManager::Get()->GetLocalizedText(interactHintTextKey);
	}
	else
	{
		return UMPLocalizationManager::Get()->GetLocalizedText(uninteractableHintTextKey);
	}
}

void AMPCharacter::BeInteracted(AMPCharacter* player)
{
	return;
}

void AMPCharacter::PlaySoundLocally(USoundCue* aSound) 
{
	if (IsValid(aSound))
    {
        UGameplayStatics::PlaySoundAtLocation(this, aSound, GetActorLocation());
    }
}

void AMPCharacter::PlaySoundBroadcast(USoundCue* aSound)
{
	PlaySoundServer(aSound);
}

void AMPCharacter::PlaySoundServer_Implementation(USoundCue* aSound)
{
	if (HasAuthority())
    {
		PlaySoundMulticast(aSound);
	}
}

void AMPCharacter::PlaySoundMulticast_Implementation(USoundCue* aSound)
{
	PlaySoundLocally(aSound);
}

// detect 
void AMPCharacter::Detect()
{
	if (!IsValid(characterCamera)) 
	{
		return;
	}

	detectStart = characterCamera->GetComponentLocation();
	detectDirection = characterCamera->GetForwardVector();
	detectEnd = ((detectDirection * detectDistance) + detectStart);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(detectHit, detectStart, detectEnd, ECC_Visibility, DefaultComponentQueryParams, DefaultResponseParam);

	if (isAbleToFireTraceLine)
	{
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
	}
	DetectReaction();
}

void AMPCharacter::DetectReaction()
{
    // Default: clear interactable references
    detectInteractableActor = nullptr;
    detectedActor = nullptr;

    if (detectHit.bBlockingHit)
    {
        AActor* hitActor = detectHit.GetActor();

        // If hit, and is an actor
        if (!IsValid(hitActor))
        {
            curDetectHintText = detectInvalidHintText;
            return;
        }

        // If hit actor implements Interactable
        if (hitActor->GetClass()->ImplementsInterface(UMPInteractable::StaticClass()))
        {
            detectInteractableActor.SetObject(hitActor);
            detectInteractableActor.SetInterface(Cast<IMPInteractable>(hitActor));
            detectedActor = hitActor;
            curDetectHintText = IMPInteractable::Execute_GetInteractHintText(hitActor, this);
            return;
        }
        else
        {
            curDetectHintText = detectInvalidHintText;
            return;
        }
    }
    else
    {
        // No hit at all
        curDetectHintText = detectInvalidHintText;
        detectInteractableActor = nullptr;
        detectedActor = nullptr;
    }
}

// common properties
ETeam AMPCharacter::GetCharacterTeam()
{
	// Base implementation - should be overridden by derived classes
	return ETeam::EHuman;
}

// controller/ input reaction
void AMPCharacter::PossessedBy(AController* newController)
{
	Super::PossessedBy(newController);
	
	if (IsValid(newController))
	{
		UMPLogManager::LogInfo(TEXT("Character possessed by controller"), TEXT("MPCharacter"));
	}
}

void AMPCharacter::Look(FVector2D direction)
{
	if (!CheckIfIsAbleToLook()) return;

	// AddControllerPitchInput(direction.Y);
	// AddControllerYawInput(direction.X);
}

void AMPCharacter::Move(FVector2D direction)
{
	if (!CheckIfIsAbleToMove()) return;

	// Calculate movement direction
	FVector forward = GetActorForwardVector();
	FVector right = GetActorRightVector();
	FVector movement = forward * direction.Y + right * direction.X;
	movement.Normalize();

	// Apply movement
	AddMovementInput(movement, 1.0f);
}

void AMPCharacter::MoveStop()
{
	// Stop movement input
	AddMovementInput(FVector::ZeroVector, 0.0f);
}

void AMPCharacter::Run()
{
	if (!CheckIfIsAbleToRun()) return;

	curSpeed = runSpeed + extraSpeed;
	UpdateSpeed();
}

void AMPCharacter::RunStop()
{
	curSpeed = moveSpeed + extraSpeed;
	UpdateSpeed();
}

void AMPCharacter::CrouchStart()
{
	if (!CheckIfIsAbleToCrouch()) return;

	curSpeed = crouchSpeed + extraSpeed;
	UpdateSpeed();
}

void AMPCharacter::CrouchEnd()
{
	curSpeed = moveSpeed + extraSpeed;
	UpdateSpeed();
}

void AMPCharacter::JumpStart()
{
	if (!CheckIfIsAbleToJump()) return;

	Jump();
}

void AMPCharacter::JumpEnd()
{
	StopJumping();
}

void AMPCharacter::Interact()
{
	if (!CheckIfIsAbleToInteract()) return;

	if (detectInteractableActor.IsValid())
	{
		detectInteractableActor->BeInteracted(this);
	}
}

bool AMPCharacter::CheckIfIsAbleToLook()
{
	return !bIsStunned && !isDoingAnAnimation;
}

bool AMPCharacter::CheckIfIsAbleToMove()
{
	return !bIsStunned && !isDoingAnAnimation;
}

bool AMPCharacter::CheckIfIsAbleToRun()
{
	return CheckIfIsAbleToMove();
}

bool AMPCharacter::CheckIfIsAbleToCrouch()
{
	return CheckIfIsAbleToMove();
}

bool AMPCharacter::CheckIfIsAbleToJump()
{
	return CheckIfIsAbleToMove();
}

bool AMPCharacter::CheckIfIsAbleToDoubleJump()
{
	return false; // Base implementation - override in derived classes
}

bool AMPCharacter::CheckIfIsAbleToClimb()
{
	return false; // Base implementation - override in derived classes
}

bool AMPCharacter::CheckIfIsAbleToInteract()
{
	return !bIsStunned && !isDoingAnAnimation;
}

bool AMPCharacter::CheckIfIsAbleToUseItems()
{
	return IsValid(curHoldingItem) && !bIsStunned && !isDoingAnAnimation;
}

// movement related
void AMPCharacter::UpdateMovingControlsPerTick(float deltaTime)
{
	// Base implementation - can be overridden by derived classes
	return;
}

void AMPCharacter::UpdateSpeed()
{
	curSpeed = moveSpeed + extraSpeed;
	
	// Update character movement speed
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->MaxWalkSpeed = static_cast<float>(curSpeed);
	}
}

// inventory related
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
	if (!IsAbleToAddItem())
	{
		UMPLogManager::LogWarning(TEXT("Cannot add item - inventory full"), TEXT("MPCharacter"));
		return;
	}

	if (!GetWorld()) 
	{ 
		UMPLogManager::LogError(TEXT("World is null"), TEXT("MPCharacter"));
		return; 
	}

	AGameModeBase* curGameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (!IsValid(curGameMode)) 
	{ 
		UMPLogManager::LogError(TEXT("GameMode is null"), TEXT("MPCharacter"));
		return; 
	}

	AMPGMGameplay* curMPGameMode = Cast<AMPGMGameplay>(curGameMode);
	if (IsValid(curMPGameMode))
	{
		AMPItem* newItem = curMPGameMode->SpawnItem(aItemTag, GetActorLocation(), GetActorRotation());
		if (IsValid(newItem))
		{
			newItem->BeInitialized(this);
			newItem->BePickedUp(this);
			
			inventory.Add(newItem);
			UMPLogManager::LogDebug(FString::Printf(TEXT("Added item to inventory: %d"), static_cast<int32>(aItemTag)), TEXT("MPCharacter"));
			// Directly update HUD after adding item
			if (APlayerController* PC = Cast<APlayerController>(GetController())) {
				if (PC->IsLocalController()) {
					if (UHUDCharacter* HUD = Cast<UHUDCharacter>(PC->GetHUD())) {
						HUD->UpdateItemSlots();
						HUD->UpdateCenterImage();
					}
				}
			}
		}
		else
		{
			UMPLogManager::LogWarning(TEXT("Failed to spawn item"), TEXT("MPCharacter"));
		}
	}
}

bool AMPCharacter::IsAbleToAddItem()
{
	return inventory.Num() < inventoryCapacity;
}

bool AMPCharacter::IsInventoryFull()
{
	return inventory.Num() >= inventoryCapacity;
}

void AMPCharacter::PickupAnItem(AMPItem* aItem)
{
	if (!IsValid(aItem))
	{
		UMPLogManager::LogWarning(TEXT("Invalid item provided for pickup"), TEXT("MPCharacter"));
		return;
	}

	if (IsAbleToAddItem())
	{
		aItem->BePickedUp(this);
		inventory.Add(aItem);
		UMPLogManager::LogInfo(TEXT("Item picked up successfully"), TEXT("MPCharacter"));
	}
	else
	{
		UMPLogManager::LogWarning(TEXT("Cannot pickup item - inventory full"), TEXT("MPCharacter"));
	}
}

void AMPCharacter::DeleteItem(AMPItem* itemToDelete)
{
	if (!IsValid(itemToDelete)) 
	{ 
		UMPLogManager::LogWarning(TEXT("Invalid item provided for deletion"), TEXT("MPCharacter"));
		return; 
	}

	int32 deleteItemIndex = inventory.Find(itemToDelete);
	if (deleteItemIndex != INDEX_NONE)
	{
		// Remove from inventory
		inventory.RemoveAt(deleteItemIndex);
		
		// If this was the currently held item, clear it
		if (curHoldingItem == itemToDelete)
		{
			curHoldingItem = nullptr;
			curHoldingItemIndex = -1;
		}
		
		// Destroy the item
		if (IsValid(itemToDelete))
		{
			itemToDelete->Destroy();
		}
		
		UMPLogManager::LogInfo(TEXT("Item deleted from inventory"), TEXT("MPCharacter"));
		// Directly update HUD after deleting item
		if (APlayerController* PC = Cast<APlayerController>(GetController())) {
			if (PC->IsLocalController()) {
				if (UHUDCharacter* HUD = Cast<UHUDCharacter>(PC->GetHUD())) {
					HUD->UpdateItemSlots();
					HUD->UpdateCenterImage();
				}
			}
		}
	}
	else
	{
		UMPLogManager::LogWarning(TEXT("Item not found in inventory"), TEXT("MPCharacter"));
	}
}

// Replication callback functions
void AMPCharacter::OnRep_AbleToInteract()
{
	// Handle UI updates for interaction state changes
	// This will be called on clients when the server updates the interaction state
	return;
}

void AMPCharacter::OnRep_CurHoldingItemIndex()
{
	// Handle UI updates for item selection changes
	return;
}

void AMPCharacter::OnRep_CurHoldingItem()
{
	// Handle visual updates for currently held item
	return;
}

void AMPCharacter::OnRep_CurSpeed()
{
	// Update character movement speed on clients
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->MaxWalkSpeed = static_cast<float>(curSpeed);
	}
}

void AMPCharacter::SelectItem(int32 itemIndex)
{
    if (itemIndex < 0 || itemIndex >= inventory.Num())
    {
        UMPLogManager::LogWarning(TEXT("Invalid item index"), TEXT("MPCharacter"));
        return;
    }

    // If selecting the same item, deselect it
    if (curHoldingItemIndex == itemIndex)
    {
        UnselectCurItem();
        // Directly update HUD after deselection
        if (APlayerController* PC = Cast<APlayerController>(GetController())) {
            if (PC->IsLocalController()) {
                if (UHUDCharacter* HUD = Cast<UHUDCharacter>(PC->GetHUD())) {
                    HUD->UpdateItemSlots();
                    HUD->UpdateCenterImage();
                }
            }
        }
        return;
    }

    curHoldingItemIndex = itemIndex;
    curHoldingItem = inventory[itemIndex];
    // Directly update HUD after selection
    if (APlayerController* PC = Cast<APlayerController>(GetController())) {
        if (PC->IsLocalController()) {
            if (UHUDCharacter* HUD = Cast<UHUDCharacter>(PC->GetHUD())) {
                HUD->UpdateItemSlots();
                HUD->UpdateCenterImage();
            }
        }
    }
    UMPLogManager::LogInfo(FString::Printf(TEXT("Selected item at index %d"), itemIndex), TEXT("MPCharacter"));
}

void AMPCharacter::UnselectCurItem()
{
    curHoldingItemIndex = -1;
    curHoldingItem = nullptr;
    // Directly update HUD after unselect
    if (APlayerController* PC = Cast<APlayerController>(GetController())) {
        if (PC->IsLocalController()) {
            if (UHUDCharacter* HUD = Cast<UHUDCharacter>(PC->GetHUD())) {
                HUD->UpdateItemSlots();
                HUD->UpdateCenterImage();
            }
        }
    }
    UMPLogManager::LogInfo(TEXT("Unselected current item"), TEXT("MPCharacter"));
}

void AMPCharacter::UseCurItem()
{
	if (!IsAbleToUseCurItem()) return;

	if (IsValid(curHoldingItem))
	{
		curHoldingItem->BeUsed(detectedActor);
	}
}

bool AMPCharacter::IsAbleToUseCurItem()
{
	return IsValid(curHoldingItem) && CheckIfIsAbleToUseItems();
}

void AMPCharacter::DropCurItem()
{
	if (!IsValid(curHoldingItem)) return;

	curHoldingItem->BeDroped(this);
	curHoldingItem = nullptr;
	curHoldingItemIndex = -1;
	
	UMPLogManager::LogInfo(TEXT("Dropped current item"), TEXT("MPCharacter"));
	// Directly update HUD after dropping item
	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
		if (PC->IsLocalController()) {
			if (UHUDCharacter* HUD = Cast<UHUDCharacter>(PC->GetHUD())) {
				HUD->UpdateItemSlots();
				HUD->UpdateCenterImage();
			}
		}
	}
}

// animation state
void AMPCharacter::SetMovementMode(EMPMovementMode newMode)
{
	// Base implementation - should be overridden by derived classes
	return;
}

void AMPCharacter::SetAirState(EMovementAirStatus newAirState)
{
	// Base implementation - should be overridden by derived classes
	return;
}

// animation context/ montage
void AMPCharacter::PlaySelectedMontage(UAnimMontage* chosenMontage, float playRate)
{
	if (!IsValid(chosenMontage))
	{
		UMPLogManager::LogWarning(TEXT("Invalid animation montage provided"), TEXT("MPCharacter"));
		return;
	}

	// Get the mesh component
	USkeletalMeshComponent* meshComponent = GetMesh();
	if (!IsValid(meshComponent))
	{
		UMPLogManager::LogWarning(TEXT("Mesh component is null"), TEXT("MPCharacter"));
		return;
	}

	// Get the animation instance
	UAnimInstance* animInstance = meshComponent->GetAnimInstance();
	if (!IsValid(animInstance))
	{
		UMPLogManager::LogWarning(TEXT("Animation instance is null"), TEXT("MPCharacter"));
		return;
	}

	// Play the montage
	animInstance->Montage_Play(chosenMontage, playRate);
	isDoingAnAnimation = true;
	
	AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &AMPCharacter::OnMontageEnded);

	UMPLogManager::LogDebug(TEXT("Playing animation montage"), TEXT("MPCharacter"));
}

void AMPCharacter::OnMontageEnded(UAnimMontage* montage, bool bInterrupted)
{
	isDoingAnAnimation = false;
	OnMontageEndedContextClear(montage, bInterrupted);

	UMPLogManager::LogDebug(FString::Printf(TEXT("Animation montage ended - Interrupted: %s"), bInterrupted ? TEXT("Yes") : TEXT("No")), TEXT("MPCharacter"));
}

void AMPCharacter::OnMontageEndedContextClear(UAnimMontage* montage, bool bInterrupted)
{
	// Base implementation - can be overridden by derived classes
	// should be overrided using switch case for each context
	return;
}


void AMPCharacter::OnRep_IsDoingAnimation()
{
	// Handle animation state changes on clients
}

// special condition
void AMPCharacter::BeStunned(int32 stunDuration)
{
	if (bIsStunned) return;

	bIsStunned = true;
	
	// Set timer to clear stun
	GetWorld()->GetTimerManager().SetTimer(stunTimerHandle, this, &AMPCharacter::StopStunned, stunDuration, false);
	
	UMPLogManager::LogInfo(FString::Printf(TEXT("Character stunned for %d seconds"), stunDuration), TEXT("MPCharacter"));
}

void AMPCharacter::StopStunned()
{
	bIsStunned = false;
	GetWorld()->GetTimerManager().ClearTimer(stunTimerHandle);
	
	UMPLogManager::LogInfo(TEXT("Character unstunned"), TEXT("MPCharacter"));
}

void AMPCharacter::OnRep_IsStunned()
{
	// Handle stun state changes on clients
	if (bIsStunned)
	{
		UMPLogManager::LogInfo(TEXT("Character stunned"), TEXT("MPCharacter"));
	}
	else
	{
		UMPLogManager::LogInfo(TEXT("Character unstunned"), TEXT("MPCharacter"));
	}
}