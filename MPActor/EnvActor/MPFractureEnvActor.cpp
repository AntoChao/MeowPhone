#include "MPFractureEnvActor.h"

AMPFractureEnvActor::AMPFractureEnvActor()
{
    // Create and set the root component as a SceneComponent
    fractureSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = fractureSceneRoot;

    // Create the Geometry Collection Component
    pieceFractureBodyMesh = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("PieceFractureMeshComponent"));
    pieceFractureBodyMesh->SetupAttachment(RootComponent);

    // Set the actor to automatically destroy after 10 seconds
    SetLifeSpan(lifeSpanSegs);
}

void AMPFractureEnvActor::ApplyForce(const FVector& Force)
{
    if (envActorFractureBodyMesh)
    {
        // pieceFractureBodyMesh->AddForce(Force, NAME_None, true);
        pieceFractureBodyMesh->AddImpulse(Force, NAME_None, true);
    }
}