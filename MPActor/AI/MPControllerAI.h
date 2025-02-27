#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MPControllerAI.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMPControllerAI : public AAIController
{
    GENERATED_BODY()

public :
    AMPControllerAI();

// common ai controller properties
protected :
    UFUNCTION(BlueprintCallable, Category = "ai controller Method")
        virtual void BeginPlay() override;
};