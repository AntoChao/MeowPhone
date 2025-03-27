#include "AMPAIController.h"
#include "AICharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

AMPAIController::AMPAIController()
{
    BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

    AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
    if (AIPerceptionComp)
    {
        AIPerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AMPAIController::OnPerceptionUpdated);
    }
}

void AMPAIController::BeginPlay()
{
    Super::BeginPlay();

    OwnedCharacter = Cast<AMPCharacterHuman>(GetPawn());

    // Initialize Blackboard with the Behavior Tree's Blackboard Asset if available
    if (BehaviorTreeAsset && BehaviorTreeAsset->BlackboardAsset)
    {
        BlackboardComp->InitializeBlackboard(*BehaviorTreeAsset->BlackboardAsset);

        // Optionally, set initial values for your keys
        BlackboardComp->SetValueAsFloat(SuspicionLevelKey, CurrentSuspicionLevel);
        BlackboardComp->SetValueAsBool(IsChasingKey, false);
    }

    // Start running the Behavior Tree
    if (BehaviorTreeAsset)
    {
        BehaviorComp->StartTree(*BehaviorTreeAsset);
    }
}

void AMPAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    TimeSinceLastStimulus += DeltaSeconds;
}

void AMPAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    for (AActor* Actor : UpdatedActors)
    {
        if (!Actor) continue;

        if (false)
        {
            SetTargetActor(Actor);
            BlackboardComp->SetValueAsBool(IsChasingKey, true);
            
            // Reset suspicion level and timer
            CurrentSuspicionLevel = 100.f;
            TimeSinceLastStimulus = 0.f;
            break;
        }
    }
}

void AMPAIController::SetTargetActor(AActor* NewTarget)
{
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsObject(TargetActorKey, NewTarget);
    }
}

void AMPAIController::SetSuspicionLevel(float NewLevel)
{
    CurrentSuspicionLevel = NewLevel;
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsFloat(SuspicionLevelKey, CurrentSuspicionLevel);
    }
}