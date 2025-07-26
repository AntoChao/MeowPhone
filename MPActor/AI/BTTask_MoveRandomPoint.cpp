#include "BTTask_MoveRandomPoint.h"
#include "MPAIController.h"

UBTTask_MoveRandomPoint::UBTTask_MoveRandomPoint()
{
    NodeName = TEXT("Move To Random Point");
}

EBTNodeResult::Type UBTTask_MoveRandomPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AMPAIController* Ctrl = Cast<AMPAIController>(OwnerComp.GetAIOwner());
    if (!Ctrl) return EBTNodeResult::Failed;
    Ctrl->MoveToRandomPoint(Radius);
    return EBTNodeResult::Succeeded;
} 