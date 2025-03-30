#include "MPEnvActorComp.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Components/AudioComponent.h"

#include "../../CommonStruct.h"

AMPEnvActorComp::AMPEnvActorComp()
{
	PrimaryActorTick.bCanEverTick = false;

    // Setup scene root
    envActorSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = envActorSceneRoot;

    // Setup mesh and attach to root
    envActorBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    envActorBodyMesh->SetupAttachment(RootComponent);

    // Audio remain the same
    envActorAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
    envActorAudioComp->SetupAttachment(RootComponent);
}

void AMPEnvActorComp::BeRandomized()
{
	return;
}

bool AMPEnvActorComp::IsInteractable(AMPCharacter* targetActor)
{
	return false;
}

FText AMPEnvActorComp::GetInteractHintText(AMPCharacter* targetActor)
{
	return envActorName;
}

void AMPEnvActorComp::BeInteracted(AMPCharacter* targetActor)
{

	GEngine->AddOnScreenDebugMessage(6, 5.0f, FColor::Yellow, TEXT("EnvActor Be Interacted"));

	if (IsInteractable(targetActor))
	{
		switch (envActorType)
		{
			case EEnvActorType::EDirectInteract:
			{
				GEngine->AddOnScreenDebugMessage(6, 5.0f, FColor::Yellow, TEXT("EnvActor Be Interacted: Applying Direct Effect"));

				ApplyInteractEffectDirect(targetActor);
				EndInteractEffectDirect(targetActor);
				break;
			}
			case EEnvActorType::EDurationInteract:
			{
				GEngine->AddOnScreenDebugMessage(6, 5.0f, FColor::Yellow, TEXT("EnvActor Be Interacted: Applying Duration Effect"));

				StartInteractEffectDuration(targetActor);
				break;
			}	
			default:
				break;
		}
	}
}

void AMPEnvActorComp::ApplyInteractEffectDirect(AMPCharacter* targetActor)
{
	// just override to apply effect
	return;
}
void AMPEnvActorComp::EndInteractEffectDirect(AMPCharacter* targetActor)
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

void AMPEnvActorComp::StartInteractEffectDuration(AMPCharacter* targetActor)
{
	isInteracting = true;
	interactedCharacter = targetActor;
	curInteractCountDown = totalInteractDuration;

	ApplyInteractEffectDuration();
}

void AMPEnvActorComp::ApplyInteractEffectDuration()
{
	ApplyInteractEffectDurationEffect();
	ApplyInteractEffectDurationCountdown();
}
void AMPEnvActorComp::ApplyInteractEffectDurationEffect()
{
	// override to apply effect
	
}
void AMPEnvActorComp::ApplyInteractEffectDurationCountdown()
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

void AMPEnvActorComp::ExpireInteractEffectDuration()
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
void AMPEnvActorComp::StartCooldown()
{
	isInCooldown = true;
	curCooldownCountDown = totalCooldown;
	CooldownCountDown();
}
void AMPEnvActorComp::CooldownCountDown()
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
void AMPEnvActorComp::EndCooldown()
{
	isInCooldown = false;
}

bool AMPEnvActorComp::CheckIfIsRandomizable()
{
	return isAbleToBeRandomlized;
}