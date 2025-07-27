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
#include "../../MPActor/Player/Widget/HUDManagerLobby.h"
#include "../../MPActor/Player/Widget/HUDEnd.h"

// character custom
void UManagerMatch::StartCustomizeCharacter()
{
    if (!GameMode || !GameMode->GetGameState()) return;

    GameMode->GetGameState()->curGameplayStatus = EGPStatus::ECustomCharacter;
    GameMode->GetGameState()->curCustomCharacterTime = GameMode->GetGameState()->customCharacterTotalTime;

    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
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
    if (!GameMode || !GameMode->GetGameState()) return;

    if (GameMode->GetGameState()->curCustomCharacterTime > 0)
    {
        UWorld* serverWorld = GameMode->GetWorld();
        if (serverWorld)
        {
            GameMode->GetGameState()->curCustomCharacterTime -= 1;
            GameMode->ClientUpdateCustomizationCountdown(GameMode->GetGameState()->curCustomCharacterTime);

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
    if (!GameMode) return;

    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
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
    if (!GameMode) return;
    TArray<AActor*> allActors;
    UGameplayStatics::GetAllActorsOfClass(GameMode->GetWorld(), AMPItem::StaticClass(), allActors);

    for (AActor* eachActor : allActors)
    {
        AMPItem* eachItem = Cast<AMPItem>(eachActor);
        if (eachItem)
        {
            int32 randomNumber = FMath::RandRange(1, 100);
            if (randomNumber > GameMode->itemRemainPercentage)
            {
                eachItem->GetEliminated();
            }
        }
    }
}

void UManagerMatch::SetupMapEnvActors()
{
    if (!GameMode || !GameMode->GetGameState()) return;

    TArray<AActor*> allActors;
    UGameplayStatics::GetAllActorsOfClass(GameMode->GetWorld(), AMPEnvActorComp::StaticClass(), allActors);

    for (AActor* eachActor : allActors)
    {
        AMPEnvActorComp* eachEnvActor = Cast<AMPEnvActorComp>(eachActor);
        if (eachEnvActor)
        {
            int32 randomNumber = FMath::RandRange(1, 100);
            if (randomNumber > GameMode->envActorRandomnessPercentage)
            {
                eachEnvActor->BeRandomized();
            }
        }
    }

    float totalProgressionWeight = 0.0f;
    TArray<AActor*> remainingActors;
    UGameplayStatics::GetAllActorsOfClass(GameMode->GetWorld(), AMPEnvActorComp::StaticClass(), remainingActors);

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

    GameMode->GetGameState()->totalMPProgression = totalProgressionWeight;
    GameMode->GetGameState()->curMPProgression = 0.0f;
    GameMode->GetGameState()->curMPProgressionPercentage = 0.0f;

    if (totalProgressionWeight <= 0.0f)
    {
        UManagerLog::LogWarning(TEXT("No pushable objects remain after randomization! Cat team objective is impossible."), TEXT("ManagerMatch"));
    }
    else
    {
        float requiredProgression = totalProgressionWeight * GameMode->GetGameState()->catWinProgressionPercentage;
        UManagerLog::LogInfo(FString::Printf(TEXT("Cat Team Objective Set: Total Weight = %f, Required = %f (%.1f%%)"),
        totalProgressionWeight, requiredProgression, GameMode->GetGameState()->catWinProgressionPercentage * 100.0f), TEXT("ManagerMatch"));
    }
}

void UManagerMatch::SetupPlayers()
{
    if (!GameMode || !GameMode->GetGameState()) return;

    int humanIndex = 0;
    int catIndex = 0;
    int catPlayerCount = 0;

    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
        if (eachState)
        {
            if (eachState->playerTeam == ETeam::EHuman)
            {
                if (GameMode->humanFactoryInstance)
                {
                    int professionInt = static_cast<int>(eachState->humanProfession);
                    AActor* humanBody = GameMode->humanFactoryInstance->SpawnMPActor(professionInt, GameMode->allHumanSpawnLocations[humanIndex], GameMode->allHumanSpawnRotations[humanIndex]);

                    if (humanBody)
                    {
                        AMPCharacter* humanMPBody = Cast<AMPCharacter>(humanBody);
                        if (humanMPBody)
                        {
                            eachPlayer->Possess(humanMPBody);
                            GameMode->allPlayerCharacters.Add(humanMPBody);
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
                if (GameMode->catFactoryInstance)
                {
                    int catInt = static_cast<int>(eachState->catRace);
                    AActor* catBody = GameMode->catFactoryInstance->SpawnMPActor(catInt, GameMode->allCatSpawnLocations[catIndex], GameMode->allCatSpawnRotations[catIndex]);

                    if (catBody)
                    {
                        AMPCharacter* catMPBody = Cast<AMPCharacter>(catBody);
                        if (catMPBody)
                        {
                            eachPlayer->Possess(catMPBody);
                            GameMode->allPlayerCharacters.Add(catMPBody);
                            catIndex++;
                            UManagerLog::LogInfo(TEXT("One cat player created successfully"), TEXT("ManagerMatch"));
                        }
                    }
                }
                eachPlayer->AttachHUD(EHUDType::EGameplayCat, 0);
            }
        }
    }

    GameMode->GetGameState()->totalCatPlayers = catPlayerCount;
    GameMode->GetGameState()->caughtCats = 0;
    GameMode->GetGameState()->caughtCatsPercentage = 0.0f;

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
    if (GameMode && GameMode->ManagerAIController)
    {
        GameMode->ManagerAIController->SpawnLobbyAIs();
    }
    SetupAIManager();
}

void UManagerMatch::SetupAICats()
{
    if (!GameMode || !GameMode->ManagerAIController) return;
    int numAICats = GameMode->ManagerAIController->GetAllAICats().Num();
    for (int i = 0; i < numAICats; i++)
    {
        if (GameMode->catFactoryInstance && GameMode->aiControllerFactoryInstance)
        {
            int maxCatRace = static_cast<int>(ECatRace::EDiedCat);
            int randomCatRace = FMath::RandRange(0, maxCatRace - 1);
            AActor* catBody = GameMode->catFactoryInstance->SpawnMPActor(randomCatRace, GameMode->allCatSpawnLocations[i], GameMode->allCatSpawnRotations[i]);

            AActor* aiController = GameMode->aiControllerFactoryInstance->SpawnMPActor(GameMode->catAIIndex, GameMode->allCatSpawnLocations[i], GameMode->allCatSpawnRotations[i]);

            if (catBody && aiController)
            {
                AMPCharacter* catAIMPBody = Cast<AMPCharacter>(catBody);
                AMPAIController* aiMPController = Cast<AMPAIController>(aiController);
                if (catAIMPBody && aiMPController)
                {
                    aiMPController->Possess(catAIMPBody);
                }
            }
        }
    }
}

void UManagerMatch::SetupAIHumans()
{
    if (!GameMode || !GameMode->ManagerAIController) return;
    int numAIHumans = GameMode->ManagerAIController->GetAllAIHumans().Num();
    for (int i = 0; i < numAIHumans; i++)
    {
        if (GameMode->humanFactoryInstance && GameMode->aiControllerFactoryInstance)
        {
            int maxHumanRace = static_cast<int>(EHumanProfession::EDiedHuman);
            int randomHumanRace = FMath::RandRange(0, maxHumanRace - 1);
            AActor* humanBody = GameMode->humanFactoryInstance->SpawnMPActor(randomHumanRace, GameMode->allHumanSpawnLocations[i], GameMode->allHumanSpawnRotations[i]);

            AActor* aiController = GameMode->aiControllerFactoryInstance->SpawnMPActor(GameMode->humanAIIndex, GameMode->allHumanSpawnLocations[i], GameMode->allHumanSpawnRotations[i]);

            if (humanBody && aiController)
            {
                AMPCharacter* humanAIMPBody = Cast<AMPCharacter>(humanBody);
                AMPAIController* aiMPController = Cast<AMPAIController>(aiController);
                if (humanAIMPBody && aiMPController)
                {
                    aiMPController->Possess(humanAIMPBody);
                }
            }
        }
    }
}

void UManagerMatch::SetupAIManager()
{
    if (!GameMode || !GameMode->aiControllerFactoryInstance) return;

    GameMode->theHumanAIManager = Cast<AMPAISystemManager>(GameMode->aiControllerFactoryInstance->SpawnMPActor(GameMode->aiManagerIndex, FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f)));

    if (GameMode->theHumanAIManager)
    {
        GameMode->theHumanAIManager->Initialize();
    }
}

// prepare time
void UManagerMatch::StartPrepareTime()
{
    if (!GameMode || !GameMode->GetGameState()) return;

    GameMode->GetGameState()->curGameplayStatus = EGPStatus::EPrepare;
    GameMode->GetGameState()->curPrepareTime = GameMode->GetGameState()->prepareTotalTime;
    CountdownPrepareGame();

    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        eachPlayer->PrepareStartUpdate();
    }
}

void UManagerMatch::CountdownPrepareGame()
{
    if (!GameMode || !GameMode->GetGameState()) return;

    if (GameMode->GetGameState()->curPrepareTime > 0)
    {
        UWorld* serverWorld = GameMode->GetWorld();
        if (serverWorld)
        {
            GameMode->GetGameState()->curPrepareTime -= 1;

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
    if (!GameMode || !GameMode->GetGameState()) return;

    GameMode->GetGameState()->curGameplayStatus = EGPStatus::EGameplay;
    GameMode->GetGameState()->curGameplayTime = GameMode->GetGameState()->gameplayTotalTime;

    bool catObjectiveImpossible = (GameMode->GetGameState()->totalMPProgression <= 0.0f);
    bool humanObjectiveImpossible = (GameMode->GetGameState()->totalCatPlayers <= 0);

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

    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        eachPlayer->GameplayStartUpdate();
    }
}

void UManagerMatch::CountdownGameplayGame()
{
    if (!GameMode || !GameMode->GetGameState()) return;

    if (GameMode->GetGameState()->curGameplayTime > 0)
    {
        UWorld* serverWorld = GameMode->GetWorld();
        if (serverWorld)
        {
            GameMode->GetGameState()->curGameplayTime -= 1;
            CheckIfGameEnd();

            if (GameMode->GetGameState()->curGameplayTime > 0)
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
    if (!GameMode || !GameMode->GetGameState()) return false;

    bool isGameEnd = false;
    FString winningTeam = TEXT("");

    if (GameMode->GetGameState()->totalMPProgression > 0.0f && GameMode->GetGameState()->curMPProgression >= 0.0f)
    {
        bool hasReachedTarget = (GameMode->GetGameState()->curMPProgressionPercentage >= GameMode->GetGameState()->catWinProgressionPercentage);
        if (hasReachedTarget)
        {
            isGameEnd = true;
            winningTeam = TEXT("Cat");
        }
    }

    if (!isGameEnd && GameMode->GetGameState()->totalCatPlayers > 0)
    {
        bool hasCaughtAllCats = (GameMode->GetGameState()->caughtCats >= GameMode->GetGameState()->totalCatPlayers) ||
                                (GameMode->GetGameState()->caughtCatsPercentage >= 0.999f);
        if (hasCaughtAllCats)
        {
            isGameEnd = true;
            winningTeam = TEXT("Human");
        }
    }

    if (!isGameEnd)
    {
        bool allHumansDead = true;
        for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
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

    if (!isGameEnd && GameMode->GetGameState()->curGameplayTime == 0)
    {
        if (GameMode->GetGameState()->curMPProgressionPercentage > GameMode->GetGameState()->caughtCatsPercentage)
        {
            isGameEnd = true;
            winningTeam = TEXT("Cat (Time Out)");
        }
        else if (GameMode->GetGameState()->caughtCatsPercentage > GameMode->GetGameState()->curMPProgressionPercentage)
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
    if (!GameMode || !GameMode->GetGameState()) return;

    RemoveGameplayHUD();

    FString resultKey;
    float humanProgress = 0.0f;
    float catProgress = 0.0f;
    
    bool catWin = false;
    bool humanWin = false;
    if (GameMode->GetGameState()->curMPProgressionPercentage >= GameMode->GetGameState()->catWinProgressionPercentage)
        catWin = true;
    if (GameMode->GetGameState()->caughtCatsPercentage >= 0.999f || GameMode->GetGameState()->caughtCats >= GameMode->GetGameState()->totalCatPlayers)
        humanWin = true;
    if (!catWin && !humanWin && GameMode->GetGameState()->curGameplayTime == 0)
    {
        if (GameMode->GetGameState()->curMPProgressionPercentage > GameMode->GetGameState()->caughtCatsPercentage)
            catWin = true;
        else
            humanWin = true;
    }
    humanProgress = GameMode->GetGameState()->caughtCatsPercentage;
    catProgress = GameMode->GetGameState()->curMPProgressionPercentage;

    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
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
    if (!GameMode) return;
    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
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
    if (!GameMode || !diedPlayer || CheckIfGameEnd()) return;

    GameMode->RemoveControlledCharacters(diedPlayer);

    AMPPlayerState* diedPlayerState = Cast<AMPPlayerState>(diedPlayer->PlayerState);
    if (diedPlayerState)
    {
        AActor* diedBody = nullptr;
        if (diedPlayerState->playerTeam == ETeam::EHuman)
        {
            if (GameMode->humanFactoryInstance)
            {
                diedBody = GameMode->humanFactoryInstance->SpawnMPActor(static_cast<int>(EHumanProfession::EDiedHuman), diedPlayerLocation, diedPlayerRotation);
            }
        }
        else if (diedPlayerState->playerTeam == ETeam::ECat)
        {
            if (GameMode->catFactoryInstance)
            {
                diedBody = GameMode->catFactoryInstance->SpawnMPActor(static_cast<int>(ECatRace::EDiedCat), diedPlayerLocation, diedPlayerRotation);
            }
        }

        if (diedBody)
        {
            AMPCharacter* mpDiedBody = Cast<AMPCharacter>(diedBody);
            if (mpDiedBody)
            {
                diedPlayer->Possess(mpDiedBody);
                GameMode->allPlayerCharacters.Add(mpDiedBody);
            }
        }
    }
}

void UManagerMatch::DisplayProgressionStatus()
{
    if (!GameMode || !GameMode->GetGameState()) return;

    UManagerLog::LogInfo(TEXT("=== Game Progression Status ==="), TEXT("ManagerMatch"));
    UManagerLog::LogInfo(FString::Printf(TEXT("Cat Team: %f/%f (%.1f%%) - Required: %.1f%%"),
        GameMode->GetGameState()->curMPProgression, GameMode->GetGameState()->totalMPProgression,
        GameMode->GetGameState()->curMPProgressionPercentage * 100.0f,
        GameMode->GetGameState()->catWinProgressionPercentage * 100.0f), TEXT("ManagerMatch"));
    UManagerLog::LogInfo(FString::Printf(TEXT("Human Team: %d/%d (%.1f%%)"),
        GameMode->GetGameState()->caughtCats, GameMode->GetGameState()->totalCatPlayers,
        GameMode->GetGameState()->caughtCatsPercentage * 100.0f), TEXT("ManagerMatch"));
    UManagerLog::LogInfo(FString::Printf(TEXT("Time Remaining: %d seconds"), GameMode->GetGameState()->curGameplayTime), TEXT("ManagerMatch"));
    UManagerLog::LogInfo(TEXT("================================"), TEXT("ManagerMatch"));
} 