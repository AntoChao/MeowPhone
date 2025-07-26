#pragma once

#include "CoreMinimal.h"
#include "MPEnvActorComp.h"

#include "MPEnvActorCompPushable.generated.h"

class AMPEnvActorCompFracture;

UCLASS(BlueprintType, Blueprintable)
class AMPEnvActorCompPushable : public AMPEnvActorComp
{
    GENERATED_BODY()

public:
    AMPEnvActorCompPushable();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
        void OnRep_AlreadyPushed();
    UFUNCTION()
        void OnRep_PushedCounter();
    UFUNCTION()
        void OnRep_HasContributedToProgression();

    // Common methods
protected:
    virtual void BeginPlay() override;
    virtual void BeginDestroy() override;
    virtual void BeRandomized() override;

    // interactable interface
public:
    virtual bool IsInteractable(AMPCharacter* targetActor) override;

    // interact specific
protected:
    virtual void ApplyInteractEffectDirect(AMPCharacter* targetActor);

    // Pushable
protected:
    UPROPERTY(ReplicatedUsing = OnRep_AlreadyPushed, BlueprintReadWrite, Category = "Push Attribute")
    bool isAlreadyPushed = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push Attribute")
    float pushForce = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push Attribute")
    int32 stunDuration = 1; // Duration in seconds for stun effect

    // Threshold push count logic
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push Attribute")
    int pushThreshold = 1;

    UPROPERTY(ReplicatedUsing = OnRep_PushedCounter, BlueprintReadWrite, Category = "Push Attribute")
    int pushedCounter = 0;

    UFUNCTION()
        void OnRep_PushedCounter();

    // breakable
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push Attribute")
    bool isBreakable = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push Attribute")
    float breakableThreshold = 500000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Push Attribute")
    TSubclassOf<AMPEnvActorCompFracture> fracturedItemClass;

    // Progression weight for cat team objective
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression Properties")
    float progressionWeight = 1.0f;

    // Flag to prevent double-counting progression
    UPROPERTY(ReplicatedUsing = OnRep_HasContributedToProgression, BlueprintReadWrite, Category = "Progression Properties")
    bool hasContributedToProgression = false;

public:
    UFUNCTION(BlueprintCallable, Category = "Progression Methods")
    float GetProgressionWeight() const { return progressionWeight; }

    UFUNCTION(BlueprintCallable, Category = "Progression Methods")
    void UpdateCatTeamProgression();

    UFUNCTION(BlueprintCallable, Category = "Progression Methods")
    void OnObjectDestroyed();

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
        UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};