#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MPAIController.generated.h"

class UBehaviorTree;
class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

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

    UPROPERTY(BlueprintReadOnly, Category = "AI")
    UBehaviorTreeComponent* BehaviorComp;
    
// tree assets
protected:
    UPROPERTY(BlueprintReadOnly, Category = "AI")
    bool isDoingGlobalTask = false;

// perception
protected :
    UPROPERTY(BlueprintReadOnly, Category="AI")
    UAIPerceptionComponent* AIPerceptionComp;

    UFUNCTION(BlueprintCallable, Category = "AI")
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

public:
    UFUNCTION(BlueprintCallable, Category = "AI")
    bool IsBusyWithGlobalTask();

    UFUNCTION(BlueprintCallable, Category = "AI")
    void AssignGlobalTask(AMPEnvActorComp* envActorAssigend);
};