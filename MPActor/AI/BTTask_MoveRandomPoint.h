#pragma once

// [Meow-Phone Project]
//
// This is a Behavior Tree task that finds a random point within a specified radius
// of the AI's current location and then commands the AI to move to that point. This is
// a core task for creating simple wandering or patrolling behavior.
//
// How to utilize in Blueprint:
// 1. Place this task node in a Behavior Tree sequence that handles wandering behavior.
// 2. In the Details panel for this node in the Behavior Tree editor, you can adjust the `Radius` property to control how far the AI will wander from its current spot.
// 3. This task implicitly uses Unreal's navigation system. When it executes, it finds a random point and then uses the built-in "Move To" logic of the AI controller. The task will succeed if a path is found and the AI starts moving, and fail otherwise.
//
// Necessary things to define:
// - A NavMeshBoundsVolume must be present in the level, covering the areas where the AI is allowed to walk. This task relies on the NavMesh to find a reachable random point.
//
// How it interacts with other classes:
// - UBTTaskNode: The base class for the task.
// - AAIController: The `ExecuteTask` function uses the AI Controller to initiate the move request.
// - UNavigationSystemV1: It uses the navigation system to find a random reachable point within the specified radius on the NavMesh.
// - UBlackboardComponent (Potentially): While not directly visible in the header, the C++ implementation might set a "MoveToLocation" vector key on the blackboard and then rely on the engine's built-in "Move To" task to perform the actual movement, or it might call the `MoveToLocation` function on the AI Controller directly.

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveRandomPoint.generated.h"

UCLASS()
class UBTTask_MoveRandomPoint : public UBTTaskNode
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category="Task")
    float Radius = 600.f;

    UBTTask_MoveRandomPoint();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
}; 