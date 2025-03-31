#pragma once

#include "CoreMinimal.h"
#include "MPAIController.h"
#include "MPAIControllerCat.generated.h"

class AMPAISystemManager;

UCLASS(BlueprintType, Blueprintable)
class AMPAIControllerCat : public AMPAIController
{
    GENERATED_BODY()

    // common functions
public:
    AMPAIControllerCat();

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

    UFUNCTION(BlueprintCallable)
        void AssignGlobalTask(AEnvActorComp* Target);

    // setter and getter
public :
    UFUNCTION(BlueprintCallable, Category="Setter && Getter")
        void SetAISystem(AMPAISystemManager* aAIManager);

    UFUNCTION(BlueprintCallable)
        bool IsBusyWithGlobalTask() const { return bIsOnGlobalTask; }
};