#include "MPEnvActorComp.h"
#include "Net/UnrealNetwork.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Components/AudioComponent.h"

#include "../../CommonStruct.h"
#include "../AI/MPAISystemManager.h"

#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

AMPEnvActorComp::AMPEnvActorComp()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

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
	if (IsInteractable(targetActor))
	{
		return UMPLocalizationManager::Get()->GetLocalizedText(interactHintTextKey);
	}
	else
	{
		return UMPLocalizationManager::Get()->GetLocalizedText(uninteractableHintTextKey);
	}
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

		if (isAbleToCauseUrgentEvent && theAISystem)
		{
			theAISystem->ReceiveUrgentNotification(this);
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

// =====================
// PlaySound interface
// =====================

void AMPEnvActorComp::PlaySoundLocally(USoundCue* aSound)
{
    if (aSound)
    {
        if (envActorAudioComp)
        {
            envActorAudioComp->SetSound(aSound);
            envActorAudioComp->Play();
        }
        else
        {
            UGameplayStatics::PlaySoundAtLocation(this, aSound, GetActorLocation());
        }
    }
}

void AMPEnvActorComp::PlaySoundBroadcast(USoundCue* aSound)
{
    if (HasAuthority())
    {
        PlaySoundMulticast(aSound);
    }
    else
    {
        PlaySoundServer(aSound);
    }
}

void AMPEnvActorComp::PlaySoundServer_Implementation(USoundCue* aSound)
{
    PlaySoundMulticast(aSound);
}

void AMPEnvActorComp::PlaySoundMulticast_Implementation(USoundCue* aSound)
{
    PlaySoundLocally(aSound);
}

// setter and getter
bool AMPEnvActorComp::CheckIfIsRandomizable()
{
	return isAbleToBeRandomlized;
}

bool AMPEnvActorComp::CheckCanCauseUrgentEvent()
{
	return isAbleToCauseUrgentEvent;
}

void AMPEnvActorComp::SetAISystem(AMPAISystemManager* aAIManager)
{
    theAIManager = aAIManager;
}

bool AMPEnvActorComp::CheckIfIsInteractableByCat()
{
	return isAbleToBeInteractedByCat;
}

void AMPEnvActorComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMPEnvActorComp, isInteracting);
    DOREPLIFETIME(AMPEnvActorComp, isInCooldown);
}

void AMPEnvActorComp::OnRep_Interacting()
{
    // Placeholder for UI/FX sync when interaction state changes
}

void AMPEnvActorComp::OnRep_InCooldown()
{
    // Placeholder for UI/FX sync when cooldown ends/starts
}