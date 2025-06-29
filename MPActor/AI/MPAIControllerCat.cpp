#include "MPAIControllerCat.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../EnvActor/MPEnvActorComp.h"

AMPAIControllerCat::AMPAIControllerCat()
{
    
}

void AMPAIControllerCat::BeginPlay()
{
    Super::BeginPlay();

    UpdateWanderLocation();
}

void AMPAIControllerCat::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Update timer and check for interaction periodically
    InteractionTimer += DeltaSeconds;
    if (InteractionTimer >= InteractionInterval)
    {
        InteractionTimer = 0.f;
        CheckForEnvActorInteraction();
    }
}

// Uses the navigation system to find a random reachable point and sets it to the Blackboard.
void AMPAIControllerCat::UpdateWanderLocation()
{
    ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn());
    if (BlackboardComp && ControlledCharacter)
    {
        UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
        if (NavSys)
        {
            FNavLocation RandomLocation;
            if (NavSys->GetRandomReachablePointInRadius(ControlledCharacter->GetActorLocation(), MoveLocationRadius, RandomLocation))
            {
                BlackboardComp->SetValueAsVector(BB_MovingLocation, RandomLocation.Location);
            }
        }
    }
}

// Scans for nearby environment actors that are interactable.
// If one is found, updates the Blackboard to indicate an interaction should occur.
void AMPAIControllerCat::CheckForEnvActorInteraction()
{
    /*
    ACharacter* ControlledCharacter = Cast<Character*>(GetPawn());
    if (!ControlledCharacter || !BlackboardComp) return;

    TArray<AActor*> OverlappingActors;
    float DetectionRadius = 500.f; // adjust detection range as needed

    // Use sphere overlap to get nearby actors (requires proper collision channels in your project)
    UKismetSystemLibrary::SphereOverlapActors(GetWorld(),
        ControlledCharacter->GetActorLocation(),
        DetectionRadius,
        TArray<TEnumAsByte<EObjectTypeQuery>>(), // specify object types if needed
        AMEnvActorComp::StaticClass(),
        TArray<AActor*>(), // actors to ignore
        OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        AMPEnvActorComp* EnvActor = Cast<AMPEnvActorComp>(Actor);
        if (EnvActor && EnvActor->IsInteractable(ControlledCharacter))
        {
            // Found an interactable env actor—update blackboard.
            BlackboardComp->SetValueAsBool(BB_WantToInteract, true);
            BlackboardComp->SetValueAsObject(BB_EnvActorToInteract, EnvActor);
            return;
        }
    }
    // If no valid actor was found, clear the keys.
    BlackboardComp->SetValueAsBool(BB_WantToInteract, false);
    BlackboardComp->ClearValue(BB_EnvActorToInteract);*/
}