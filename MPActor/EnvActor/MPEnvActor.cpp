#include "MPEnvActor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Components/AudioComponent.h"

#include "../../CommonStruct.h"

AMPEnvActor::AMPEnvActor()
{

}

bool AMPEnvActor::IsInteractable(AMPCharacter* targetActor)
{
	return false;
}

FText AMPEnvActor::GetInteractHintText(AMPCharacter* targetActor)
{
	return envActorName;
}

void AMPEnvActor::BeInteracted(AMPCharacter* targetActor)
{
	if (IsInteractable(targetActor))
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

void AMPEnvActor::ApplyInteractEffectDirect(AMPCharacter* targetActor)
{
	// just override to apply effect
}
void AMPEnvActor::EndInteractEffectDirect(AMPCharacter* targetActor)
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

void AMPEnvActor::StartInteractEffectDuration(AMPCharacter* targetActor)
{
	isInteracting = true;
	interactedCharacter = targetActor;
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
	isInteracting = false;
	interactedCharacter = nullptr;

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
		UWorld* serverWorld = GetWorld();
		if (serverWorld)
		{	
			curCooldownCountDown -= 1;

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