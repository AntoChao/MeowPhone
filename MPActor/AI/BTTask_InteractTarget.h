#pragma once

// [Meow-Phone Project]
//
// This is a Behavior Tree task that commands the AI to perform an "interact" action
// on a target actor. The target is read from a specified Blackboard key.
//
// How to utilize in Blueprint:
// 1. Place this task node in your Behavior Tree, typically after a "Move To" task has successfully brought the AI to its target.
// 2. In the Details panel for this task node in the Behavior Tree editor, you can change the `BB Target Key` property if your target actor is stored in a Blackboard key other than the default "TargetActor".
// 3. This task assumes that the controlled pawn (e.g., `AMPCharacterCat`) has an `Interact` function that can be called. The `ExecuteTask` implementation will get the pawn and call this function.
//
// Necessary things to define:
// - The Blackboard associated with the Behavior Tree must have an Object-based key (e.g., named "TargetActor") that will hold a reference to the actor the AI should interact with.
// - Other parts of the Behavior Tree (e.g., a service or another task) are responsible for finding a target and setting this Blackboard key *before* this task is executed.
//
// How it interacts with other classes:
// - UBTTaskNode: The base class for the task.
// - UBlackboardComponent: It reads the `BB_TargetKey` from the Blackboard to determine which actor to interact with.
// - AIController / Pawn: It gets the AI's controlled pawn.
// - MPInteractable or a similar interface (Inferred): The task's C++ code likely gets the pawn and calls a generic `Interact()` function on it. This function on the pawn would then perform the specific interaction logic, possibly by calling a function on the target actor if it implements an "interactable" interface.

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_InteractTarget.generated.h"

UCLASS()
class UBTTask_InteractTarget : public UBTTaskNode
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category="Task")
    FName BB_TargetKey = TEXT("TargetActor");

    UBTTask_InteractTarget();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
}; 