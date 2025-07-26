#pragma once
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