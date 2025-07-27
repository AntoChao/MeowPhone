#pragma once

// [Meow-Phone Project]
//
// This class represents a "pushable" environmental actor. It's a dynamic object that can
// be interacted with (pushed) by a cat, potentially contributing to the cat team's objectives.
// It can also be configured to break after a certain impact force.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint inheriting from this class (e.g., `BP_PushableVase`).
// 2. In the Blueprint's Static Mesh component, you MUST enable "Simulate Physics". This class relies on physics for its core behavior.
// 3. Configure its properties in the defaults:
//    - `pushForce`: How much force is applied when a cat interacts with it.
//    - `pushThreshold`: How many times it must be pushed before it contributes to the game's progression score.
//    - `isBreakable`: If true, the object will shatter on a heavy impact.
//    - `breakableThreshold`: The impact force required to break it.
//    - `fracturedItemClass`: If `isBreakable` is true, you MUST assign a `BP_MPEnvActorCompFracture` Blueprint here. This is the actor that will be spawned to show the shattered pieces.
//    - `progressionWeight`: How much this object is worth to the cat team's score when successfully pushed/broken.
//
// Necessary things to define:
// - The Static Mesh must have "Simulate Physics" enabled.
// - If `isBreakable` is true, `fracturedItemClass` must be set to a valid `AMPEnvActorCompFracture` Blueprint.
//
// How it interacts with other classes:
// - AMPEnvActorComp: Inherits base interaction functionality.
// - AMPCharacterCat: Cats are the intended interactors. When a cat interacts, `ApplyInteractEffectDirect` is called, which applies a physics impulse to the object.
// - AMPEnvActorCompFracture: If the object is breakable, it spawns an instance of this class to replace itself upon destruction.
// - OnHit event: It listens for physics collisions. If a collision is strong enough (`breakableThreshold`), it triggers the breaking logic.
// - GameState (`AMPGS`): When pushed enough times or broken, it calls `UpdateCatTeamProgression` (likely via the Game Mode) to update the global game score.
// - Replication: `isAlreadyPushed`, `pushedCounter`, and `hasContributedToProgression` are all replicated to keep clients in sync with the object's state.

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