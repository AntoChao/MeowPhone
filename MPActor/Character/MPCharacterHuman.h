#pragma once

#include "CoreMinimal.h"
#include "MPCharacter.h"

#include "MPCharacterHuman.generated.h"

enum class EHumanAction : uint8;
class AMPCharacterCat;

UCLASS(BlueprintType, Blueprintable)
class AMPCharacterHuman : public AMPCharacter
{
    GENERATED_BODY()

public:
    AMPCharacterHuman();


    // interactable interface
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FText catInteractHintText;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FText uninteractableCatHintText;

public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual bool IsInteractable(AMPCharacter* player) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual FText GetInteractHintText(AMPCharacter* player) override;

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
    virtual bool CheckIfIsAbleToLook() override;
    virtual bool CheckIfIsAbleToMove() override;
    virtual bool CheckIfIsAbleToInteract() override;

public :
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StartHoldingCat(AMPCharacterCat* catToHold);
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StopHoldingCat();

    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StartToBeRubbed(AMPCharacterCat* rubbedCat);
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StopToBeRubbed();

public:
    virtual void Interact() override;
};