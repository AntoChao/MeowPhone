#include "MPFactory.h"

UMPFactory::UMPFactory()
{

}

void UMPFactory::SetGameWorld(UWorld* aWorld)
{
    gameWorld = aWorld;
}

AActor* UMPFactory::SpawnMPActor(int actorCode,
    FVector actorLocation, FRotator actorRotation)
{
    return spawnedActor;
}

UObject* UMPFactory::SpawnMPObject(AActor* owner, int actorCode)
{
    return spawnedObject;
}
