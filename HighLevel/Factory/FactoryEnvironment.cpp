#include "FactoryEnvironment.h"

UFactoryEnvironment::UFactoryEnvironment()
{

}

AActor* UFactoryEnvironment::SpawnMPActor(int actorCode,
    FVector actorLocation, FRotator actorRotation)
{
    return nullptr;
}

UObject* UFactoryEnvironment::SpawnMPObject(AActor* owner, int actorCode)
{
    return nullptr;
}
