#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MPAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UBehaviorTree;
class AMPCharacter;

UCLASS(BlueprintType, Blueprintable)
class AMPAIController : public AAIController
{
    GENERATED_BODY()

    // common functions
public:
    AMPAIController();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    // components
protected:
    UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category="AI Controller Component")
    UBehaviorTreeComponent* BehaviorComp;

    UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category="AI Controller Component")
    UBlackboardComponent* BlackboardComp;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller Component")
    UBehaviorTree* BehaviorTreeAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller Component")
    UAIPerceptionComponent* AIPerceptionComp;

    // components functions 
protected:
    UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

    /* AI internal state variables */
protected :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller Internal Variable")
    AMPCharacter* OwnedCharacter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller Internal Variable")
    float CurrentSuspicionLevel = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller Internal Variable")
    float TimeSinceLastStimulus = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller Internal Variable")
    bool bIsReactingToGlobalEvent = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller Internal Variable")
    bool bHasLockedDownRoute = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller Internal Variable")
    FVector LastHeardSoundLocation;

    /* Blackboard Keys */
    // These keys must match what your Behavior Tree asset is expecting.
protected :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller Blackboard")
    FName TargetActorKey = TEXT("TargetActor");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller Blackboard")
    FName SuspicionLevelKey = TEXT("SuspicionLevel");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller Blackboard")
    FName PatrolTargetKey = TEXT("PatrolTarget");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller Blackboard")
    FName IsChasingKey = TEXT("IsChasing");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller Blackboard")
    FName GlobalTaskTargetKey = TEXT("GlobalTaskTarget");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI Controller Blackboard")
    FName bReactingToGlobalTaskKey = TEXT("bReactingToGlobalTask");

// Utility functions to update Blackboard keys
public :
    UFUNCTION(BlueprintCallable, Category="AI Controller Setter && Getter")
    void SetTargetActor(AActor* NewTarget);

    UFUNCTION(BlueprintCallable, Category="AI Controller Setter && Getter")
    void SetSuspicionLevel(float NewLevel);
};