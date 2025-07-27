#include "FactoryAbility.h"
#include "../../MPActor/Ability/MPAbility.h"
#include "Kismet/GameplayStatics.h"

UFactoryAbility::UFactoryAbility()
{

}

AActor* UFactoryAbility::SpawnMPActor(int actorCode,
    FVector actorLocation, FRotator actorRotation)
{
    if (!gameWorld) { return nullptr; }

    // For now we only have one ability class – could map code to subclasses later.
    TSubclassOf<AActor> spawnClass = AMPAbility::StaticClass();
    spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    return gameWorld->SpawnActor<AActor>(spawnClass, actorLocation, actorRotation, spawnParams);
}
