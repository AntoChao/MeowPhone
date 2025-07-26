#pragma once

#include "CoreMinimal.h"
#include "MPAIController.h"
#include "MPAIControllerCatPlayer.generated.h"

UCLASS(Blueprintable)
class AMPAIControllerCatPlayer : public AMPAIController
{
    GENERATED_BODY()
public:
    AMPAIControllerCatPlayer();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;

    // Voluntary action probabilities (0-1 range, will be normalized)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Decision")
    float ProbIdle = 0.2f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Decision")
    float ProbWander = 0.4f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Decision")
    float ProbInteractHuman = 0.2f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Decision")
    float ProbInteractCat = 0.1f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Decision")
    float ProbPushEnv = 0.1f;

    virtual void ChooseNextVoluntaryAction() override;

    // Blackboard keys specific to Cat Player
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
    FName BB_PerceivedHuman = TEXT("PerceivedHuman");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
    FName BB_TargetActor = TEXT("TargetActor");
}; 