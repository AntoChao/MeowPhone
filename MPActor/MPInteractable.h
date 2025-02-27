#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MPInteractable.generated.h"

struct FLocalizedText;
class AMPCharacter;

UINTERFACE(MinimalAPI, Meta = (CannotImplementInterfaceInBlueprint))
class UMPInteractable : public UInterface
{
    GENERATED_BODY()
};

class IMPInteractable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual bool IsInteractable(AMPCharacter* player) = 0;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual FText GetInteractHintText(AMPCharacter* player) = 0;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual void BeInteracted(AMPCharacter* player) = 0;
};