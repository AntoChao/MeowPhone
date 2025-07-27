#pragma once

// [Meow-Phone Project]
//
// This is a specialized AI Controller intended to possess a PLAYER-controlled Cat Character.
// Its purpose is likely to provide autonomous behaviors when the human player is idle or
// to assist the player with certain actions (e.g., auto-interacting with nearby objects).
// It essentially acts as an "autopilot" for the player's character.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint inheriting from this class (`BP_AIC_CatPlayer`).
// 2. In this Blueprint, you must assign a `BehaviorTree` asset. This BT will define the "autopilot" behaviors.
// 3. The probabilities (`ProbIdle`, `ProbWander`, etc.) can be tweaked in the Blueprint to define what the cat is most likely to do when left to its own devices.
// 4. This controller would be spawned and would possess the player's cat pawn under specific conditions, likely managed by the `AMPControllerPlayer` or the Game Mode. For example, if the player is AFK for a certain amount of time, the `AMPControllerPlayer` might unpossess the pawn and have this AI controller possess it instead.
//
// Necessary things to define:
// - A `BehaviorTree` asset must be assigned in the Blueprint's defaults.
// - A system must be in place (likely in the PlayerController or GameMode) to decide *when* this AI should take control of a player's pawn.
//
// How it interacts with other classes:
// - AMPAIController: Inherits the base AI systems.
// - AMPControllerPlayer: This class works in tandem with the actual player controller. The `AMPControllerPlayer` would be responsible for swapping control between the human player and this AI controller.
// - AMPCharacterCat: It possesses and controls a player's cat pawn.
// - Behavior Tree: The assigned BT uses the `ChooseNextVoluntaryAction` function to make decisions. This function's implementation, with its various probabilities, gives the player's idle cat a believable set of behaviors.
// - UBlackboardComponent: It writes to the Blackboard to direct the flow of its associated Behavior Tree.

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