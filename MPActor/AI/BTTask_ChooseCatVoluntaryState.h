#pragma once

// [Meow-Phone Project]
//
// This is a custom Behavior Tree Task node. Its purpose is to make a decision about what
// a Cat AI should do next when it's in a "voluntary" state (i.e., not currently in danger or
// reacting to a direct threat). This could involve deciding to wander, rest, or perform some
// other idle animation.
//
// How to utilize in Blueprint:
// 1. This task node will appear in the Behavior Tree editor under the "Tasks" category.
// 2. It should be placed in the Behavior Tree graph where you want the AI to make a decision about its idle behavior. For example, it could be at the end of a sequence where the AI has finished its primary objective.
// 3. This node does not have any public properties to configure in the editor; its logic is self-contained.
// 4. When this task executes, it will run its internal logic (in the C++ `ExecuteTask` function) and likely set a Blackboard key (e.g., an enum `AIState`) to a new value like "Wandering" or "Resting".
// 5. Subsequent nodes in the Behavior Tree will then use a decorator to check the value of that Blackboard key to decide which branch of the tree to execute next.
//
// Necessary things to define:
// - A Behavior Tree asset must be created for the Cat AI.
// - A Blackboard asset must be created and linked to the Behavior Tree. This Blackboard needs to have a key (e.g., an Enum `ECatState`) that this task can set.
//
// How it interacts with other classes:
// - UBTTaskNode: The base class for all Behavior Tree task nodes.
// - UBehaviorTreeComponent: This task gets access to the owning Behavior Tree Component, which allows it to get references to the AI Controller and the Blackboard.
// - UBlackboardComponent: It writes a value to a key on the Blackboard to communicate its decision to the rest of the Behavior Tree.
// - AMPAIControllerCatBot: This task will get the AI Controller to access the controlled pawn and its current state.

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChooseCatVoluntaryState.generated.h"

UCLASS()
class UBTTask_ChooseCatVoluntaryState : public UBTTaskNode
{
    GENERATED_BODY()
public:
    UBTTask_ChooseCatVoluntaryState();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
}; 