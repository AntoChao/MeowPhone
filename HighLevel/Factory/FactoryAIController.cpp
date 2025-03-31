#include "FactoryAIController.h"

UFactoryAIController::UFactoryAIController()
{

}

AActor* UFactoryAIController::SpawnMPActor(int actorCode,
    FVector actorLocation, FRotator actorRotation)
{
    if (gameWorld)
    {
        switch (actorCode)
        {
            case 0 :
            {
                if (catAIControllerClass)
                {
                    spawnedActor = gameWorld->SpawnActor<AActor>(catAIControllerClass, actorLocation, actorRotation);
                }
                break;
            }
            case 3:
            {
                if (humanAIControllerClass)
                {
                    spawnedActor = gameWorld->SpawnActor<AActor>(humanAIControllerClass, actorLocation, actorRotation);
                }
                break;
            }
        }
    }
    return spawnedActor;
}

UObject* UFactoryAIController::SpawnMPObject(AActor* owner, 
    int actorCode)
{
    return nullptr;
}
