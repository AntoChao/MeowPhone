#include "FactoryItem.h"

UFactoryItem::UFactoryItem()
{

}

AActor* UFactoryItem::SpawnMPActor(int actorCode,
    FVector actorLocation, FRotator actorRotation)
{
    return nullptr;
}

UObject* UFactoryItem::SpawnMPObject(AActor* owner, int actorCode)
{
    return nullptr;
}
