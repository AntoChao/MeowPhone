#include "MPAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "NavigationSystem.h"
#include "../Character/MPCharacter.h"

AMPAIController::AMPAIController()
{
    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
    BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
    PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
}

void AMPAIController::BeginPlay()
{
    Super::BeginPlay();
    SetupPerceptionSystem();
    RunBehaviorTreeAsset();
}

void AMPAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AMPAIController::SetupPerceptionSystem()
{
    SightConfig = NewObject<UAISenseConfig_Sight>();
    HearingConfig = NewObject<UAISenseConfig_Hearing>();

    if (SightConfig)
    {
        SightConfig->SightRadius = 1500.f;
        SightConfig->LoseSightRadius = 1700.f;
        SightConfig->PeripheralVisionAngleDegrees = 60.f;
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
        PerceptionComp->ConfigureSense(*SightConfig);
    }
    if (HearingConfig)
    {
        HearingConfig->HearingRange = 1200.f;
        HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
        HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
        HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
        PerceptionComp->ConfigureSense(*HearingConfig);
    }
    PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AMPAIController::OnPerceptionUpdated);
    // Bind detailed stimulus callback so derived classes can override
    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AMPAIController::OnTargetPerceptionUpdated);
    PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AMPAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    // Base implementation does nothing.
}

void AMPAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    // Base implementation does nothing – derived classes can react to specific senses
}

void AMPAIController::ChooseNextVoluntaryAction()
{
    // Base class placeholder – specific AI controllers implement their own decision logic
}

void AMPAIController::RunBehaviorTreeAsset()
{
    if (BehaviorTreeAsset)
    {
        if (BehaviorTreeAsset->BlackboardAsset)
        {
            BlackboardComp->InitializeBlackboard(*BehaviorTreeAsset->BlackboardAsset);
        }
        RunBehaviorTree(BehaviorTreeAsset);
    }
}

void AMPAIController::ApplyStun(float DurationSeconds)
{
    if (bStunned) return;
    bStunned = true;
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsBool(BB_StunnedKey, true);
    }

    // Notify the possessed pawn so its animation & logic update too
    if (AMPCharacter* Char = Cast<AMPCharacter>(GetPawn()))
    {
        Char->BeStunned(static_cast<int32>(DurationSeconds));
    }

    GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, this, &AMPAIController::ClearStun, DurationSeconds, false);
    // Stop movement
    StopMovement();
}

void AMPAIController::ClearStun()
{
    bStunned = false;

    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsBool(BB_StunnedKey, false);
    }

    if (AMPCharacter* Char = Cast<AMPCharacter>(GetPawn()))
    {
        Char->StopStunned();
    }
}

void AMPAIController::MoveToRandomPoint(float Radius)
{
    UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld());
    APawn* MyPawn = GetPawn();
    if (!Nav || !MyPawn) return;
    FNavLocation OutLoc;
    if (Nav->GetRandomReachablePointInRadius(MyPawn->GetActorLocation(), Radius, OutLoc))
    {
        MoveToLocation(OutLoc.Location, 5.f);
        if (AMPCharacter* Char = Cast<AMPCharacter>(MyPawn))
        {
            Char->AI_Move(FVector2D(1.f,0.f));
        }
    }
}

void AMPAIController::JumpTowards(const FVector& TargetLoc)
{
    APawn* MyPawn = GetPawn();
    if (AMPCharacter* Char = Cast<AMPCharacter>(MyPawn))
    {
        Char->AI_JumpStart();
        MoveToLocation(TargetLoc, 5.f);
    }
}

void AMPAIController::StartInteractWithActor(AActor* Target)
{
    if (!Target) return;
    APawn* MyPawn = GetPawn();
    MoveToActor(Target, 80.f);
    if (AMPCharacter* Char = Cast<AMPCharacter>(MyPawn))
    {
        Char->AI_Interact();
    }
} 