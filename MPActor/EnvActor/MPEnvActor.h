#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../MPInteractable.h"

#include "MPEnvActor.generated.h"

struct FLocalizedText;
class AMPCharacter;

UCLASS(BlueprintType, Blueprintable)
class AMPCharacter : public AActor, public IMPInteractable
{
    GENERATED_BODY()

public :
    AMPEnvActor();

// common env actor properties
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        FLocalizedText mapObjectName;

// interactable interface
public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual bool IsInteractable(AMPCharacter* player) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual FLocalizedText GetInteractHintText() override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual void BeInteracted(AMPCharacter* player) override;

}