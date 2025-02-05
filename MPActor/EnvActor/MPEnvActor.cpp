#include "MPEnvActor.h"

#include "TimeManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"

#include "../../CommonStruct.h"

AMPEnvActor::AMPEnvActor()
{

}

bool AMPEnvActor::IsInteractable(AMPCharacter* player)
{
	return;
}

FLocalizedText AMPEnvActor::GetInteractHintText()
{
	return;
}

void AMPEnvActor::BeInteracted(AMPCharacter* player)
{
	if (IsAbleToBeUsed(player, targetActor))
	{
		switch (envActorType)
		{
			case EEnvActorType::EDirectInteract:
			{
				ApplyInteractEffectDirect(targetActor);
				EndInteractEffectDirect(targetActor);
				break;
			}
			case EEnvActorType::EDurationInteract:
			{
				StartInteractEffectDuration(targetActor);
				break;
			}	
			default:
				break;
		}
	}
}

void AMPEnvActor::ApplyInteractEffectDirect(AActor* targetActor)
{
	// just override to apply effect
}
void AMPEnvActor::EndInteractEffectDirect(AActor* targetActor)
{
	if (isSingleUse)
	{
		Destroy();
	}
	else 
	{
		StartCooldown();
	}
}

void AMPEnvActor::StartInteractEffectDuration(AActor* targetActor)
{
	isBeingUse = true;
	targetActorSaved = targetActor;
	curInteractCountDown = totalInteractDuration;

	ApplyInteractEffectDuration();
}

void AMPEnvActor::ApplyInteractEffectDuration()
{
	ApplyInteractEffectDurationEffect();
	ApplyInteractEffectDurationCountdown();
}
void AMPEnvActor::ApplyInteractEffectDurationEffect()
{
	// override to apply effect
	
}
void AMPEnvActor::ApplyInteractEffectDurationCountdown()
{
	if (curInteractCountDown > 0)
	{
		UWorld* serverWorld = GetWorld();
		if (serverWorld)
		{	
			curInteractCountDown -= 1;

			serverWorld->GetTimerManager().ClearTimer(interactTimerHandle);
			FTimerDelegate interactTimerDel;
			interactTimerDel.BindUFunction(this, FName("ApplyInteractEffectDuration"));
			serverWorld->GetTimerManager().SetTimer(interactTimerHandle, 
				interactTimerDel, 1, false);
		}
	}
	else 
	{
		ExpireInteractEffectDuration();
	}
}

void AMPEnvActor::ExpireInteractEffectDuration()
{
	isBeingUse = false;
	targetActorSaved = nullptr;

	if (isSingleUse)
	{
		Destroy();
	}
	else 
	{
		StartCooldown();
	}
}
void AMPEnvActor::StartCooldown()
{
	isInCooldown = true;
	curCooldownCountDown = totalCooldown;
	CooldownCountDown();
}
void AMPEnvActor::CooldownCountDown()
{
	if (curCooldownCountDown > 0)
	{
		if (serverWorld)
		{	
			curCooldownCountDown -= 1;

			UWorld* serverWorld = GetWorld();
			serverWorld->GetTimerManager().ClearTimer(cooldownTimerHandle);
			FTimerDelegate cooldownTimerDel;
			cooldownTimerDel.BindUFunction(this, FName("CooldownCountDown"));
			serverWorld->GetTimerManager().SetTimer(cooldownTimerHandle, 
				cooldownTimerDel, totalCooldown, false);
		}
	}
	else 
	{
		EndCooldown();
	}
}
void AMPEnvActor::EndCooldown()
{
	isInCooldown = false;
}