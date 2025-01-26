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
        EHumanAction curAction;

    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
        bool isHoldingCat;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
        AMPCharacterCat* catHolding;
    
public :
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual void Interact() override;
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual void InteractHuman() override;
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual void InteractCat() override;

}
