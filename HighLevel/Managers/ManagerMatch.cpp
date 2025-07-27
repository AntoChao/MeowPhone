#include "ManagerMatch.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "../MPGMGameplay.h"
#include "../MPGS.h"

#include "../Managers/ManagerLog.h"
#include "../Managers/ManagerAIController.h"

#include "../Factory/FactoryHuman.h"
#include "../Factory/FactoryCat.h"
#include "../Factory/FactoryAIController.h"

#include "../../MPActor/Player/MPControllerPlayer.h"
#include "../../MPActor/Player/MPPlayerState.h"
#include "../../MPActor/Character/MPCharacter.h"
#include "../../MPActor/Character/MPCharacterHuman.h"
#include "../../MPActor/Character/MPCharacterCat.h"
#include "../../MPActor/Item/MPItem.h"
#include "../../MPActor/EnvActor/MPEnvActorComp.h"
#include "../../MPActor/EnvActor/MPEnvActorCompPushable.h"
#include "../../MPActor/AI/MPAIController.h"
#include "../../MPActor/AI/MPAISystemManager.h"
#include "../../MPActor/Player/Widget/HUDLobbyManager.h"
#include "../../MPActor/Player/Widget/HUDEnd.h"

// character custom
void UManagerMatch::StartCustomizeCharacter()
{
    if (!gameMode || !gameMode->GetGameState()) return;

    gameMode->GetGameState()->curGameplayStatus = EGPStatus::ECustomCharacter;
    gameMode->GetGameState()->curCustomCharacterTime = gameMode->GetGameState()->customCharacterTotalTime;

    for (AMPControllerPlayer* eachPlayer : gameMode->GetAllPlayerControllers())
    {
        if (eachPlayer && eachPlayer->GetManagerLobbyHUD())
        {
            AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
            if (eachState)
            {
                eachPlayer->GetManagerLobbyHUD()->ShowCustomizationHUD(eachState->playerTeam);
            }
        }
    }

    CountdownCustomizeCharacter();
}

void UManagerMatch::CountdownCustomizeCharacter()
{
    if (!gameMode || !gameMode->GetGameState()) return;

    if (gameMode->GetGameState()->curCustomCharacterTime > 0)
    {
        UWorld* serverWorld = gameMode->GetWorld();
        if (serverWorld)
        {
            gameMode->GetGameState()->curCustomCharacterTime -= 1;
            gameMode->ClientUpdateCustomizationCountdown(gameMode->GetGameState()->curCustomCharacterTime);

            serverWorld->GetTimerManager().ClearTimer(customCharacterTimerHandle);
            FTimerDelegate customCharacterTimerDel;
            customCharacterTimerDel.BindUFunction(this, FName("CountdownCustomizeCharacter"));
            serverWorld->GetTimerManager().SetTimer(customCharacterTimerHandle, customCharacterTimerDel, 1, false);
        }
    }
    else
    {
        EndCustomizeCharacter();
    }
}

void UManagerMatch::EndCustomizeCharacter()
{
    if (!gameMode) return;

    for (AMPControllerPlayer* eachPlayer : gameMode->GetAllPlayerControllers())
    {
        if (eachPlayer)
        {
            eachPlayer->RemoveHUD(EHUDType::ELobby);
        }
    }

    StartGame();
}

// start game
void UManagerMatch::StartGame()
{
    UManagerLog::LogInfo(TEXT("Starting Game"), TEXT("ManagerMatch"));
    SetupGame();
    StartPrepareTime();
}

void UManagerMatch::SetupGame()
{
    SetupMap();
    SetupPlayers();
    SetupAIs();
}

void UManagerMatch::SetupMap()
{
    SetupMapItems();
    SetupMapEnvActors();
}

void UManagerMatch::SetupMapItems()
{
    if (!gameMode) return;
    TArray<AActor*> allActors;
    UGameplayStatics::GetAllActorsOfClass(gameMode->GetWorld(), AMPItem::StaticClass(), allActors);

    for (AActor* eachActor : allActors)
    {
        AMPItem* eachItem = Cast<AMPItem>(eachActor);
        if (eachItem)
        {
            int32 randomNumber = FMath::RandRange(1, 100);
            if (randomNumber > gameMode->itemRemainPercentage)
            {
                eachItem->GetEliminated();
            }
        }
    }
}

void UManagerMatch::SetupMapEnvActors()
{
    if (!gameMode || !gameMode->GetGameState()) return;

    TArray<AActor*> allActors;
    UGameplayStatics::GetAllActorsOfClass(gameMode->GetWorld(), AMPEnvActorComp::StaticClass(), allActors);

    for (AActor* eachActor : allActors)
    {
        AMPEnvActorComp* eachEnvActor = Cast<AMPEnvActorComp>(eachActor);
        if (eachEnvActor)
        {
            int32 randomNumber = FMath::RandRange(1, 100);
            if (randomNumber > gameMode->envActorRandomnessPercentage)
            {
                eachEnvActor->BeRandomized();
            }
        }
    }

    float totalProgressionWeight = 0.0f;
    TArray<AActor*> remainingActors;
    UGameplayStatics::GetAllActorsOfClass(gameMode->GetWorld(), AMPEnvActorComp::StaticClass(), remainingActors);

    for (AActor* eachActor : remainingActors)
    {
        AMPEnvActorComp* eachEnvActor = Cast<AMPEnvActorComp>(eachActor);
        if (eachEnvActor)
        {
            AMPEnvActorCompPushable* pushableActor = Cast<AMPEnvActorCompPushable>(eachEnvActor);
            if (pushableActor)
            {
                totalProgressionWeight += pushableActor->GetProgressionWeight();
            }
        }
    }

    gameMode->GetGameState()->totalMPProgression = totalProgressionWeight;
    gameMode->GetGameState()->curMPProgression = 0.0f;
    gameMode->GetGameState()->curMPProgressionPercentage = 0.0f;

    if (totalProgressionWeight <= 0.0f)
    {
        UManagerLog::LogWarning(TEXT("No pushable objects remain after randomization! Cat team objective is impossible."), TEXT("ManagerMatch"));
    }
    else
    {
        float requiredProgression = totalProgressionWeight * gameMode->GetGameState()->catWinProgressionPercentage;
        UManagerLog::LogInfo(FString::Printf(TEXT("Cat Team Objective Set: Total Weight = %f, Required = %f (%.1f%%)"),
        totalProgressionWeight, requiredProgression, gameMode->GetGameState()->catWinProgressionPercentage * 100.0f), TEXT("ManagerMatch"));
    }
}

void UManagerMatch::SetupPlayers()
{
    if (!gameMode || !gameMode->GetGameState()) return;

    int humanIndex = 0;
    int catIndex = 0;
    int catPlayerCount = 0;

    for (AMPControllerPlayer* eachPlayer : gameMode->GetAllPlayerControllers())
    {
        AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
        if (eachState)
        {
            if (eachState->playerTeam == ETeam::EHuman)
            {
                if (gameMode->humanFactoryInstance)
                {
                    int professionInt = static_cast<int>(eachState->humanProfession);
                    AActor* humanBody = gameMode->humanFactoryInstance->SpawnMPActor(professionInt, gameMode->allHumanSpawnLocations[humanIndex], gameMode->allHumanSpawnRotations[humanIndex]);

                    if (humanBody)
                    {
                        AMPCharacter* humanMPBody = Cast<AMPCharacter>(humanBody);
                        if (humanMPBody)
                        {
                            eachPlayer->Possess(humanMPBody);
                            gameMode->GetAllPlayerCharacters().Add(humanMPBody);
                            humanIndex++;
                            UManagerLog::LogInfo(TEXT("One human player created successfully"), TEXT("ManagerMatch"));
                        }
                    }
                }
                eachPlayer->AttachHUD(EHUDType::EGameplayHuman, 0);
            }
            else if (eachState->playerTeam == ETeam::ECat)
            {
                catPlayerCount++;
                if (gameMode->catFactoryInstance)
                {
                    int catInt = static_cast<int>(eachState->catRace);
                    AActor* catBody = gameMode->catFactoryInstance->SpawnMPActor(catInt, gameMode->allCatSpawnLocations[catIndex], gameMode->allCatSpawnRotations[catIndex]);

                    if (catBody)
                    {
                        AMPCharacter* catMPBody = Cast<AMPCharacter>(catBody);
                        if (catMPBody)
                        {
                            eachPlayer->Possess(catMPBody);
                            gameMode->GetAllPlayerCharacters().Add(catMPBody);
                            catIndex++;
                            UManagerLog::LogInfo(TEXT("One cat player created successfully"), TEXT("ManagerMatch"));
                        }
                    }
                }
                eachPlayer->AttachHUD(EHUDType::EGameplayCat, 0);
            }
        }
    }

    gameMode->GetGameState()->totalCatPlayers = catPlayerCount;
    gameMode->GetGameState()->caughtCats = 0;
    gameMode->GetGameState()->caughtCatsPercentage = 0.0f;

    if (catPlayerCount <= 0)
    {
        UManagerLog::LogWarning(TEXT("No cat players found! Human team objective is impossible."), TEXT("ManagerMatch"));
    }
    else
    {
        UManagerLog::LogInfo(FString::Printf(TEXT("Human Team Objective Set: Catch %d Cat Players"), catPlayerCount), TEXT("ManagerMatch"));
    }
}

void UManagerMatch::SetupAIs()
{
    if (gameMode && gameMode->GetManagerAIController())
    {
        gameMode->GetManagerAIController()->SpawnLobbyAIs();
        gameMode->GetManagerAIController()->SetupAIManager();
    }
}

// prepare time
void UManagerMatch::StartPrepareTime()
{
    if (!gameMode || !gameMode->GetGameState()) return;

    gameMode->GetGameState()->curGameplayStatus = EGPStatus::EPrepare;
    gameMode->GetGameState()->curPrepareTime = gameMode->GetGameState()->prepareTotalTime;
    CountdownPrepareGame();

    for (AMPControllerPlayer* eachPlayer : gameMode->GetAllPlayerControllers())
    {
        eachPlayer->PrepareStartUpdate();
    }
}

void UManagerMatch::CountdownPrepareGame()
{
    if (!gameMode || !gameMode->GetGameState()) return;

    if (gameMode->GetGameState()->curPrepareTime > 0)
    {
        UWorld* serverWorld = gameMode->GetWorld();
        if (serverWorld)
        {
            gameMode->GetGameState()->curPrepareTime -= 1;

            serverWorld->GetTimerManager().ClearTimer(prepareTimerHandle);
            FTimerDelegate prepareTimerDel;
            prepareTimerDel.BindUFunction(this, FName("CountdownPrepareGame"));
            serverWorld->GetTimerManager().SetTimer(prepareTimerHandle, prepareTimerDel, 1, false);
        }
    }
    else
    {
        EndPrepareTime();
    }
}

void UManagerMatch::EndPrepareTime()
{
    StartGameplayTime();
}

void UManagerMatch::StartGameplayTime()
{
    if (!gameMode || !gameMode->GetGameState()) return;

    gameMode->GetGameState()->curGameplayStatus = EGPStatus::EGameplay;
    gameMode->GetGameState()->curGameplayTime = gameMode->GetGameState()->gameplayTotalTime;

    bool catObjectiveImpossible = (gameMode->GetGameState()->totalMPProgression <= 0.0f);
    bool humanObjectiveImpossible = (gameMode->GetGameState()->totalCatPlayers <= 0);

    if (catObjectiveImpossible && humanObjectiveImpossible)
    {
        UManagerLog::LogError(TEXT("CRITICAL ERROR: Both teams have impossible objectives!"), TEXT("ManagerMatch"));
    }
    else if (catObjectiveImpossible)
    {
        UManagerLog::LogWarning(TEXT("Cat team has impossible objective - no objects to break"), TEXT("ManagerMatch"));
    }
    else if (humanObjectiveImpossible)
    {
        UManagerLog::LogWarning(TEXT("Human team has impossible objective - no cat players to catch"), TEXT("ManagerMatch"));
    }

    CountdownGameplayGame();

    for (AMPControllerPlayer* eachPlayer : gameMode->GetAllPlayerControllers())
    {
        eachPlayer->GameplayStartUpdate();
    }
}

void UManagerMatch::CountdownGameplayGame()
{
    if (!gameMode || !gameMode->GetGameState()) return;

    if (gameMode->GetGameState()->curGameplayTime > 0)
    {
        UWorld* serverWorld = gameMode->GetWorld();
        if (serverWorld)
        {
            gameMode->GetGameState()->curGameplayTime -= 1;
            CheckIfGameEnd();

            if (gameMode->GetGameState()->curGameplayTime > 0)
            {
                serverWorld->GetTimerManager().ClearTimer(gameplayTimerHandle);
                FTimerDelegate gameplayTimerDel;
                gameplayTimerDel.BindUFunction(this, FName("CountdownGameplayGame"));
                serverWorld->GetTimerManager().SetTimer(gameplayTimerHandle, gameplayTimerDel, 1, false);
            }
        }
    }
    else
    {
        CheckIfGameEnd();
    }
}

bool UManagerMatch::CheckIfGameEnd()
{
    if (!gameMode || !gameMode->GetGameState()) return false;

    bool isGameEnd = false;
    FString winningTeam = TEXT("");

    if (gameMode->GetGameState()->totalMPProgression > 0.0f && gameMode->GetGameState()->curMPProgression >= 0.0f)
    {
        bool hasReachedTarget = (gameMode->GetGameState()->curMPProgressionPercentage >= gameMode->GetGameState()->catWinProgressionPercentage);
        if (hasReachedTarget)
        {
            isGameEnd = true;
            winningTeam = TEXT("Cat");
        }
    }

    if (!isGameEnd && gameMode->GetGameState()->totalCatPlayers > 0)
    {
        bool hasCaughtAllCats = (gameMode->GetGameState()->caughtCats >= gameMode->GetGameState()->totalCatPlayers) ||
                                (gameMode->GetGameState()->caughtCatsPercentage >= 0.999f);
        if (hasCaughtAllCats)
        {
            isGameEnd = true;
            winningTeam = TEXT("Human");
        }
    }

    if (!isGameEnd)
    {
        bool allHumansDead = true;
        for (AMPControllerPlayer* eachPlayer : gameMode->GetAllPlayerControllers())
        {
            AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
            if (eachState && eachState->playerTeam == ETeam::EHuman)
            {
                AMPCharacterHuman* humanChar = Cast<AMPCharacterHuman>(eachPlayer->GetPawn());
                if (humanChar && !humanChar->IsDead())
                {
                    allHumansDead = false;
                    break;
                }
            }
        }
        if (allHumansDead)
        {
            isGameEnd = true;
            winningTeam = TEXT("Cat (All Humans Dead)");
        }
    }

    if (!isGameEnd && gameMode->GetGameState()->curGameplayTime == 0)
    {
        if (gameMode->GetGameState()->curMPProgressionPercentage > gameMode->GetGameState()->caughtCatsPercentage)
        {
            isGameEnd = true;
            winningTeam = TEXT("Cat (Time Out)");
        }
        else if (gameMode->GetGameState()->caughtCatsPercentage > gameMode->GetGameState()->curMPProgressionPercentage)
        {
            isGameEnd = true;
            winningTeam = TEXT("Human (Time Out)");
        }
        else
        {
            isGameEnd = true;
            winningTeam = TEXT("Cat (Time Out Tie)");
        }
    }

    if (isGameEnd)
    {
        UManagerLog::LogInfo(FString::Printf(TEXT("=== GAME END: %s Team Wins! ==="), *winningTeam), TEXT("ManagerMatch"));
        EndGameplayTime();
        return true;
    }

    return false;
}

void UManagerMatch::EndGameplayTime()
{
    if (!gameMode || !gameMode->GetGameState()) return;

    RemoveGameplayHUD();

    FString resultKey;
    float humanProgress = 0.0f;
    float catProgress = 0.0f;
    
    bool catWin = false;
    bool humanWin = false;
    if (gameMode->GetGameState()->curMPProgressionPercentage >= gameMode->GetGameState()->catWinProgressionPercentage)
        catWin = true;
    if (gameMode->GetGameState()->caughtCatsPercentage >= 0.999f || gameMode->GetGameState()->caughtCats >= gameMode->GetGameState()->totalCatPlayers)
        humanWin = true;
    if (!catWin && !humanWin && gameMode->GetGameState()->curGameplayTime == 0)
    {
        if (gameMode->GetGameState()->curMPProgressionPercentage > gameMode->GetGameState()->caughtCatsPercentage)
            catWin = true;
        else
            humanWin = true;
    }
    humanProgress = gameMode->GetGameState()->caughtCatsPercentage;
    catProgress = gameMode->GetGameState()->curMPProgressionPercentage;

    for (AMPControllerPlayer* eachPlayer : gameMode->GetAllPlayerControllers())
    {
        if (eachPlayer)
        {
            eachPlayer->AttachHUD(EHUDType::EEnd, 0);
            UHUDEnd* endHUD = eachPlayer->GetEndHUD();
            if (endHUD)
            {
                AMPPlayerState* state = Cast<AMPPlayerState>(eachPlayer->PlayerState);
                bool isCat = (state && state->playerTeam == ETeam::ECat);
                bool isHuman = (state && state->playerTeam == ETeam::EHuman);
                if (isCat)
                    resultKey = catWin ? TEXT("WIN") : TEXT("LOSE");
                else if (isHuman)
                    resultKey = humanWin ? TEXT("WIN") : TEXT("LOSE");
                else
                    resultKey = TEXT("LOSE");
                endHUD->SetResultText(resultKey);
                endHUD->SetHumanProgression(humanProgress);
                endHUD->SetCatProgression(catProgress);
                endHUD->SetReturnLobbyButtonText(TEXT("RETURN_LOBBY"));
            }
        }
    }

    UManagerLog::LogInfo(TEXT("Game ended. Players can manually restart or wait for auto-restart."), TEXT("ManagerMatch"));
}

void UManagerMatch::RemoveGameplayHUD()
{
    if (!gameMode) return;
    for (AMPControllerPlayer* eachPlayer : gameMode->GetAllPlayerControllers())
    {
        if (eachPlayer)
        {
            AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
            if (eachState)
            {
                switch (eachState->playerTeam)
                {
                case ETeam::EHuman:
                    eachPlayer->RemoveHUD(EHUDType::EGameplayHuman);
                    break;
                case ETeam::ECat:
                    eachPlayer->RemoveHUD(EHUDType::EGameplayCat);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void UManagerMatch::RegisterPlayerDeath(AMPControllerPlayer* diedPlayer, FVector diedPlayerLocation, FRotator diedPlayerRotation)
{
    if (!gameMode || !diedPlayer || CheckIfGameEnd()) return;

    gameMode->RemoveControlledCharacters(diedPlayer);

    AMPPlayerState* diedPlayerState = Cast<AMPPlayerState>(diedPlayer->PlayerState);
    if (diedPlayerState)
    {
        AActor* diedBody = nullptr;
        if (diedPlayerState->playerTeam == ETeam::EHuman)
        {
            if (gameMode->humanFactoryInstance)
            {
                diedBody = gameMode->humanFactoryInstance->SpawnMPActor(static_cast<int>(EHumanProfession::EDiedHuman), diedPlayerLocation, diedPlayerRotation);
            }
        }
        else if (diedPlayerState->playerTeam == ETeam::ECat)
        {
            if (gameMode->catFactoryInstance)
            {
                diedBody = gameMode->catFactoryInstance->SpawnMPActor(static_cast<int>(ECatRace::EDiedCat), diedPlayerLocation, diedPlayerRotation);
            }
        }

        if (diedBody)
        {
            AMPCharacter* mpDiedBody = Cast<AMPCharacter>(diedBody);
            if (mpDiedBody)
            {
                diedPlayer->Possess(mpDiedBody);
                gameMode->GetAllPlayerCharacters().Add(mpDiedBody);
            }
        }
    }
}

void UManagerMatch::DisplayProgressionStatus()
{
    if (!gameMode || !gameMode->GetGameState()) return;

    UManagerLog::LogInfo(TEXT("=== Game Progression Status ==="), TEXT("ManagerMatch"));
    UManagerLog::LogInfo(FString::Printf(TEXT("Cat Team: %f/%f (%.1f%%) - Required: %.1f%%"),
        gameMode->GetGameState()->curMPProgression, gameMode->GetGameState()->totalMPProgression,
        gameMode->GetGameState()->curMPProgressionPercentage * 100.0f,
        gameMode->GetGameState()->catWinProgressionPercentage * 100.0f), TEXT("ManagerMatch"));
    UManagerLog::LogInfo(FString::Printf(TEXT("Human Team: %d/%d (%.1f%%)"),
        gameMode->GetGameState()->caughtCats, gameMode->GetGameState()->totalCatPlayers,
        gameMode->GetGameState()->caughtCatsPercentage * 100.0f), TEXT("ManagerMatch"));
    UManagerLog::LogInfo(FString::Printf(TEXT("Time Remaining: %d seconds"), gameMode->GetGameState()->curGameplayTime), TEXT("ManagerMatch"));
    UManagerLog::LogInfo(TEXT("================================"), TEXT("ManagerMatch"));
} 