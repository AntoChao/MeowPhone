#include "MPAbility.h"
#include "Net/UnrealNetwork.h"
#include "../../CommonEnum.h"
#include "../../CommonStruct.h"
#include "../Character/MPCharacterCat.h"


UMPAbility::UMPAbility()
{
	bReplicates = true;
}

void UMPAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UMPAbility, isBeingUse);
    DOREPLIFETIME(UMPAbility, isInCooldown);
}

void UMPAbility::OnRep_BeingUse() {}
void UMPAbility::OnRep_InCooldown() {}

// common Ability methods
void UMPAbility::BeInitialized(AMPCharacterCat* player)
{
	abilityOwner = player;
	ownerWorld = abilityOwner->GetWorld();
}

EAbility UMPAbility::GetAbilityTag()
{
	return abilityTag;
}

// usage
void UMPAbility::BeUsed(AActor* targetActor)
{
	if (IsAbleToBeUsed(targetActor))
	{
		switch (abilityType)
		{
			case EAbilityType::EDirectUse:
			{
				ApplyUsageEffectDirect(targetActor);
				EndUsageEffectDirect(targetActor);
				break;
			}
			case EAbilityType::EDurationUse:
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
		if (ownerWorld)
		{	
			curUsageCountDown -= 1;

			ownerWorld->GetTimerManager().ClearTimer(usageTimerHandle);
			FTimerDelegate usageTimerDel;
			usageTimerDel.BindUFunction(this, FName("ApplyUsageEffectDuration"));
			ownerWorld->GetTimerManager().SetTimer(usageTimerHandle,
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
		if (ownerWorld)
		{	
			curCooldownCountDown -=1;

			ownerWorld->GetTimerManager().ClearTimer(cooldownTimerHandle);
			FTimerDelegate usageCooldownTimerDel;
			usageCooldownTimerDel.BindUFunction(this, FName("CooldownCountDown"));
			ownerWorld->GetTimerManager().SetTimer(cooldownTimerHandle,
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