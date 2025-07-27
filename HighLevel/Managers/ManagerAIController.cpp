#include "ManagerAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "../../CommonEnum.h"
#include "../MPGMGameplay.h"
#include "../Factory/FactoryAIController.h"

#include "../Managers/ManagerLog.h"
#include "../../MPActor/AI/MPAIControllerHumanPlayer.h"
#include "../../MPActor/AI/MPAIController.h"

bool UManagerAIController::AddBot(ETeam team)
{
    if (!GameMode || !GameMode->aiControllerFactoryInstance)
    {
        UManagerLog::LogError(TEXT("Failed to create AI controller for bot: Factory instance missing"), TEXT("ManagerAIController"));
        return false;
    }

    AMPAIController* NewAI = GameMode->aiControllerFactoryInstance->SpawnAIController(team);
    if (!NewAI)
    {
        UManagerLog::LogError(TEXT("Failed to spawn AI controller"), TEXT("ManagerAIController"));
        return false;
    }

    if (team == ETeam::EHuman)
    {
        AllAIHumans.Add(NewAI);
        UManagerLog::LogInfo(FString::Printf(TEXT("Added human bot. Total human AIs: %d"), AllAIHumans.Num()), TEXT("ManagerAIController"));
    }
    else if (team == ETeam::ECat)
    {
        AllAICats.Add(NewAI);
        UManagerLog::LogInfo(FString::Printf(TEXT("Added cat bot. Total cat AIs: %d"), AllAICats.Num()), TEXT("ManagerAIController"));
    }
    return true;
}

bool UManagerAIController::RemoveBot(int32 playerIndex)
{
    int32 HumanCount = AllAIHumans.Num();
    bool Removed = false;

    if (playerIndex < HumanCount)
    {
        AMPAIController* Ctrl = AllAIHumans[playerIndex];
        if (Ctrl)
        {
            Ctrl->Destroy();
        }
        AllAIHumans.RemoveAt(playerIndex);
        Removed = true;
        UManagerLog::LogInfo(FString::Printf(TEXT("Removed human bot idx %d"), playerIndex), TEXT("ManagerAIController"));
    }
    else
    {
        int32 CatIndex = playerIndex - HumanCount;
        if (AllAICats.IsValidIndex(CatIndex))
        {
            AMPAIController* Ctrl = AllAICats[CatIndex];
            if (Ctrl)
            {
                Ctrl->Destroy();
            }
            AllAICats.RemoveAt(CatIndex);
            Removed = true;
            UManagerLog::LogInfo(FString::Printf(TEXT("Removed cat bot idx %d"), CatIndex), TEXT("ManagerAIController"));
        }
    }

    if (!Removed)
    {
        UManagerLog::LogWarning(FString::Printf(TEXT("Bot idx %d not found"), playerIndex), TEXT("ManagerAIController"));
    }
    return Removed;
}

void UManagerAIController::SpawnLobbyAIs()
{
    if (!GameMode) return;

    // Helper lambdas
    auto SpawnCatPawn = [&](AMPAIController* Controller, int32 Index)
    {
        if (!Controller || !GameMode->catFactoryInstance) return;
        int MaxCatRace = static_cast<int>(ECatRace::EDiedCat);
        int RandomRace = FMath::RandRange(0, MaxCatRace - 1);
        FVector Loc = GameMode->allCatSpawnLocations.IsValidIndex(Index) ? GameMode->allCatSpawnLocations[Index] : FVector::ZeroVector;
        FRotator Rot = GameMode->allCatSpawnRotations.IsValidIndex(Index) ? GameMode->allCatSpawnRotations[Index] : FRotator::ZeroRotator;
        AActor* PawnActor = GameMode->catFactoryInstance->SpawnMPActor(RandomRace, Loc, Rot);
        if (PawnActor)
        {
            APawn* Pawn = Cast<APawn>(PawnActor);
            if (Pawn)
            {
                Controller->Possess(Pawn);
            }
        }
    };

    auto SpawnHumanPawn = [&](AMPAIController* Controller, int32 Index)
    {
        if (!Controller || !GameMode->humanFactoryInstance) return;
        int MaxHuman = static_cast<int>(EHumanProfession::EDiedHuman);
        int RandomProf = FMath::RandRange(0, MaxHuman - 1);
        FVector Loc = GameMode->allHumanSpawnLocations.IsValidIndex(Index) ? GameMode->allHumanSpawnLocations[Index] : FVector::ZeroVector;
        FRotator Rot = GameMode->allHumanSpawnRotations.IsValidIndex(Index) ? GameMode->allHumanSpawnRotations[Index] : FRotator::ZeroRotator;
        AActor* PawnActor = GameMode->humanFactoryInstance->SpawnMPActor(RandomProf, Loc, Rot);
        if (PawnActor)
        {
            APawn* Pawn = Cast<APawn>(PawnActor);
            if (Pawn)
            {
                Controller->Possess(Pawn);
            }
        }
    };

    for (int32 i=0;i<AllAICats.Num();++i)
    {
        SpawnCatPawn(AllAICats[i], i);
    }
    for (int32 i=0;i<AllAIHumans.Num();++i)
    {
        SpawnHumanPawn(AllAIHumans[i], i);
    }
}

void UManagerAIController::AddGlobalSearchTask(const FVector& NoiseLocation)
{
    FGlobalAITask NewTask;
    NewTask.Location = NoiseLocation;
    PendingTasks.Add(NewTask);

    AssignGlobalTasks();
}

void UManagerAIController::AssignGlobalTasks()
{
    for (FGlobalAITask& Task : PendingTasks)
    {
        if (Task.bAssigned) continue;

        // Find first idle human AI
        for (AMPAIController* Ctrl : AllAIHumans)
        {
            if (!Ctrl) continue;
            UBlackboardComponent* BB = Ctrl->GetBB();
            if (!BB) continue;

            uint8 CurState = BB->GetValueAsEnum(TEXT("State"));
            if (CurState == static_cast<uint8>(EAIHumanState::Idle) || CurState == static_cast<uint8>(EAIHumanState::Wander))
            {
                BB->SetValueAsVector(TEXT("NoiseLocation"), Task.Location);
                BB->SetValueAsEnum(TEXT("State"), static_cast<uint8>(EAIHumanState::Search));
                Task.bAssigned = true;
                break;
            }
        }
    }

    // Remove completed tasks (all assigned)
    PendingTasks.RemoveAll([](const FGlobalAITask& T){ return T.bAssigned; });
} 


void UManagerAIController::SetNumAICats(int aiCatNum)
{
	aiCatNumber = aiCatNum;
}
void UManagerAIController::SetNumAIHumans(int aiHumanNum)
{
	aiHumanNumber = aiHumanNum;
}