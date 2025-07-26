#include "BTTask_ChooseCatVoluntaryState.h"
#include "MPAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ChooseCatVoluntaryState::UBTTask_ChooseCatVoluntaryState()
{
    NodeName = TEXT("Choose Voluntary Action (Cat)");
}

EBTNodeResult::Type UBTTask_ChooseCatVoluntaryState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AMPAIController* AI = Cast<AMPAIController>(OwnerComp.GetAIOwner());
    if (!AI) return EBTNodeResult::Failed;
    AI->ChooseNextVoluntaryAction();
    return EBTNodeResult::Succeeded;
} 