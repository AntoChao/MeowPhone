#include "MissionZone.h"
#include "../EnvActor/MPEnvActorComp.h"

AMissionZone::AMissionZone()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMissionZone::RandomlizeZone()
{
    USceneComponent* RootComp = GetRootComponent();
    if (!RootComp)
    {
        return;
    }

    // Get all descendant scene components in one call.
    TArray<USceneComponent*> AllDescendants;
    RootComp->GetChildrenComponents(true, AllDescendants);

    // To prevent duplicate processing, track already randomized EnvActorComp actors.
    TSet<AMPEnvActorComp*> ProcessedEnvActors;

    for (USceneComponent* Descendant : AllDescendants)
    {
        if (!Descendant)
        {
            continue;
        }

        AActor* Owner = Descendant->GetOwner();
        if (Owner && Owner->IsA(AMPEnvActorComp::StaticClass()))
        {
            AMPEnvActorComp* EnvActor = Cast<AMPEnvActorComp>(Owner);
            if (EnvActor && EnvActor->CheckIfIsRandomizable() && !ProcessedEnvActors.Contains(EnvActor))
            {
                EnvActor->BeRandomized();
                ProcessedEnvActors.Add(EnvActor);
            }
        }
    }
}