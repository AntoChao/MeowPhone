#pragma once

#include "CoreMinimal.h"
#include "MPCharacter.h"

#include "MPCharacterHuman.generated.h"

enum class EHumanAction : uint8;
class AMPCharacterCat;

UCLASS(BlueprintType, Blueprintable)
class AMPCharacterCat : public AMPCharacter
{
    GENERATED_BODY()

public :
    AMPCharacterHuman();


// interactable interface
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
    FLocalizedText catInteractHintText;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
    FLocalizedText uninteractableCatHintText;

public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual bool IsInteractable(AMPCharacter* player) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual FLocalizedText GetInteractHintText(AMPCharacter* player) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual void BeInteracted(AMPCharacter* player) override;

// controller/ input reaction
protected:
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
        EHumanAction curHumanAction;

    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    bool isHoldingCat = false;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    AMPCharacterCat* catHolding;

    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    bool isRubbedByCat = false;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    AMPCharacterCat* catRubbing;

protected:
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    virtual bool CheckIfIsAbleToLook() override;
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    virtual bool CheckIfIsAbleToMove() override;
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    virtual bool CheckIfIsAbleToInteract() override;

    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StartHoldingCat(AMPCharacterCat* catToHold);
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StopHoldingCat();

    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StartToBeRubbed(AMPCharacterCat* rubbedCat);
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StopToBeRubbed();

public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual void Interact() override;
}