#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "TimerManager.h"

#include "MPAbility.generated.h"

enum class EAbility : uint8;
class AMPCharacterCat;
class AActor;

UCLASS(BlueprintType, Blueprintable)
class UMPAbility : public UObject
{
    GENERATED_BODY()

public:
    UMPAbility();

    // common Ability properties
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    EAbility abilityTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    AMPCharacterCat* abilityOwner;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    UWorld* ownerWorld;

public:
    /* BeInitialized
    * this->SetOwner(player);
    */
    UFUNCTION(BlueprintCallable, Category = "Common Method")
    void BeInitialized(AMPCharacterCat* player);

    UFUNCTION(BlueprintCallable, Category = "Common Method")
    EAbility GetAbilityTag();

    // usage
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usage Properties")
    EAbilityType abilityType;

    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
    bool isBeingUse = false;
    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
    AActor* targetActorSaved = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usage Properties")
    float totalUsageDuration;
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
    float curUsageCountDown;
    
    FTimerHandle usageTimerHandle;

    // cooldown
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
    bool isInCooldown;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooldown Properties")
    float totalCooldown;
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
    float curCooldownCountDown;
    
    FTimerHandle cooldownTimerHandle;

public:
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

};