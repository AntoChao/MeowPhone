#include "MPEnvActorCompPushable.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "../../HighLevel/Managers/ManagerLog.h"
#include "../../HighLevel/MPGS.h"
#include "../Character/MPCharacter.h"
#include "../Character/MPCharacterCat.h"
#include "../Character/MPCharacterHuman.h"

AMPEnvActorCompPushable::AMPEnvActorCompPushable()
{
	envActorTag = EEnvActor::EPushable;
	envActorType = EEnvActorType::EDirectInteract;

	PrimaryActorTick.bCanEverTick = false;

    envActorBodyMesh->SetSimulatePhysics(false);
    envActorBodyMesh->SetNotifyRigidBodyCollision(true); // To receive OnHit
}

void AMPEnvActorCompPushable::BeginPlay()
{
    Super::BeginPlay();

    if (envActorBodyMesh)
    {
        envActorBodyMesh->OnComponentHit.AddDynamic(this, &AMPEnvActorCompPushable::OnHit);
    }
}

void AMPEnvActorCompPushable::BeginDestroy()
{
    // Update progression before destruction if not already contributed
    OnObjectDestroyed();
    
    Super::BeginDestroy();
}

void AMPEnvActorCompPushable::BeRandomized()
{
    Destroy();
}

bool AMPEnvActorCompPushable::IsInteractable(AMPCharacter* targetActor)
{
	return true;
}

void AMPEnvActorCompPushable::ApplyInteractEffectDirect(AMPCharacter* targetActor)
{
    if (!HasAuthority()) { return; }

    if (isAlreadyPushed) { return; }

    if (envActorBodyMesh && targetActor)
    {
        FVector pushDir = targetActor->GetActorForwardVector();
        envActorBodyMesh->AddImpulse(pushDir * pushForce);
    }

    pushedCounter++;
    OnRep_PushedCounter();

    if (pushedCounter >= pushThreshold)
    {
        isAlreadyPushed = true;
        OnRep_AlreadyPushed();
        
        // Update progression when object is "completed" through pushing
        UpdateCatTeamProgression();
        
        UManagerLog::LogInfo(TEXT("Pushable object completed through pushing - Progression Updated"), TEXT("MPEnvActorCompPushable"));
    }
}

void AMPEnvActorCompPushable::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    UManagerLog::LogInfo(TEXT("Pushable EnvActor OnHIT called"), TEXT("MPEnvActorCompPushable"));

    float impactForce = NormalImpulse.Size();

    // Check if the hit actor is a character and stun them
    AMPCharacter* hitCharacter = Cast<AMPCharacter>(OtherActor);
    if (hitCharacter && stunDuration > 0)
    {
        UManagerLog::LogInfo(TEXT("Character hit by pushable object - STUNNED!"), TEXT("MPEnvActorCompPushable"));
        hitCharacter->BeStunned(stunDuration); // This will call the appropriate override
    }

    if (isBreakable && impactForce >= breakableThreshold)
    {
        UManagerLog::LogInfo(TEXT("Object broke due to impact!"), TEXT("MPEnvActorCompPushable"));

        // Update progression before destroying the object
        UpdateCatTeamProgression();

        if (fracturedItemClass)
        {
            AMPEnvActorCompFracture* fractureActor = GetWorld()->SpawnActor<AMPEnvActorCompFracture>(fracturedItemClass, GetActorTransform());
			if (fractureActor)
			{
				fractureActor->ApplyForce(NormalImpulse);
			}
        }

        Destroy();
    }
}

void AMPEnvActorCompPushable::UpdateCatTeamProgression()
{
    if (!HasAuthority()) { return; }
    if (hasContributedToProgression) { return; } // Prevent double-counting

    // Get the game mode and update progression
    AGameModeBase* gameMode = UGameplayStatics::GetGameMode(GetWorld());
    AMPGMGameplay* mpGameMode = Cast<AMPGMGameplay>(gameMode);
    
    if (mpGameMode)
    {
        AMPGS* gameState = Cast<AMPGS>(mpGameMode->GameState);
        if (gameState)
        {
            // Update progression with this object's weight
            gameState->UpdateMPProgression(static_cast<int>(progressionWeight));
            
            // Mark as contributed to prevent double-counting (this will be replicated)
            hasContributedToProgression = true;
            
            UManagerLog::LogInfo(FString::Printf(TEXT("Cat Team Progression Updated: +%f (Total: %f/%f)"), 
                progressionWeight, gameState->curMPProgression, gameState->totalMPProgression), TEXT("MPEnvActorCompPushable"));
            
            // Display current progression status (server only)
            mpGameMode->DisplayProgressionStatus();
            
            // Check for immediate win condition
            mpGameMode->CheckIfGameEnd();
        }
    }
}

void AMPEnvActorCompPushable::OnObjectDestroyed()
{
    if (!HasAuthority()) { return; }
    
    // Update progression if not already contributed
    if (!hasContributedToProgression)
    {
        UpdateCatTeamProgression();
    }
}

void AMPEnvActorCompPushable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMPEnvActorCompPushable, isAlreadyPushed);
    DOREPLIFETIME(AMPEnvActorCompPushable, pushedCounter);
    DOREPLIFETIME(AMPEnvActorCompPushable, hasContributedToProgression);
}

void AMPEnvActorCompPushable::OnRep_AlreadyPushed(){}

void AMPEnvActorCompPushable::OnRep_PushedCounter(){}

void AMPEnvActorCompPushable::OnRep_HasContributedToProgression(){}