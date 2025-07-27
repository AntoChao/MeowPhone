#include "MPAIControllerHumanPlayer.h"
#include "../../CommonEnum.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Character/MPCharacterCat.h"
#include "Perception/AISense_Hearing.h"
#include "../Character/MPCharacterHuman.h"


AMPAIControllerHumanPlayer::AMPAIControllerHumanPlayer()
{
}

void AMPAIControllerHumanPlayer::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AMPAIControllerHumanPlayer::BeginPlay()
{
    Super::BeginPlay();
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsEnum(BB_StateKey, static_cast<uint8>(EAIHumanState::Idle));
    }
}

void AMPAIControllerHumanPlayer::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    for (AActor* Actor : UpdatedActors)
    {
        if (Cast<AMPCharacterCat>(Actor))
        {
            if (BlackboardComp)
            {
                BlackboardComp->SetValueAsObject(BB_PerceivedCat, Actor);
            }
            return;
        }
    }
    if (BlackboardComp)
    {
        BlackboardComp->ClearValue(BB_PerceivedCat);
    }
}

void AMPAIControllerHumanPlayer::ChooseNextVoluntaryAction()
{
    float Total = ProbIdle + ProbWander + ProbSearch + ProbInteractContext;
    if (Total <= 0.f || !BlackboardComp) return;

    float Rand = FMath::FRand() * Total;
    EAIHumanState Chosen = EAIHumanState::Idle;
    float Acc = ProbIdle;
    if (Rand < Acc) {
        Chosen = EAIHumanState::Idle;
    } else if ((Acc += ProbWander) && Rand < Acc) {
        Chosen = EAIHumanState::Wander;
    } else if ((Acc += ProbSearch) && Rand < Acc) {
        Chosen = EAIHumanState::Search;
    } else {
        Chosen = EAIHumanState::InteractContext;
    }

    BlackboardComp->SetValueAsEnum(BB_StateKey, static_cast<uint8>(Chosen));
}

void AMPAIControllerHumanPlayer::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    // React to hearing (noise) stimulus
    if (Stimulus.WasSuccessfullySensed())
    {
        // If Actor is nullptr, this is likely a noise stimulus
        if (Actor == nullptr)
        {
            if (BlackboardComp)
            {
                BlackboardComp->SetValueAsVector(BB_NoiseLocation, Stimulus.StimulusLocation);
                BlackboardComp->SetValueAsEnum(BB_StateKey, static_cast<uint8>(EAIHumanState::Search));
            }
        }
    }
} 

void AMPAIControllerHumanPlayer::SetAISystem(AMPAISystemManager* theManager)
{ 
    AISystem = theManager; 
}

bool AMPAIControllerHumanPlayer::IsBusyWithGlobalTask()
{
    return isBeingAssined;
}

void AMPAIControllerHumanPlayer::AssignGlobalTask(AMPEnvActorComp* urgentActor)
{
    if (BlackboardComp)
    {
        BlackboardComp->SetValueAsObject(BB_GlobalTaskActor, urgentActor);
        isBeingAssined = true;
    }
}