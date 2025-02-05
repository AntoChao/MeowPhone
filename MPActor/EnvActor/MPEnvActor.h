#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../MPInteractable.h"

#include "Components/TimelineComponent.h"

#include "MPEnvActor.generated.h"

enum class EEnvActor : uint8;
class UStaticMeshComponent;
class UBoxComponent;
class UAudioComponent;

UCLASS(BlueprintType, Blueprintable)
class AMPCharacter : public AActor, public IMPInteractable
{
    GENERATED_BODY()

public :
    AMPEnvActor();

// common envActor properties
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        FLocalizedText envActorName;

    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        EEnvActorType envActorTag;

// components
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        UStaticMeshComponent* envActorBodyMesh;

    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        UBoxComponent* envActorCollision;

    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        UAudioComponent* envActorAudioComp;

// interactable interface
public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual bool IsInteractable(AMPCharacter* player) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual FLocalizedText GetInteractHintText(AMPCharacter* player) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual void BeInteracted(AMPCharacter* player) override;

protected :
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
        bool isSingleUse = false;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
        bool isInteracting = false;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
        AMPCharacter* interactedCharacter - nullptr;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
        float totalInteractDuration;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
        float curInteractCountDown;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
        FTimeline interactTimerHandle;

    // cooldown
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
        bool isInCooldown;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
        float totalCooldown;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
        float curCooldownCountDown;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
        FTimeline cooldownTimerHandle;

// interact specific
protected :
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual void ApplyInteractEffectDirect(AActor* targetActor);    
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        void EndInteractEffectDirect(AActor* targetActor);    
    
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        void StartInteractEffectDuration(AActor* targetActor);    
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
}