#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../MPInteractable.h"

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
class AMPEnvActorComp : public AActor, public IMPInteractable
{
    GENERATED_BODY()

public:
    AMPEnvActorComp();

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
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual bool IsInteractable(AMPCharacter* targetActor) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual FText GetInteractHintText(AMPCharacter* targetActor) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual void BeInteracted(AMPCharacter* targetActor) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    bool isSingleUse = false;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
    bool isInteracting = false;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
    AMPCharacter* interactedCharacter = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    float totalInteractDuration;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
    float curInteractCountDown;
    FTimerHandle interactTimerHandle;

    // cooldown
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
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