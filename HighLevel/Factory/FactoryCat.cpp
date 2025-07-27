#include "FactoryCat.h"
#include "../../MPActor/Character/MPCharacterCat.h"

UFactoryCat::UFactoryCat()
{

}

AActor* UFactoryCat::SpawnMPActor(int actorCode,
    FVector actorLocation, FRotator actorRotation)
{
    if (gameWorld)
    {
        switch (actorCode)
        {
            case 0 :
            {
                if (catEXPClass)
                {
                    spawnedActor = gameWorld->SpawnActor<AActor>(catEXPClass, actorLocation, actorRotation);
                }
                break;
            }
            case 3:
            {
                if (catEXPClass)
                {
                    spawnedActor = gameWorld->SpawnActor<AActor>(catDiedClass, actorLocation, actorRotation);
                }
                break;
            }
        }
    }
    return spawnedActor;
}