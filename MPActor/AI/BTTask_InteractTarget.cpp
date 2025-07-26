#include "BTTask_InteractTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MPAIController.h"

UBTTask_InteractTarget::UBTTask_InteractTarget()
{
    NodeName = TEXT("Interact With Target");
}

EBTNodeResult::Type UBTTask_InteractTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AMPAIController* Ctrl = Cast<AMPAIController>(OwnerComp.GetAIOwner());
    if (!Ctrl) return EBTNodeResult::Failed;
    UObject* Obj = OwnerComp.GetBlackboardComponent()->GetValueAsObject(BB_TargetKey);
    AActor* Target = Cast<AActor>(Obj);
    if (!Target) return EBTNodeResult::Failed;
    Ctrl->StartInteractWithActor(Target);
    return EBTNodeResult::Succeeded;
} 