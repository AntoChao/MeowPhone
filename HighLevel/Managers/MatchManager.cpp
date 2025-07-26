#include "MatchManager.h"
#include "HighLevel/MPGMGameplay.h"
#include "HighLevel/MPGS.h"
#include "MPActor/Player/MPControllerPlayer.h"
#include "MPActor/Player/MPPlayerState.h"
#include "MPActor/Character/MPCharacter.h"
#include "MPActor/Character/MPCharacterHuman.h"
#include "MPActor/Character/MPCharacterCat.h"
#include "MPActor/Item/MPItem.h"
#include "MPActor/EnvActor/MPEnvActorComp.h"
#include "MPActor/EnvActor/MPEnvActorCompPushable.h"
#include "MPActor/AI/MPAIController.h"
#include "MPActor/AI/MPAISystemManager.h"
#include "HighLevel/Factory/FactoryHuman.h"
#include "HighLevel/Factory/FactoryCat.h"
#include "HighLevel/Factory/FactoryAIController.h"
#include "HighLevel/MPLogManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "MPActor/Player/Widget/HUDLobbyManager.h"
#include "MPActor/Player/Widget/HUDEnd.h"
#include "Managers/AIControllerManager.h"


void UMatchManager::Initialize(AMPGMGameplay* InGameMode)
{
    GameMode = InGameMode;
}

// character custom
void UMatchManager::StartCustomizeCharacter()
{
    if (!GameMode || !GameMode->theGameState) return;

    GameMode->theGameState->curGameplayStatus = EGPStatus::ECustomCharacter;
    GameMode->theGameState->curCustomCharacterTime = GameMode->theGameState->customCharacterTotalTime;

    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        if (eachPlayer && eachPlayer->lobbyManagerHUD)
        {
            AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
            if (eachState)
            {
                eachPlayer->lobbyManagerHUD->ShowCustomizationHUD(eachState->playerTeam);
            }
        }
    }

    CountdownCustomizeCharacter();
}

void UMatchManager::CountdownCustomizeCharacter()
{
    if (!GameMode || !GameMode->theGameState) return;

    if (GameMode->theGameState->curCustomCharacterTime > 0)
    {
        UWorld* serverWorld = GameMode->GetWorld();
        if (serverWorld)
        {
            GameMode->theGameState->curCustomCharacterTime -= 1;
            GameMode->ClientUpdateCustomizationCountdown(GameMode->theGameState->curCustomCharacterTime);

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

void UMatchManager::EndCustomizeCharacter()
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
void UMatchManager::StartGame()
{
    UMPLogManager::LogInfo(TEXT("Starting Game"), TEXT("MatchManager"));
    SetupGame();
    StartPrepareTime();
}

void UMatchManager::SetupGame()
{
    SetupMap();
    SetupPlayers();
    SetupAIs();
}

void UMatchManager::SetupMap()
{
    SetupMapItems();
    SetupMapEnvActors();
}

void UMatchManager::SetupMapItems()
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

void UMatchManager::SetupMapEnvActors()
{
    if (!GameMode || !GameMode->theGameState) return;

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

    GameMode->theGameState->totalMPProgression = totalProgressionWeight;
    GameMode->theGameState->curMPProgression = 0.0f;
    GameMode->theGameState->curMPProgressionPercentage = 0.0f;

    if (totalProgressionWeight <= 0.0f)
    {
        UMPLogManager::LogWarning(TEXT("No pushable objects remain after randomization! Cat team objective is impossible."), TEXT("MatchManager"));
    }
    else
    {
        float requiredProgression = totalProgressionWeight * GameMode->theGameState->catWinProgressionPercentage;
        UMPLogManager::LogInfo(FString::Printf(TEXT("Cat Team Objective Set: Total Weight = %f, Required = %f (%.1f%%)"),
            totalProgressionWeight, requiredProgression, GameMode->theGameState->catWinProgressionPercentage * 100.0f), TEXT("MatchManager"));
    }
}

void UMatchManager::SetupPlayers()
{
    if (!GameMode || !GameMode->theGameState) return;

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
                            UMPLogManager::LogInfo(TEXT("One human player created successfully"), TEXT("MatchManager"));
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
                            UMPLogManager::LogInfo(TEXT("One cat player created successfully"), TEXT("MatchManager"));
                        }
                    }
                }
                eachPlayer->AttachHUD(EHUDType::EGameplayCat, 0);
            }
        }
    }

    GameMode->theGameState->totalCatPlayers = catPlayerCount;
    GameMode->theGameState->caughtCats = 0;
    GameMode->theGameState->caughtCatsPercentage = 0.0f;

    if (catPlayerCount <= 0)
    {
        UMPLogManager::LogWarning(TEXT("No cat players found! Human team objective is impossible."), TEXT("MatchManager"));
    }
    else
    {
        UMPLogManager::LogInfo(FString::Printf(TEXT("Human Team Objective Set: Catch %d Cat Players"), catPlayerCount), TEXT("MatchManager"));
    }
}

void UMatchManager::SetupAIs()
{
    if (GameMode && GameMode->AIControllerManager)
    {
        GameMode->AIControllerManager->SpawnLobbyAIs();
    }
    SetupAIManager();
}

void UMatchManager::SetupAICats()
{
    if (!GameMode || !GameMode->AIControllerManager) return;
    int numAICats = GameMode->AIControllerManager->GetAllAICats().Num();
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

void UMatchManager::SetupAIHumans()
{
    if (!GameMode || !GameMode->AIControllerManager) return;
    int numAIHumans = GameMode->AIControllerManager->GetAllAIHumans().Num();
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

void UMatchManager::SetupAIManager()
{
    if (!GameMode || !GameMode->aiControllerFactoryInstance) return;

    GameMode->theHumanAIManager = Cast<AMPAISystemManager>(GameMode->aiControllerFactoryInstance->SpawnMPActor(GameMode->aiManagerIndex, FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f)));

    if (GameMode->theHumanAIManager)
    {
        GameMode->theHumanAIManager->Initialize();
    }
}

// prepare time
void UMatchManager::StartPrepareTime()
{
    if (!GameMode || !GameMode->theGameState) return;

    GameMode->theGameState->curGameplayStatus = EGPStatus::EPrepare;
    GameMode->theGameState->curPrepareTime = GameMode->theGameState->prepareTotalTime;
    CountdownPrepareGame();

    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        eachPlayer->PrepareStartUpdate();
    }
}

void UMatchManager::CountdownPrepareGame()
{
    if (!GameMode || !GameMode->theGameState) return;

    if (GameMode->theGameState->curPrepareTime > 0)
    {
        UWorld* serverWorld = GameMode->GetWorld();
        if (serverWorld)
        {
            GameMode->theGameState->curPrepareTime -= 1;

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

void UMatchManager::EndPrepareTime()
{
    StartGameplayTime();
}

void UMatchManager::StartGameplayTime()
{
    if (!GameMode || !GameMode->theGameState) return;

    GameMode->theGameState->curGameplayStatus = EGPStatus::EGameplay;
    GameMode->theGameState->curGameplayTime = GameMode->theGameState->gameplayTotalTime;

    bool catObjectiveImpossible = (GameMode->theGameState->totalMPProgression <= 0.0f);
    bool humanObjectiveImpossible = (GameMode->theGameState->totalCatPlayers <= 0);

    if (catObjectiveImpossible && humanObjectiveImpossible)
    {
        UMPLogManager::LogError(TEXT("CRITICAL ERROR: Both teams have impossible objectives!"), TEXT("MatchManager"));
    }
    else if (catObjectiveImpossible)
    {
        UMPLogManager::LogWarning(TEXT("Cat team has impossible objective - no objects to break"), TEXT("MatchManager"));
    }
    else if (humanObjectiveImpossible)
    {
        UMPLogManager::LogWarning(TEXT("Human team has impossible objective - no cat players to catch"), TEXT("MatchManager"));
    }

    CountdownGameplayGame();

    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        eachPlayer->GameplayStartUpdate();
    }
}

void UMatchManager::CountdownGameplayGame()
{
    if (!GameMode || !GameMode->theGameState) return;

    if (GameMode->theGameState->curGameplayTime > 0)
    {
        UWorld* serverWorld = GameMode->GetWorld();
        if (serverWorld)
        {
            GameMode->theGameState->curGameplayTime -= 1;
            CheckIfGameEnd();

            if (GameMode->theGameState->curGameplayTime > 0)
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

bool UMatchManager::CheckIfGameEnd()
{
    if (!GameMode || !GameMode->theGameState) return false;

    bool isGameEnd = false;
    FString winningTeam = TEXT("");

    if (GameMode->theGameState->totalMPProgression > 0.0f && GameMode->theGameState->curMPProgression >= 0.0f)
    {
        bool hasReachedTarget = (GameMode->theGameState->curMPProgressionPercentage >= GameMode->theGameState->catWinProgressionPercentage);
        if (hasReachedTarget)
        {
            isGameEnd = true;
            winningTeam = TEXT("Cat");
        }
    }

    if (!isGameEnd && GameMode->theGameState->totalCatPlayers > 0)
    {
        bool hasCaughtAllCats = (GameMode->theGameState->caughtCats >= GameMode->theGameState->totalCatPlayers) ||
                                (GameMode->theGameState->caughtCatsPercentage >= 0.999f);
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

    if (!isGameEnd && GameMode->theGameState->curGameplayTime == 0)
    {
        if (GameMode->theGameState->curMPProgressionPercentage > GameMode->theGameState->caughtCatsPercentage)
        {
            isGameEnd = true;
            winningTeam = TEXT("Cat (Time Out)");
        }
        else if (GameMode->theGameState->caughtCatsPercentage > GameMode->theGameState->curMPProgressionPercentage)
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
        UMPLogManager::LogInfo(FString::Printf(TEXT("=== GAME END: %s Team Wins! ==="), *winningTeam), TEXT("MatchManager"));
        EndGameplayTime();
        return true;
    }

    return false;
}

void UMatchManager::EndGameplayTime()
{
    if (!GameMode || !GameMode->theGameState) return;

    RemoveGameplayHUD();

    FString resultKey;
    float humanProgress = 0.0f;
    float catProgress = 0.0f;
    
    bool catWin = false;
    bool humanWin = false;
    if (GameMode->theGameState->curMPProgressionPercentage >= GameMode->theGameState->catWinProgressionPercentage)
        catWin = true;
    if (GameMode->theGameState->caughtCatsPercentage >= 0.999f || GameMode->theGameState->caughtCats >= GameMode->theGameState->totalCatPlayers)
        humanWin = true;
    if (!catWin && !humanWin && GameMode->theGameState->curGameplayTime == 0)
    {
        if (GameMode->theGameState->curMPProgressionPercentage > GameMode->theGameState->caughtCatsPercentage)
            catWin = true;
        else
            humanWin = true;
    }
    humanProgress = GameMode->theGameState->caughtCatsPercentage;
    catProgress = GameMode->theGameState->curMPProgressionPercentage;

    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        if (eachPlayer)
        {
            eachPlayer->AttachHUD(EHUDType::EEnd, 0);
            UHUDEnd* endHUD = eachPlayer->endHUD;
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

    UMPLogManager::LogInfo(TEXT("Game ended. Players can manually restart or wait for auto-restart."), TEXT("MatchManager"));
}

void UMatchManager::RemoveGameplayHUD()
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

void UMatchManager::RegisterPlayerDeath(AMPControllerPlayer* diedPlayer, FVector diedPlayerLocation, FRotator diedPlayerRotation)
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

void UMatchManager::DisplayProgressionStatus()
{
    if (!GameMode || !GameMode->theGameState) return;

    UMPLogManager::LogInfo(TEXT("=== Game Progression Status ==="), TEXT("MatchManager"));
    UMPLogManager::LogInfo(FString::Printf(TEXT("Cat Team: %f/%f (%.1f%%) - Required: %.1f%%"),
        GameMode->theGameState->curMPProgression, GameMode->theGameState->totalMPProgression,
        GameMode->theGameState->curMPProgressionPercentage * 100.0f,
        GameMode->theGameState->catWinProgressionPercentage * 100.0f), TEXT("MatchManager"));
    UMPLogManager::LogInfo(FString::Printf(TEXT("Human Team: %d/%d (%.1f%%)"),
        GameMode->theGameState->caughtCats, GameMode->theGameState->totalCatPlayers,
        GameMode->theGameState->caughtCatsPercentage * 100.0f), TEXT("MatchManager"));
    UMPLogManager::LogInfo(FString::Printf(TEXT("Time Remaining: %d seconds"), GameMode->theGameState->curGameplayTime), TEXT("MatchManager"));
    UMPLogManager::LogInfo(TEXT("================================"), TEXT("MatchManager"));
} 