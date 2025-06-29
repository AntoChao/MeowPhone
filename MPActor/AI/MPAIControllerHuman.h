#pragma once

#include "CoreMinimal.h"
#include "MPAIController.h"
#include "MPAIControllerHuman.generated.h"

class AMPAISystemManager;
class AEnvActorComp;

UCLASS(BlueprintType, Blueprintable)
class AMPAIControllerHuman : public AMPAIController
{
    GENERATED_BODY()

    // common functions
public:
    AMPAIControllerHuman();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    // System
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller System Interaction")
        AMPAISystemManager* theAIManager = nullptr;

// specific cat ai
protected:
    UPROPERTY(BlueprintReadWrite, Category="Human AI")
        bool bIsOnGlobalTask = false;

    // setter and getter
public :
    UFUNCTION(BlueprintCallable, Category="Setter && Getter")
        void SetAISystem(AMPAISystemManager* aAIManager);
};