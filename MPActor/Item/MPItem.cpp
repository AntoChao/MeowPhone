#include "MPItem.h"

#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"

#include "../../CommonStruct.h"

AMPItem::AMPItem()
{
	bReplicates = true;

	USceneComponent* root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(root);

	itemBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));

    itemBodyMesh->SetupAttachment(RootComponent);
    itemBodyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    itemBodyMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	itemCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ItemCollision"));
    itemCollision->SetupAttachment(RootComponent);
    itemCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    itemCollision->SetCollisionObjectType(ECC_WorldDynamic);
    itemCollision->SetCollisionResponseToAllChannels(ECR_Overlap);

    itemAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("ItemAudio"));
    itemAudioComp->SetupAttachment(RootComponent);
}

// interactable interface
bool AMPItem::IsInteractable(AMPCharacter* player)
{
	return !isPickedUp;
}

FText AMPItem::GetInteractHintText(AMPCharacter* player) 
{
	if (IsInteractable(player))
	{
		return interactableText;
	}
	else 
	{
		return uninteractableText;
	}
}

void AMPItem::BeInteracted(AMPCharacter* player)
{
	if (player)
	{
		player->PickupAnItem(this);
	}	
}

// initialize / interact
void AMPItem::BeInitialized(AMPCharacter* player)
{
	return;
}

void AMPItem::BePickedUp(AMPCharacter* player)
{
	isPickedUp = true;
	itemOwner = player;

	if (itemBodyMesh)
    {
        itemBodyMesh->SetVisibility(false);
		itemBodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (itemCollision)
    {
        itemCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}
void AMPItem::BeDroped(AMPCharacter* player)
{
	isPickedUp = false;
	
	if (itemBodyMesh)
    {
        itemBodyMesh->SetVisibility(true);
		itemBodyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    if (itemCollision)
    {
        itemCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
}

// usage
void AMPItem::BeUsed(AActor* targetActor)
{
	if (IsAbleToBeUsed(targetActor))
	{
		switch (itemType)
		{
			case EItemType::EDirectUse:
			{
				ApplyUsageEffectDirect(targetActor);
				EndUsageEffectDirect(targetActor); 
				break;
			}
			case EItemType::EDurationUse:
			{
				StartUsageEffectDuration(targetActor);
				break;
			}	
			default:
				break;
		}
	}
}
bool AMPItem::IsAbleToBeUsed(AActor* targetActor)
{
	return !isBeingUse && !isInCooldown;
}  

/* ApplyUsageEffectDirect(AActor* targetActor)
*	no need to save targetActor
*/
void AMPItem::ApplyUsageEffectDirect(AActor* targetActor)
{
	// effect ...
}

void AMPItem::EndUsageEffectDirect(AActor* targetActor)
{
	if (isSingleUse)
	{
		if (itemOwner)
		{
			itemOwner->DeleteItem(this);
		}
	}
	else 
	{
		StartCooldown();
	}
}


void AMPItem::StartUsageEffectDuration(AActor* targetActor)
{
	isBeingUse = true;
	targetActorSaved = targetActor;
	curUsageCountDown = totalUsageDuration;

	ApplyUsageEffectDuration();
}

void AMPItem::ApplyUsageEffectDuration()
{
	ApplyUsageEffectDurationEffect();
	ApplyUsageEffectDurationCountdown();
}
void AMPItem::ApplyUsageEffectDurationEffect()
{
	// effect ...
}
void AMPItem::ApplyUsageEffectDurationCountdown()
{
	if (curUsageCountDown > 0)
	{
		UWorld* serverWorld = GetWorld();
		if (serverWorld)
		{	
			curUsageCountDown -= 1;

			serverWorld->GetTimerManager().ClearTimer(usageTimerHandle);
			FTimerDelegate usageTimerDel;
			usageTimerDel.BindUFunction(this, FName("ApplyUsageEffectDuration"));
			serverWorld->GetTimerManager().SetTimer(usageTimerHandle, 
				usageTimerDel, 1, false);
		}
	}
	else 
	{
		ExpireUsageEffectDuration();
	}
}

void AMPItem::ExpireUsageEffectDuration()
{
	isBeingUse = false;
	targetActorSaved = nullptr;

	if (isSingleUse)
	{
		if (itemOwner)
		{
			itemOwner->DeleteItem(this);
		}
	}
	else 
	{
		StartCooldown();
	}
}
void AMPItem::StartCooldown()
{
	isInCooldown = true;
	curCooldownCountDown = totalCooldown;
	CooldownCountDown();
}
void AMPItem::CooldownCountDown()
{
	if (curCooldownCountDown > 0)
	{
		UWorld* serverWorld = GetWorld();

		if (serverWorld)
		{	
			curCooldownCountDown -=1;
	
			serverWorld->GetTimerManager().ClearTimer(cooldownTimerHandle);
			FTimerDelegate usageCooldownTimerDel;
			usageCooldownTimerDel.BindUFunction(this, FName("CooldownCountDown"));
			serverWorld->GetTimerManager().SetTimer(cooldownTimerHandle,
				usageCooldownTimerDel, totalCooldown, false);
		}
	}
	else 
	{
		EndCooldown();
	}
}
void AMPItem::EndCooldown()
{
	isInCooldown = false;
}

void AMPItem::GetEliminated()
{
	Destroy();
}