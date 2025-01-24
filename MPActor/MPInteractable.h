#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RLActor.generated.h"

UINTERFACE(MinimalAPI, Meta = (CannotImplementInterfaceInBlueprint))
class UMPInteractable : public UInterface
{
    GENERATED_BODY()
};

class IMPInteractable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual FString GetActorName(ELanguage curLanguage) = 0;

    UFUNCTION(BlueprintCallable)
    virtual FString GetDescription(ELanguage curLanguage) = 0;

    UFUNCTION(BlueprintCallable)
    virtual FString GetInteractionDescription(ELanguage curLanguage) = 0;

    UFUNCTION(BlueprintCallable)
    virtual bool IsAbleToBeInteracted(APlayerCharacter* Sender) = 0;

    UFUNCTION(BlueprintCallable)
    virtual void BeInteracted(APlayerCharacter* Sender) = 0;
};