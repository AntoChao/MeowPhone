#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../../CommonEnum.h"
#include "Perception/AIPerceptionTypes.h"
#include "MPAIController.generated.h"

class UBehaviorTree;
class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

UCLASS(Abstract, Blueprintable)
class AMPAIController : public AAIController
{
    GENERATED_BODY()

public:
    AMPAIController();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    bool bStunned = false;
    FTimerHandle StunTimerHandle;

    // Called to apply stun to this AI
public:
    UFUNCTION(BlueprintCallable, Category="AI|Status")
    void ApplyStun(float DurationSeconds);

public:
    virtual void ClearStun();

    // Movement / action helpers
    void JumpTowards(const FVector& TargetLoc);
    // Attempt forced double jump to reach high location – blueprint can call
    UFUNCTION(BlueprintCallable, Category="AI|Movement")
    virtual bool CanReachWithDoubleJump(const FVector& TargetLoc) const { return false; }

    // Decide if falling from current loc to target loc is safe
    UFUNCTION(BlueprintCallable, Category="AI|Movement")
    virtual bool IsSafeFallingDestination(const FVector& TargetLoc) const { return false; }
    
    void StartInteractWithActor(AActor* Target);

    void MoveToRandomPoint(float Radius);
protected:
    // Perception
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
    UAIPerceptionComponent* PerceptionComp;

    UPROPERTY()
    UAISenseConfig_Sight* SightConfig;
    UPROPERTY()
    UAISenseConfig_Hearing* HearingConfig;

    UFUNCTION()
    virtual void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

    virtual void SetupPerceptionSystem();

    // Behaviour Tree / Blackboard
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
    UBehaviorTree* BehaviorTreeAsset;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
    UBehaviorTreeComponent* BehaviorComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
    UBlackboardComponent* BlackboardComp;

    virtual void RunBehaviorTreeAsset();

    // Blackboard key names (generic)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
    FName BB_StateKey = TEXT("State");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
    FName BB_StunnedKey = TEXT("Stunned");

    // Helper functions
public:
    UFUNCTION(BlueprintCallable, Category="AI")
    UBlackboardComponent* GetBB() const { return BlackboardComp; }

    // Decide next voluntary action – overridden by child classes & callable from BT tasks
    UFUNCTION(BlueprintCallable, Category="AI|Decision")
    virtual void ChooseNextVoluntaryAction();

    // Per-actor perception stimulus callback (sight, hearing, etc.)
    UFUNCTION()
    virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
}; 