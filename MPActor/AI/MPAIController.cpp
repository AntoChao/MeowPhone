#include "MPAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

#include "../EnvActor/AMPEnvActorComp.h"

AMPAIController::AMPAIController()
{
    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
    AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
    BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviourTreeComp"));
}

void AMPAIController::BeginPlay()
{
    Super::BeginPlay();

    SetupPerceptionSystem();
    LoadBehaviorTree();
}

void AMPAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AMPAIController::SetupPerceptionSystem()
{
    if (AIPerceptionComp)
    {
        AIPerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AMPAIController::OnPerceptionUpdated);
    }
}
void AMPAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    return;
}

void AMPAIController::LoadBehaviorTree()
{
    if (BehaviorTreeAsset && BehaviorTreeAsset->BlackboardAsset)
    {
        BehaviorComp->StartTree(*BehaviorTreeAsset);
    }
}

bool AMPAIController::IsBusyWithGlobalTask()
{
    return isDoingGlobalTask;
}

void AMPAIController::AssignGlobalTask(AMPEnvActorComp* envActorAssigend)
{
    return;
}