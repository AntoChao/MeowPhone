#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../MPInteractable.h"

#include "TimerManager.h"

#include "MPEnvActor.generated.h"

enum class EEnvActor : uint8;
enum class EEnvActorType : uint8;

class UStaticMeshComponent;
class UBoxComponent;
class UAudioComponent;

UCLASS(BlueprintType, Blueprintable)
class AMPEnvActor : public AActor, public IMPInteractable
{
    GENERATED_BODY()

public:
    AMPEnvActor();

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    UStaticMeshComponent* envActorBodyMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    UBoxComponent* envActorCollision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    UAudioComponent* envActorAudioComp;

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
};