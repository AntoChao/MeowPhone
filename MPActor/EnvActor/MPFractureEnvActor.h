#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "MPFractureEnvActor.generated.h"

UCLASS(Blueprintable, BlueprintType)
class AMPFractureEnvActor : public AActor
{
    GENERATED_BODY()

public:
    AMPFractureEnvActor();

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