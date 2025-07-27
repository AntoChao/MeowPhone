#include "FactoryAIController.h"
#include "../../CommonEnum.h"

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

AMPAIController* UFactoryAIController::SpawnAIController(ETeam team)
{
    if (!gameWorld)
    {
        return nullptr;
    }
    
    AMPAIController* aiController = nullptr;
    
    switch (team)
    {
        case ETeam::ECat:
        {
            if (catAIControllerClass)
            {
                aiController = gameWorld->SpawnActor<AMPAIController>(catAIControllerClass);
            }
            break;
        }
        case ETeam::EHuman:
        {
            if (humanAIControllerClass)
            {
                aiController = gameWorld->SpawnActor<AMPAIController>(humanAIControllerClass);
            }
            break;
        }
        default:
            break;
    }
    
    return aiController;
}
