#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RLActor.generated.h"

struct FLocalizedText;

UINTERFACE(MinimalAPI, Meta = (CannotImplementInterfaceInBlueprint))
class MEOWPHONE_API UMPInteractable : public UInterface
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
        virtual FLocalizedText GetInteractHintText() = 0;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual void BeInteracted(AMPCharacter* player) = 0;
};