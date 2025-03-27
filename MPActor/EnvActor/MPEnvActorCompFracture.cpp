#include "MPEnvActorCompFracture.h"

AMPEnvActorCompFracture::AMPEnvActorCompFracture()
{
    // Create and set the root component as a SceneComponent
    fractureSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = fractureSceneRoot;

    // Create the Geometry Collection Component
    envActorFractureBodyMesh = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("PieceFractureMeshComponent"));
    envActorFractureBodyMesh->SetupAttachment(RootComponent);

    // Set the actor to automatically destroy after 10 seconds
    SetLifeSpan(lifeSpanSegs);
}

void AMPEnvActorCompFracture::ApplyForce(const FVector& Force)
{
    if (envActorFractureBodyMesh)
    {
        envActorFractureBodyMesh->AddImpulse(Force, NAME_None, true);
    }
}