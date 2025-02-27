#include "FactoryAbility.h"

UFactoryAbility::UFactoryAbility()
{

}

AActor* UFactoryAbility::SpawnMPActor(int actorCode,
    FVector actorLocation, FRotator actorRotation)
{
    return nullptr;
}

UObject* UFactoryAbility::SpawnMPObject(AActor* owner, int actorCode)
{
    return nullptr;
}
