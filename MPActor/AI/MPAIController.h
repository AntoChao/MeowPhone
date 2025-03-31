#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MPAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UBehaviorTree;

UCLASS(BlueprintType, Blueprintable)
class AMPAIController : public AAIController
{
    GENERATED_BODY()

    // common functions
public:
    AMPAIController();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

// behaviour tree
protected:
    virtual void SetupPerceptionSystem();
    virtual void LoadBehaviorTree();

    UPROPERTY(EditAnywhere, Category="AI")
    UBehaviorTree* BehaviorTreeAsset;

    UPROPERTY(BlueprintReadOnly, Category="AI")
    UBlackboardComponent* BlackboardComp;

// perception
protected :
    UPROPERTY(BlueprintReadOnly, Category="AI")
    UAIPerceptionComponent* AIPerception;
};