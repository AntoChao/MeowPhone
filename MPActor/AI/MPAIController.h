#pragma once

// [Meow-Phone Project]
//
// This is the abstract base class for all AI Controllers in the game. It provides a
// foundational set of AI systems, including a Behavior Tree, a Blackboard, and a Perception
// Component (for sight and hearing). It also includes base logic for handling status effects
// like being stunned.
//
// How to utilize in Blueprint:
// 1. You should not create a Blueprint directly from this class because it is `Abstract`. Instead, create Blueprints from its child classes (e.g., `AMPAIControllerCatBot`).
// 2. In a child Blueprint, the most important property to set is `Behavior Tree Asset`. Assign the specific Behavior Tree that this AI should run.
// 3. The perception system (sight and hearing) is set up in C++ but can be configured in the child Blueprint. You can tweak properties like sight radius and hearing range on the `PerceptionComp`.
// 4. This class exposes helper functions like `ApplyStun` which can be called from other actors (like an ability) to affect the AI.
// 5. The Blackboard key names (`BB_StateKey`, `BB_StunnedKey`) can be changed in the child Blueprint's defaults if your Blackboard uses different key names.
//
// Necessary things to define:
// - In any child Blueprint, you MUST assign a `UBehaviorTree` asset to the `BehaviorTreeAsset` property. Without it, the AI will do nothing.
//
// How it interacts with other classes:
// - AAIController: The standard Unreal AI Controller it inherits from.
// - UBehaviorTreeComponent / UBlackboardComponent: It creates and manages these components, which are essential for running the AI logic defined in the Behavior Tree asset.
// - UAIPerceptionComponent: It sets up and manages the AI's ability to see and hear the world. The `OnPerceptionUpdated` and `OnTargetPerceptionUpdated` functions are the entry points for reacting to perceived stimuli.
// - Child AI Controllers (e.g., `AMPAIControllerCatBot`): Concrete AI controllers inherit from this class to gain all the base systems and then implement specialized logic.
// - Behavior Tree Tasks/Services/Decorators: These assets read from and write to the Blackboard owned by this controller to make decisions and control the AI's flow of logic. For example, a task might call `ChooseNextVoluntaryAction`.

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