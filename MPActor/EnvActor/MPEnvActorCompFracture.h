#pragma once

// [Meow-Phone Project]
//
// This Actor class is specifically designed to work with Unreal Engine's Chaos Fracture
// system. It represents a destructible object in the world that is composed of a
// Geometry Collection.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint class inheriting from this one (e.g., `BP_BreakableVase`).
// 2. The most important step is setting up the `envActorFractureBodyMesh` component. In the Blueprint editor, select this component and assign a `GeometryCollection` asset to it.
// 3. Place instances of this Blueprint in your level.
// 4. To break the object, other actors (like an ability or a projectile) should call the `ApplyForce` function on it. This will simulate a force on the Geometry Collection, causing it to fracture and break apart.
//
// Necessary things to define:
// - A `GeometryCollection` asset MUST be assigned to the `envActorFractureBodyMesh` component in the Blueprint. This asset is created in the Unreal Editor by fracturing a regular Static Mesh.
//
// How it interacts with other classes:
// - AActor: The base class.
// - UGeometryCollectionComponent: This is the core component from Unreal's Chaos physics system that handles the simulation and rendering of the fractured object. This class is essentially a wrapper around it.
// - Other Actors/Abilities: Any gameplay system that can cause damage or apply force will interact with this class by calling `ApplyForce` to trigger the destruction effect.

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "MPEnvActorCompFracture.generated.h"

UCLASS(Blueprintable, BlueprintType)
class AMPEnvActorCompFracture : public AActor
{
    GENERATED_BODY()

public:
    AMPEnvActorCompFracture();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fracture EnvActor Properties")
    USceneComponent* fractureSceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fracture EnvActor Properties")
    UGeometryCollectionComponent* envActorFractureBodyMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fracture EnvActor Properties")
    int lifeSpanSegs = 10;

public:
    UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Fracture EnvActor Methods")
    void ApplyForce(const FVector& Force);
};