#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../MPInteractable.h"
#include "TimeManager.h"
#include "MPItem.generated.h"

struct FLocalizedText;
class AMPCharacter;
enum EItem;

UCLASS(BlueprintType, Blueprintable)
class AMPItem : public AActor, public IMPInteractable
{
    GENERATED_BODY()

public :
    AMPItem();

// common item properties
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        FLocalizedText itemName;

    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        EItem itemTag;
    
// interactable interface
public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual bool IsInteractable(AMPCharacter* player) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual FLocalizedText GetInteractHintText() override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual void BeInteracted(AMPCharacter* player) override;

// initialize / interact
public :
    UFUNCTION(BlueprintCallable, Category = "Interact Method")
        void BeInitialized(AMPCharacter* player);

    UFUNCTION(BlueprintCallable, Category = "Interact Method")
        void BePickedUp(AMPCharacter* player);
    
    UFUNCTION(BlueprintCallable, Category = "Interact Method")
        void BeDroped(AMPCharacter* player);
    
// usage
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
        bool isDirectUse;
    
    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
        bool isUsable;
    
    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
        bool isBeingUse;
    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
        float usageDuration;
    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
        FTimeline usageDurationTimeline;
    
    // cooldown
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
        bool isInCooldown;
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
        float totalCooldown;
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
        float curCooldownCOuntDown;
    UPROPERTY(BlueprintReadWrite, Category = "Cooldown Properties")
        FTimeline usageCooldownTimeline;

public :
    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void BeUsed(AMPCharacter* player, AActor* targetActor);
    
    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void ApplyUsageEffect(AMPCharacter* player, AActor* targetActor);
    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void ExpireUsageEffect(AMPCharacter* player, AActor* targetActor);

    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void StartCooldown(AMPCharacter* player, AActor* targetActor);
    UFUNCTION(BlueprintCallable, Category = "Usage Method")
        void EndCooldown(AMPCharacter* player, AActor* targetActor);

}