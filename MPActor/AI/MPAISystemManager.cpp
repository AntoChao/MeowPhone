#include "MPAISystemManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "MPAIControllerHuman.h"
#include "../EnvActor/MPEnvActorComp.h"

AMPAISystemManager::AMPAISystemManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

// initialize
void AMPAISystemManager::Initialize()
{
    LocateAIHumans();
    LocateUrgentEnvActors();
}
void AMPAISystemManager::LocateAIHumans()
{
    TArray<AActor*> allActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMPAIControllerHuman::StaticClass(), allActors);

    for (AActor* eachActor : allActors)
    {
        AMPAIControllerHuman* eachHumanController = Cast<AMPAIControllerHuman>(eachActor);
        if (eachHumanController)
        {
            eachHumanController->SetAISystem(this);
            allAIHumanControllers.Add(eachHumanController);
        }
    }
}
void AMPAISystemManager::LocateUrgentEnvActors()
{
    TArray<AActor*> allActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnvActorComp::StaticClass(), allActors);

    for (AActor* eachActor : allActors)
    {
        AEnvActorComp* eachEnvActorComp = Cast<AEnvActorComp>(eachActor);
        if (eachEnvActorComp)
        {
            if (eachEnvActorComp->CheckCanCauseUrgentEvent())
            {
                eachEnvActorComp->SetAISystem(this);
            }
        }
    }
}

// urgent event
void AMPAISystemManager::ReceiveUrgentNotification(AEnvActorComp* eventActor)
{
    if (!allUrgentEnvActor.Contains(AEnvActorComp))
    {
        allUrgentEnvActor.Add(eventActor);
        AllocateUrgent();
    }
}

void AMPAISystemManager::AllocateUrgent()
{
    // Example: Iterate over all spawned AI characters and trigger an urgent behavior.
    for (AMPCharacter* AICharacter : SpawnedAICharacters)
    {
        if (human && !human->IsBusyWithGlobalTask())
        {
            human->AssignGlobalTask(allUrgentEnvActor.Last());
            break;
        }
    }
}