#include "MPAIControllerCatBot.h"
#include "CommonEnum.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MPActor/Character/MPCharacterHuman.h"
#include "MPActor/Character/MPCharacterCat.h"

AMPAIControllerCatBot::AMPAIControllerCatBot()
{
}

void AMPAIControllerCatBot::BeginPlay()
{
    Super::BeginPlay();
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsEnum(BB_StateKey, static_cast<uint8>(EAICatState::Idle));
    }
}

void AMPAIControllerCatBot::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AMPAIControllerCatBot::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    // Same as CatPlayer but simpler
    for (AActor* Actor : UpdatedActors)
    {
        if (Cast<AMPCharacterHuman>(Actor))
        {
            if (BlackboardComp)
            {
                BlackboardComp->SetValueAsObject(BB_TargetActor, Actor);
            }
            return;
        }
    }
    if (BlackboardComp)
    {
        BlackboardComp->ClearValue(BB_TargetActor);
    }
}

void AMPAIControllerCatBot::ChooseNextVoluntaryAction()
{
    float Total = ProbIdle + ProbWander + ProbInteractHuman;
    if (Total <= 0.f || !BlackboardComp) return;

    float Rand = FMath::FRand() * Total;
    EAICatState Chosen = EAICatState::Idle;
    float Acc = ProbIdle;
    if (Rand < Acc) {
        Chosen = EAICatState::Idle;
    } else if ((Acc += ProbWander) && Rand < Acc) {
        Chosen = EAICatState::Wander;
    } else {
        Chosen = EAICatState::InteractHuman;
    }

    BlackboardComp->SetValueAsEnum(BB_StateKey, static_cast<uint8>(Chosen));

    // Trigger idle pose timer if needed
    if (Chosen == EAICatState::Idle) {
        if (AMPCharacterCat* CatPawn = Cast<AMPCharacterCat>(GetPawn())) {
            CatPawn->BeginIdlePoseTimer();
        }
    }
} 