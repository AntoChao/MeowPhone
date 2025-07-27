#pragma once

// [Meow-Phone Project]
//
// This is a concrete AI Controller for a non-player-controlled (bot) Cat. It inherits the
// base AI systems from `AMPAIController` and implements the specific decision-making logic
// that defines the cat's behavior, such as choosing whether to idle, wander, or interact.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint class inheriting from `AMPAIControllerCatBot` (e.g., `BP_AIC_CatBot`).
// 2. In this Blueprint's defaults, you MUST:
//    - Assign a `BehaviorTree` asset for the cat.
// 3. You CAN configure the behavioral probabilities:
//    - `Prob Idle`, `Prob Wander`, `Prob Interact Human`: Adjust these floats (they should sum to 1.0) to change the cat's personality. A higher `ProbInteractHuman` will make the cat more social, while a higher `ProbWander` will make it more exploratory.
// 4. This AI Controller is typically spawned by the `UFactoryAIController` and assigned to a `AMPCharacterCat` pawn by the `UManagerAIController` when a match with bots begins.
//
// Necessary things to define:
// - A `BehaviorTree` asset must be assigned in the Blueprint.
// - The Blackboard used by the Behavior Tree must have keys that match the ones defined here and in the base class (e.g., "State", "TargetActor").
//
// How it interacts with other classes:
// - AMPAIController: It inherits all the core AI functionality from this base class.
// - BTTask_ChooseCatVoluntaryState: This BT Task will likely call this controller's `ChooseNextVoluntaryAction` function. The function in this class implements the weighted random logic to pick an action and then sets the "State" key on the Blackboard, directing the Behavior Tree to the appropriate branch (e.g., the wandering sequence, the idle sequence).
// - UBlackboardComponent: It writes to the Blackboard to set the AI's current state and target actor, which in turn controls the flow of the Behavior Tree.

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