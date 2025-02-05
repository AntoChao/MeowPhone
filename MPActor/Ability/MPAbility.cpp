#include "MPAbility.h"
#include "../../CommonEnum.h"
#include "../../CommonStruct.h"
#include "../Character/MPCharacterCat.h"


UMPAbility::UMPAbility()
{
	
}

// common Ability methods
bool UMPAbility::BeInitialized(AMPCharacter* player)
{
	return;
}

// usage
void UMPAbility::BeUsed(AActor* targetActor)
{
	if (IsAbleToBeUsed(player, targetActor))
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
bool UMPAbility::IsAbleToBeUsed(AActor* targetActor)
{
	return !isBeingUse && !isInCooldown;
}  

/* ApplyUsageEffectDirect(AActor* targetActor)
*	no need to save targetActor
*/
void UMPAbility::ApplyUsageEffectDirect(AActor* targetActor)
{
	// effect ...
}
void UMPAbility::EndUsageEffectDirect(AActor* targetActor)
{
	StartCooldown();
}

void UMPAbility::StartUsageEffectDuration(AActor* targetActor)
{
	isBeingUse = true;
	targetActorSaved = targetActor;
	curUsageCountDown = totalUsageDuration;

	ApplyUsageEffectDuration();
}

void UMPAbility::ApplyUsageEffectDuration()
{
	ApplyUsageEffectDurationEffect();
	ApplyUsageEffectDurationCountdown();
}
void UMPAbility::ApplyUsageEffectDurationEffect()
{
	// effect ...
}
void UMPAbility::ApplyUsageEffectDurationCountdown()
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

void UMPAbility::ExpireUsageEffectDuration()
{
	isBeingUse = false;
	targetActorSaved = nullptr;

	StartCooldown();
}
void UMPAbility::StartCooldown()
{
	isInCooldown = true;
	curCooldownCountDown = totalCooldown;
	CooldownCountDown();
}
void UMPAbility::CooldownCountDown()
{
	if (curCooldownCountDown > 0)
	{
		if (serverWorld)
		{	
			curCooldownCountDown -=1;

			UWorld* serverWorld = GetWorld();
			serverWorld->GetTimerManager().ClearTimer(usageCooldownTimerHandle);
			FTimerDelegate usageCooldownTimerDel;
			usageCooldownTimerDel.BindUFunction(this, FName("CooldownCountDown"));
			serverWorld->GetTimerManager().SetTimer(usageCooldownTimerHandle, 
				usageCooldownTimerDel, totalCooldown, false);
		}
	}
	else 
	{
		EndCooldown();
	}
}
void UMPAbility::EndCooldown()
{
	isInCooldown = false;
}