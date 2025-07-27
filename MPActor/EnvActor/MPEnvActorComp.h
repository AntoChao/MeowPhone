#pragma once

// [Meow-Phone Project]
//
// This is the base class for all interactive and dynamic environmental actors. Despite the
// "Comp" in its name, it is an Actor, not a component. It provides a robust framework for
// creating objects that players and AI can interact with, complete with replication, cooldowns,
// interaction durations, and hooks for AI systems.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint class inheriting from this class (or one of its children like `AMPEnvActorCompCage`). This will be your environmental object (e.g., `BP_Vase`, `BP_Switch`).
// 2. In the Blueprint's defaults, you must configure:
//    - **Static Mesh**: Assign a `UStaticMeshComponent` to `envActorBodyMesh` to give the actor its visual appearance.
//    - **Interaction Properties**: Set `isSingleUse`, `totalInteractDuration`, `totalCooldown`, and the hint text keys to define how the object behaves when interacted with.
//    - **Behavior Flags**: Set booleans like `isAbleToBeRandomlized` or `isAbleToCauseUrgentEvent` to enable specific behaviors.
// 3. The core interaction logic is implemented by overriding the `ApplyInteractEffect...` functions.
//    - For instant effects, override `ApplyInteractEffectDirect`.
//    - For effects that happen over a duration, override `ApplyInteractEffectDurationEffect`.
//
// Necessary things to define:
// - A `StaticMesh` must be assigned to the `envActorBodyMesh` component in the Blueprint.
// - Interaction parameters (`totalCooldown`, etc.) must be set to non-zero values for those features to work.
//
// How it interacts with other classes:
// - AActor: The base class.
// - IMPInteractable / IMPPlaySoundInterface: Implements these interfaces to be discoverable by the character's interaction trace and to play sounds.
// - AMPCharacter: Characters interact with it by calling `BeInteracted`. This class then controls the logic, timers, and cooldowns.
// - AMPAISystemManager: If `isAbleToCauseUrgentEvent` is true, this actor can get a reference to the AI System Manager and send it notifications, causing AI to come and investigate.
// - Replication: `isInteracting` and `isInCooldown` are replicated so all clients can correctly see the object's state (e.g., visually changing its material or disabling its interaction prompt).
// - Child Classes (`AMPEnvActorCompCage`, `...Fracture`, etc.): Inherit this base functionality and add more specialized logic (e.g., breaking, holding a cat).

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../MPInteractable.h"
#include "../MPPlaySoundInterface.h"
#include "TimerManager.h"

#include "MPEnvActorComp.generated.h"

enum class EEnvActor : uint8;
enum class EEnvActorType : uint8;

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class UAudioComponent;

class AMPAISystemManager;

UCLASS(BlueprintType, Blueprintable)
class AMPEnvActorComp : public AActor, public IMPInteractable, public IMPPlaySoundInterface
{
    GENERATED_BODY()

public:
    AMPEnvActorComp();

    // IMPPlaySoundInterface
    virtual void PlaySoundLocally(USoundCue* aSound) override;
    virtual void PlaySoundBroadcast(USoundCue* aSound) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
        void OnRep_Interacting();
    UFUNCTION()
        void OnRep_InCooldown();

protected:
    UFUNCTION(Server, Reliable)
        void PlaySoundServer(USoundCue* aSound);

    UFUNCTION(NetMulticast, Reliable)
        void PlaySoundMulticast(USoundCue* aSound);

    // common envActor properties
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    FText envActorName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    EEnvActor envActorTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    EEnvActorType envActorType;

    // components
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* envActorSceneRoot;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    UStaticMeshComponent* envActorBodyMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    UAudioComponent* envActorAudioComp;

    // randomness
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnvActor Random Properties")
    bool isAbleToBeRandomlized = false;
public:
    UFUNCTION(BlueprintCallable, Category = "EnvActor Random Method")
    virtual void BeRandomized();

    // envactor urgent
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Urgent Properties")
    bool isAbleToCauseUrgentEvent = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Urgent Properties")
    AMPAISystemManager* theAISystem = nullptr;

    // cat interaction
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Urgent Properties")
        bool isAbleToBeInteractedByCat = false;

    // interactable interface
public:
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FString interactHintTextKey;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FString uninteractableHintTextKey;
    
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual bool IsInteractable(AMPCharacter* targetActor) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual FText GetInteractHintText(AMPCharacter* targetActor) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual void BeInteracted(AMPCharacter* targetActor) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    bool isSingleUse = false;
    UPROPERTY(ReplicatedUsing = OnRep_Interacting, BlueprintReadWrite, Category = "Interface Properties")
    bool isInteracting = false;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
    AMPCharacter* interactedCharacter = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    float totalInteractDuration;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
    float curInteractCountDown;
    FTimerHandle interactTimerHandle;

    // cooldown
    UPROPERTY(ReplicatedUsing = OnRep_InCooldown, BlueprintReadWrite, Category = "Interface Properties")
    bool isInCooldown;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    float totalCooldown;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
    float curCooldownCountDown;
    FTimerHandle cooldownTimerHandle;

    // interact specific
protected:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual void ApplyInteractEffectDirect(AMPCharacter* targetActor);
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    void EndInteractEffectDirect(AMPCharacter* targetActor);

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    void StartInteractEffectDuration(AMPCharacter* targetActor);
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    void ApplyInteractEffectDuration();
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual void ApplyInteractEffectDurationEffect();
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    void ApplyInteractEffectDurationCountdown();

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    void ExpireInteractEffectDuration();

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    void StartCooldown();
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    void CooldownCountDown();
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    void EndCooldown();

    // setter && getter
public:
    UFUNCTION(BlueprintCallable, Category = "Setter && Getter")
        bool CheckIfIsRandomizable();

    UFUNCTION(BlueprintCallable, Category = "Setter && Getter")
        bool CheckCanCauseUrgentEvent();

    UFUNCTION(BlueprintCallable, Category = "Setter && Getter")
        void SetAISystem(AMPAISystemManager* aAIManager);

    UFUNCTION(BlueprintCallable, Category = "Setter && Getter")
        bool CheckIfIsInteractableByCat();

protected :
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
        AMPAISystemManager* theAIManager;
};