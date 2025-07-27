#pragma once

// [Meow-Phone Project]
//
// This is the base class for all gameplay abilities. It is an Actor that can be spawned
// into the world to represent an ability being used. It includes built-in, replicated logic
// for usage duration, cooldowns, and distinguishing between instant and over-time effects.
//
// How to utilize in Blueprint:
// 1. Create a new Blueprint class that inherits from `AMPAbility`. This will be your specific ability (e.g., `BP_ScratchAbility`, `BP_HealAbility`).
// 2. In the Blueprint's Class Defaults, set the key properties:
//    - `Ability Tag`: A unique enum value to identify this ability.
//    - `Ability Type`: Set to `Direct` for instant effects or `Duration` for effects over time.
//    - `Total Usage Duration`: If it's a duration ability, set how long the effect lasts.
//    - `Total Cooldown`: Set the time before the ability can be used again.
// 3. The core logic of the ability is implemented by overriding specific functions in the Blueprint:
//    - For `Direct` abilities, override `ApplyUsageEffectDirect` to implement the ability's effect (e.g., deal damage, apply a stun).
//    - For `Duration` abilities, override `ApplyUsageEffectDurationEffect` to implement the logic that happens on each "tick" of the ability's duration.
// 4. This ability actor is spawned by a Factory (`UFactoryAbility`), usually when a character activates it. The `BeInitialized` function is called immediately after spawning to link the ability to its owner.
// 5. The owning character calls `BeUsed` on the ability instance to trigger it.
//
// Necessary things to define:
// - All the public properties in the "Usage Properties" and "Cooldown Properties" sections must be configured in the derived Blueprint to define the ability's behavior.
// - The appropriate `Apply...` function must be overridden in the Blueprint to give the ability its unique gameplay effect.
//
// How it interacts with other classes:
// - AActor: It is an actor that exists in the world.
// - AMPCharacterCat: The `abilityOwner` is typically a cat character. The ability holds a reference to its owner.
// - UFactoryAbility: This factory is responsible for spawning instances of `AMPAbility` blueprints.
// - Replication: Key boolean flags (`isBeingUse`, `isInCooldown`) are replicated so that clients can visually represent the ability's state (e.g., greying out an icon on the HUD). `OnRep_` functions are used to trigger these visual updates.
// - FTimerManager: It relies heavily on timers to manage the duration and cooldown countdowns.

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "TimerManager.h"

#include "MPAbility.generated.h"

enum class EAbility : uint8;
class AMPCharacterCat;
class AActor;

UCLASS(BlueprintType, Blueprintable)
class AMPAbility : public AActor
{
    GENERATED_BODY()

public:
    AMPAbility();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

    // replication flags
    UPROPERTY(ReplicatedUsing = OnRep_BeingUse)
    bool isBeingUse = false;
    UPROPERTY(ReplicatedUsing = OnRep_InCooldown)
    bool isInCooldown = false;

    UFUNCTION()
        void OnRep_BeingUse();
    UFUNCTION()
        void OnRep_InCooldown();

    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
    AActor* targetActorSaved = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usage Properties")
    float totalUsageDuration;
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
    float curUsageCountDown;
    
    FTimerHandle usageTimerHandle;

    // cooldown
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
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