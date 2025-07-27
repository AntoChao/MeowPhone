#include "MPAbility.h"
#include "Net/UnrealNetwork.h"
#include "../../CommonEnum.h"
#include "../../CommonStruct.h"
#include "../Character/MPCharacterCat.h"


AMPAbility::AMPAbility()
{
	bReplicates = true;
}

void AMPAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMPAbility, isBeingUse);
    DOREPLIFETIME(AMPAbility, isInCooldown);
}

void AMPAbility::OnRep_BeingUse() {}
void AMPAbility::OnRep_InCooldown() {}

// common Ability methods
void AMPAbility::BeInitialized(AMPCharacterCat* player)
{
	abilityOwner = player;
	ownerWorld = abilityOwner->GetWorld();
}

EAbility AMPAbility::GetAbilityTag()
{
	return abilityTag;
}

// usage
void AMPAbility::BeUsed(AActor* targetActor)
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
bool AMPAbility::IsAbleToBeUsed(AActor* targetActor)
{
	return !isBeingUse && !isInCooldown;
}  

/* ApplyUsageEffectDirect(AActor* targetActor)
*	no need to save targetActor
*/
void AMPAbility::ApplyUsageEffectDirect(AActor* targetActor)
{
	// effect ...
}
void AMPAbility::EndUsageEffectDirect(AActor* targetActor)
{
	StartCooldown();
}

void AMPAbility::StartUsageEffectDuration(AActor* targetActor)
{
	isBeingUse = true;
	targetActorSaved = targetActor;
	curUsageCountDown = totalUsageDuration;

	ApplyUsageEffectDuration();
}

void AMPAbility::ApplyUsageEffectDuration()
{
	ApplyUsageEffectDurationEffect();
	ApplyUsageEffectDurationCountdown();
}
void AMPAbility::ApplyUsageEffectDurationEffect()
{
	// effect ...
}
void AMPAbility::ApplyUsageEffectDurationCountdown()
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

void AMPAbility::ExpireUsageEffectDuration()
{
	isBeingUse = false;
	targetActorSaved = nullptr;

	StartCooldown();
}

void AMPAbility::StartCooldown()
{
	isInCooldown = true;
	curCooldownCountDown = totalCooldown;
	CooldownCountDown();
}

void AMPAbility::CooldownCountDown()
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
void AMPAbility::EndCooldown()
{
	isInCooldown = false;
}