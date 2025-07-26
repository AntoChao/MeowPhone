#pragma once

#include "CoreMinimal.h"
#include "MPAIController.h"
#include "MPAIControllerHumanPlayer.generated.h"

UCLASS(Blueprintable)
class AMPAIControllerHumanPlayer : public AMPAIController
{
    GENERATED_BODY()
public:
    AMPAIControllerHumanPlayer();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;
    virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Decision")
    float ProbIdle = 0.2f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Decision")
    float ProbWander = 0.4f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Decision")
    float ProbSearch = 0.2f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Decision")
    float ProbInteractContext = 0.2f;

    // Blackboard keys for Human AI
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
    FName BB_PerceivedCat = TEXT("PerceivedCat");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
    FName BB_NoiseLocation = TEXT("NoiseLocation");

    // Decision making
    virtual void ChooseNextVoluntaryAction() override;
}; 