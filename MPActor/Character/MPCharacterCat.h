#pragma once

#include "CoreMinimal.h"
#include "MPCharacter.h"

#include "MPCharacterCat.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMPCharacterCat : public AMPCharacter
{
    GENERATED_BODY()

public :
    AMPCharacterCat();

// ability
protected:
    UPROPERTY(BlueprintReadWrite, Category = "Ability Properties")
    TArray<EAbility> initABilities;

    UPROPERTY(BlueprintReadWrite, Category = "Ability Properties")
    TArray<UAbility> allABilities;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    void InitializeAllAbility();

// interactable interface
public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual bool IsInteractable(AMPCharacter* player) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual FLocalizedText GetInteractHintText() override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual void BeInteracted(AMPCharacter* player) override;

// controller direct relation
protected:
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    bool isBeingHolded;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    AMPCharacterHuman* humanHolding;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    float holdTotalTime;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    float curHoldTime;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    float curHoldPercentage;

public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual void Interact() override;
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    virtual void InteractHuman() override;
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    virtual void InteractCat() override;
}
