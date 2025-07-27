#include "MPAIControllerCatPlayer.h"
#include "../../CommonEnum.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Character/MPCharacterCat.h"
#include "../Character/MPCharacterHuman.h"

AMPAIControllerCatPlayer::AMPAIControllerCatPlayer()
{
    // Could assign a default BT asset in blueprint
}

void AMPAIControllerCatPlayer::BeginPlay()
{
    Super::BeginPlay();
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsEnum(BB_StateKey, static_cast<uint8>(EAICatState::Idle));
    }
}

void AMPAIControllerCatPlayer::ChooseNextVoluntaryAction()
{
    float Total = ProbIdle + ProbWander + ProbInteractHuman + ProbInteractCat + ProbPushEnv;
    if (Total <= 0.f) return;
    float Rand = FMath::FRand() * Total;
    EAICatState Chosen = EAICatState::Idle;
    float Acc = ProbIdle;
    if (Rand < Acc) Chosen = EAICatState::Idle; else if ((Acc += ProbWander) && Rand < Acc) Chosen = EAICatState::Wander;
    else if ((Acc += ProbInteractHuman) && Rand < Acc) Chosen = EAICatState::InteractHuman;
    else if ((Acc += ProbInteractCat) && Rand < Acc) Chosen = EAICatState::InteractCat;
    else Chosen = EAICatState::PushEnvActor;
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsEnum(BB_StateKey, static_cast<uint8>(Chosen));
    }

    if (Chosen == EAICatState::Idle)
    {
        if (AMPCharacterCat* CatPawn = Cast<AMPCharacterCat>(GetPawn()))
        {
            CatPawn->BeginIdlePoseTimer();
        }
    }
}

void AMPAIControllerCatPlayer::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    // Decision making will be handled in Behavior Tree; minimal C++ tick needed.
}

void AMPAIControllerCatPlayer::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    for (AActor* Actor : UpdatedActors)
    {
        if (Cast<AMPCharacterHuman>(Actor))
        {
            if (BlackboardComp)
            {
                BlackboardComp->SetValueAsBool(BB_PerceivedHuman, true);
                BlackboardComp->SetValueAsObject(BB_TargetActor, Actor);
            }
            return;
        }
    }
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsBool(BB_PerceivedHuman, false);
        BlackboardComp->ClearValue(BB_TargetActor);
    }
} 