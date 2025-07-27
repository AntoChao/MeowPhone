#pragma once

// [Meow-Phone Project]
//
// This is a specialized AI Controller intended to possess a PLAYER-controlled Human Character.
// Its purpose is to provide autonomous behaviors when the human player is idle (AFK),
// allowing the character to continue participating in the game in a limited way, such as
// wandering, searching for cats, or reacting to global tasks.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint inheriting from this class (e.g., `BP_AIC_HumanPlayer`).
// 2. In this Blueprint, you MUST assign a `BehaviorTree` asset that defines the human's autonomous behaviors.
// 3. The behavioral probabilities (`ProbIdle`, `ProbWander`, `ProbSearch`) can be adjusted in the Blueprint to define the character's default actions when idle.
// 4. This controller would be swapped in by the `AMPControllerPlayer` when it detects the human player is inactive. When the player provides input again, the `AMPControllerPlayer` would re-possess the pawn, disabling this AI.
//
// Necessary things to define:
// - A `BehaviorTree` asset is mandatory and must be assigned in the Blueprint's defaults.
// - The Blackboard for the BT must have keys matching those defined here (e.g., "PerceivedCat", "NoiseLocation").
// - A system in `AMPControllerPlayer` or the Game Mode must exist to manage when this AI takes and relinquishes control.
//
// How it interacts with other classes:
// - AMPAIController: Inherits the base AI systems (perception, BT, etc.).
// - AMPControllerPlayer: Works in concert with the player's real controller, which manages the possession swapping.
// - AMPCharacterHuman: It possesses and controls a player's human pawn.
// - Behavior Tree: The assigned BT uses this controller to make decisions. The `ChooseNextVoluntaryAction` function uses probabilities to select an idle-state behavior, while `OnTargetPerceptionUpdated` handles reactions to seeing a cat or hearing a noise.
// - AMPAISystemManager: It can receive and act on global tasks assigned by this high-level manager.

#include "CoreMinimal.h"
#include "MPAIController.h"
#include "MPAIControllerHumanPlayer.generated.h"

class AMPAISystemManager;
class AMPEnvActorComp;

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FName BB_GlobalTaskActor = TEXT("GlobalTaskActor");

    // Decision making
    virtual void ChooseNextVoluntaryAction() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|System")
	AMPAISystemManager* AISystem = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool isBeingAssined;

public:
    void SetAISystem(AMPAISystemManager* theManager);

    bool IsBusyWithGlobalTask();

    void AssignGlobalTask(AMPEnvActorComp* urgentActor);
}; 