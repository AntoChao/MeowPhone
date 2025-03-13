#pragma once

#include "CoreMinimal.h"
#include "MPEnvActor.h"

#include "MPEnvActorPushable.generated.h"

class AMPFractureEnvActor;

UCLASS(BlueprintType, Blueprintable)
class AMPEnvActorPushable : public AMPEnvActor
{
    GENERATED_BODY()

public:
    AMPEnvActorPushable();

    // Common methods
protected:
    virtual void BeginPlay() override;

    // interactable interface
public:
    virtual bool IsInteractable(AMPCharacter* targetActor) override;

    virtual FText GetInteractHintText(AMPCharacter* targetActor) override;

    // interact specific
protected:
    virtual void ApplyInteractEffectDirect(AMPCharacter* targetActor);

    // Pushable
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push Attribute")
    bool isAlreadyPushed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push Attribute")
    float pushForce = 1000.0f;

    // breakable
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push Attribute")
    bool isBreakable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push Attribute")
    float breakableThreshold = 500000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push Attribute")
    TSubclassOf<AMPFractureEnvActor> FracturedVersionClass;

public:
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
               UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};