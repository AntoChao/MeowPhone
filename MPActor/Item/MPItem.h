#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../MPInteractable.h"

#include "TimerManager.h"
#include "../MPPlaySoundInterface.h"
#include "MPItem.generated.h"

enum class EMPItem : uint8;
class UStaticMeshComponent;
class UBoxComponent;
class UAudioComponent;
enum class EItemType : uint8;

UCLASS(BlueprintType, Blueprintable)
class AMPItem : public AActor, public IMPInteractable, public IMPPlaySoundInterface
{
    GENERATED_BODY()

public :
    AMPItem();

// common item properties
protected :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
        FText itemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
        EMPItem itemTag;

// components
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        UStaticMeshComponent* itemBodyMesh;

    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        UBoxComponent* itemCollision;

    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        UAudioComponent* itemAudioComp;

// interactable interface
protected :
    UPROPERTY(ReplicatedUsing = OnRep_PickedUp, BlueprintReadWrite, Category = "Interact Properties")
        bool isPickedUp = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
        FString interactHintTextKey;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
        FString uninteractableHintTextKey;

public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual bool IsInteractable(AMPCharacter* player) override;

    // Change return type to FName for localization key
        virtual FText GetInteractHintText(AMPCharacter* player) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual void BeInteracted(AMPCharacter* player) override;

// initialize / interact
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
        AMPCharacter* itemOwner = nullptr;

public :
    UFUNCTION(BlueprintCallable, Category = "Interact Method")
        void BeInitialized(AMPCharacter* player);

    UFUNCTION(BlueprintCallable, Category = "Interact Method")
        void BePickedUp(AMPCharacter* player); // add to inventory, no collision + invinsible
    
    UFUNCTION(BlueprintCallable, Category = "Interact Method")
        void BeDroped(AMPCharacter* player);
    
// usage
protected :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usage Properties")
        EItemType itemType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usage Properties")
        bool isSingleUse = false;
    UPROPERTY(ReplicatedUsing = OnRep_BeingUse, BlueprintReadWrite, Category = "Usage Properties")
        bool isBeingUse = false;
    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
        AActor* targetActorSaved = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usage Properties")
        float totalUsageDuration;
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
        float curUsageCountDown;
    FTimerHandle usageTimerHandle;

    // cooldown
    UPROPERTY(ReplicatedUsing = OnRep_InCooldown, BlueprintReadWrite, Category = "Cooldown Properties")
        bool isInCooldown;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooldown Properties")
        float totalCooldown;
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
        float curCooldownCountDown;
    FTimerHandle cooldownTimerHandle;

public :
    /* no item is direct use, player select an item and then use it or drop it */
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

public :
    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void GetEliminated();

    // IMPPlaySoundInterface
    virtual void PlaySoundLocally(USoundCue* aSound) override;
    virtual void PlaySoundBroadcast(USoundCue* aSound) override;

protected:
    UFUNCTION(Server, Reliable)
        void PlaySoundServer(USoundCue* aSound);

    UFUNCTION(NetMulticast, Reliable)
        void PlaySoundMulticast(USoundCue* aSound);

public :
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
        void OnRep_PickedUp();
    UFUNCTION()
        void OnRep_BeingUse();
    UFUNCTION()
        void OnRep_InCooldown();
};