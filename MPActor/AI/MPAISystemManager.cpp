#include "MPAISystemManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "../AI/MPAIControllerHumanPlayer.h"
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
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMPAIControllerHumanPlayer::StaticClass(), allActors);

    for (AActor* eachActor : allActors)
    {
        AMPAIControllerHumanPlayer* eachHumanController = Cast<AMPAIControllerHumanPlayer>(eachActor);
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
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMPEnvActorComp::StaticClass(), allActors);

    for (AActor* eachActor : allActors)
    {
        AMPEnvActorComp* eachEnvActorComp = Cast<AMPEnvActorComp>(eachActor);
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
void AMPAISystemManager::ReceiveUrgentNotification(AMPEnvActorComp* eventActor)
{
    if (!allUrgentEnvActor.Contains(eventActor))
    {
        allUrgentEnvActor.Add(eventActor);
        AllocateUrgent();
    }
}

void AMPAISystemManager::AllocateUrgent()
{
    // Example: Iterate over all spawned AI characters and trigger an urgent behavior.
    for (AMPAIControllerHumanPlayer* eachHumanController : allAIHumanControllers)
    {
        if (eachHumanController && !eachHumanController->IsBusyWithGlobalTask())
        {
            eachHumanController->AssignGlobalTask(allUrgentEnvActor.Last());
            break;
        }
    }
}