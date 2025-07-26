#pragma once

#include "CoreMinimal.h"
#include "MPAIController.h"
#include "MPAIControllerCatBot.generated.h"

UCLASS(Blueprintable)
class AMPAIControllerCatBot : public AMPAIController
{
    GENERATED_BODY()
public:
    AMPAIControllerCatBot();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Decision")
    float ProbIdle = 0.3f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Decision")
    float ProbWander = 0.5f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Decision")
    float ProbInteractHuman = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
    FName BB_TargetActor = TEXT("TargetActor");

    // Decision making
    virtual void ChooseNextVoluntaryAction() override;
}; 