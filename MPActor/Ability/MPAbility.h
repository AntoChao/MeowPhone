#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Object.h"

#include "Components/TimelineComponent.h"

#include "MPItem.generated.h"

enum class EAbility : uint8;
class AMPCharacterCat;
class AActor;

UCLASS(BlueprintType, Blueprintable)
class UMPAbility : public UObject
{
    GENERATED_BODY()

public :
    UMPAbility();

// common Ability properties
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        EAbility abilityTag;

public :
    /* BeInitialized
    * this->SetOwner(player);
    */
    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void BeInitialized(AMPCharacterCat* player);

// usage
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
        bool isBeingUsed;
    
    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
        bool isBeingUse = false;
    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
        AActor* targetActorSaved = nullptr;
    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
        float totalUsageDuration;
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
        float curUsageCountDown;
    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
        FTimeline usageTimerHandle;

    // cooldown
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
        bool isInCooldown;
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
        float totalCooldown;
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
        float curCooldownCountDown;
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
        FTimeline cooldownTimerHandle;

public :
    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void BeUsed(AActor* targetActor);
    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        bool IsAbleToBeUsed(AActor* targetActor);

    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        virtual void ApplyUsageEffectDirect(AActor* targetActor);
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        void EndUsageEffectDirect(AActor* targetActor);  

    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void StartUsageEffectDuration(AActor* targetActor);    
    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void ApplyUsageEffectDuration();
    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        virtual void ApplyUsageEffectDurationEffect();
    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void ApplyUsageEffectDurationCountdown();
    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void ExpireUsageEffectDuration();

    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void StartCooldown();
    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void CooldownCountDown();
    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void EndCooldown();

}