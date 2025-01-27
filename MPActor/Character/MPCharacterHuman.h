#pragma once

#include "CoreMinimal.h"
#include "MPCharacter.h"

#include "MPCharacterHuman.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMPCharacterCat : public AMPCharacter
{
    GENERATED_BODY()

public :
    AMPCharacterHuman();


// interactable interface
public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual bool IsInteractable(AMPCharacter* player) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual FLocalizedText GetInteractHintText() override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual void BeInteracted(AMPCharacter* player) override;

// controller/ input reaction
protected:
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    bool isHoldingCat;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    AMPCharacterCat* catHolding;

public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual void Interact() override;
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    virtual void InteractHuman() override;
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    virtual void InteractCat() override;

}