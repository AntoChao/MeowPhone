#include "Managers/LobbyManager.h"
#include "HighLevel/MPGMGameplay.h"
#include "MPActor/Player/MPControllerPlayer.h"
#include "MPActor/Player/MPPlayerState.h"
#include "HighLevel/MPLogManager.h"
#include "MPGS.h"
#include "TimerManager.h"

void ULobbyManager::Initialize(AMPGMGameplay* InGameMode)
{
    GameMode = InGameMode;
}

void ULobbyManager::StartLobby()
{
    if (!GameMode || !GameMode->theGameState) return;
    GameMode->theGameState->curGameplayStatus = EGPStatus::ELobby;

    // Log debug mode status
    if (GameMode->singlePlayerDebugMode || GameMode->multiplayerDebugMode)
    {
        UMPLogManager::LogInfo(FString::Printf(TEXT("Debug mode status: %s"), *GameMode->GetDebugModeStatus()), TEXT("MPGMGameplay"));
    }

    // Auto-assign teams for any unassigned players
    AutoAssignTeams();

    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        if (eachPlayer)
        {
            eachPlayer->AttachHUD(EHUDType::ELobby, 0);
            eachPlayer->LobbyStartUpdate();
        }
    }
}

bool ULobbyManager::CheckReadyToStartGame() const
{
    if (GameMode->singlePlayerDebugMode)
    {
        return CheckAtLeastOnePlayerReady();
    }
    else if (GameMode->multiplayerDebugMode)
    {
        return CheckAtLeastOnePlayerReady();
    }
    else
    {
        return CheckBothTeamHasPlayers() && CheckHalfPlayersAreReady();
    }
}

bool ULobbyManager::CheckAtLeastOnePlayerReady() const
{
    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        AMPPlayerState* eachPlayerState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
        if (eachPlayerState && eachPlayerState->isPlayerReady)
        {
            return true;
        }
    }
    return false;
}

bool ULobbyManager::CheckBothTeamHasPlayers() const
{
    if (GameMode->singlePlayerDebugMode || GameMode->multiplayerDebugMode)
    {
        return true;
    }
    int humanPlayers = 0;
    int catPlayers = 0;
    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
        if (eachState)
        {
            switch(eachState->playerTeam)
            {
                case ETeam::EHuman: humanPlayers++; break;
                case ETeam::ECat: catPlayers++; break;
                default: break;
            }
        }
    }
    return humanPlayers > 0 && catPlayers > 0;
}

bool ULobbyManager::CheckHalfPlayersAreReady() const
{
    int numReadyPlayers = 0;
    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        AMPPlayerState* eachPlayerState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
        if (eachPlayerState && eachPlayerState->isPlayerReady)
        {
            numReadyPlayers++;
        }
    }
    int halfPlayers = FMath::Max(1, GameMode->allPlayersControllers.Num() / 2);
    return numReadyPlayers >= halfPlayers;
}

bool ULobbyManager::AssignPlayerToTeam(AMPControllerPlayer* player, ETeam team)
{
    if (!player) return false;
    AMPPlayerState* playerState = Cast<AMPPlayerState>(player->PlayerState);
    if (!playerState) return false;
    int currentTeamCount = GetTeamPlayerCount(team);
    int otherTeamCount = GetTeamPlayerCount(team == ETeam::EHuman ? ETeam::ECat : ETeam::EHuman);
    if (currentTeamCount <= otherTeamCount + 1)
    {
        playerState->playerTeam = team;
        UMPLogManager::LogInfo(FString::Printf(TEXT("Player %s assigned to %s team"), *playerState->playerName, team == ETeam::EHuman ? TEXT("Human") : TEXT("Cat")), TEXT("MPGMGameplay"));
        return true;
    }
    UMPLogManager::LogWarning(FString::Printf(TEXT("Cannot assign player to %s team - teams would be unbalanced"), team == ETeam::EHuman ? TEXT("Human") : TEXT("Cat")), TEXT("MPGMGameplay"));
    return false;
}

bool ULobbyManager::SwitchPlayerTeam(AMPControllerPlayer* player, ETeam newTeam)
{
    if (!player) return false;
    AMPPlayerState* playerState = Cast<AMPPlayerState>(player->PlayerState);
    if (!playerState) return false;
    ETeam currentTeam = playerState->playerTeam;
    if (currentTeam == newTeam) return false;
    int currentTeamCount = GetTeamPlayerCount(currentTeam);
    int newTeamCount = GetTeamPlayerCount(newTeam);
    if (newTeamCount < currentTeamCount || newTeamCount <= currentTeamCount + 1)
    {
        playerState->playerTeam = newTeam;
        UMPLogManager::LogInfo(FString::Printf(TEXT("Player %s switched from %s to %s team"), *playerState->playerName, currentTeam == ETeam::EHuman ? TEXT("Human") : TEXT("Cat"), newTeam == ETeam::EHuman ? TEXT("Human") : TEXT("Cat")), TEXT("MPGMGameplay"));
        return true;
    }
    UMPLogManager::LogWarning(FString::Printf(TEXT("Cannot switch player to %s team - would unbalance teams"), newTeam == ETeam::EHuman ? TEXT("Human") : TEXT("Cat")), TEXT("MPGMGameplay"));
    return false;
}

void ULobbyManager::AutoAssignTeams()
{
    TArray<AMPControllerPlayer*> unassignedPlayers;
    for (AMPControllerPlayer* player : GameMode->allPlayersControllers)
    {
        AMPPlayerState* playerState = Cast<AMPPlayerState>(player->PlayerState);
        if (playerState && playerState->playerTeam == ETeam::ENone)
        {
            unassignedPlayers.Add(player);
        }
    }
    for (AMPControllerPlayer* player : unassignedPlayers)
    {
        int humanCount = GetTeamPlayerCount(ETeam::EHuman);
        int catCount = GetTeamPlayerCount(ETeam::ECat);
        ETeam targetTeam = (humanCount <= catCount) ? ETeam::EHuman : ETeam::ECat;
        AssignPlayerToTeam(player, targetTeam);
    }
    UMPLogManager::LogInfo(FString::Printf(TEXT("Auto-assigned %d players to teams"), unassignedPlayers.Num()), TEXT("MPGMGameplay"));
}

int ULobbyManager::GetTeamPlayerCount(ETeam team) const
{
    int count = 0;
    for (AMPControllerPlayer* player : GameMode->allPlayersControllers)
    {
        AMPPlayerState* playerState = Cast<AMPPlayerState>(player->PlayerState);
        if (playerState && playerState->playerTeam == team)
        {
            count++;
        }
    }
    return count;
}

void ULobbyManager::CountdownReadyGame()
{
    if (!CheckReadyToStartGame())
    {
        GameMode->theGameState->isMostPlayerReady = false;
        if (GameMode->GetWorld())
        {
            GameMode->GetWorld()->GetTimerManager().ClearTimer(GameMode->readyTimerHandle);
        }
        UMPLogManager::LogInfo(TEXT("Lobby: Countdown CANCELLED - conditions no longer met"), TEXT("MPGMGameplay"));
        return;
    }
    if (GameMode->theGameState->curReadyTime > 0)
    {
        UWorld* serverWorld = GameMode->GetWorld();
        if (serverWorld)
        {
            GameMode->theGameState->curReadyTime -= 1;
            GameMode->ClientUpdateReadyCountdown(GameMode->theGameState->curReadyTime);
            UMPLogManager::LogInfo(FString::Printf(TEXT("Lobby: Countdown %d seconds remaining"), GameMode->theGameState->curReadyTime), TEXT("MPGMGameplay"));
            serverWorld->GetTimerManager().ClearTimer(GameMode->readyTimerHandle);
            FTimerDelegate readyTimerDel;
            readyTimerDel.BindUFunction(GameMode, FName("CountdownReadyGame"));
            serverWorld->GetTimerManager().SetTimer(GameMode->readyTimerHandle, readyTimerDel, 1, false);
        }
    }
    else
    {
        UMPLogManager::LogInfo(TEXT("Lobby: Countdown FINISHED - starting game"), TEXT("MPGMGameplay"));
        EndReadyTime();
    }
}

void ULobbyManager::EndReadyTime()
{
    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        if (eachPlayer)
        {
            eachPlayer->RemoveHUD(EHUDType::ELobby);
        }
    }
    GameMode->StartCustomizeCharacter();
}

bool ULobbyManager::SetPlayerReady(AMPControllerPlayer* player, bool isReady)
{
    if (!player)
    {
        UMPLogManager::LogError(TEXT("SetPlayerReadyState called with null player"), TEXT("MPGMGameplay"));
        return false;
    }
    AMPPlayerState* playerState = Cast<AMPPlayerState>(player->PlayerState);
    if (!playerState)
    {
        UMPLogManager::LogError(TEXT("Player state is null"), TEXT("MPGMGameplay"));
        return false;
    }
    playerState->isPlayerReady = isReady;
    UMPLogManager::LogInfo(FString::Printf(TEXT("Player %s ready state set to: %s"), *playerState->playerName, isReady ? TEXT("Ready") : TEXT("Not Ready")), TEXT("MPGMGameplay"));
    if (isReady && CheckReadyToStartGame())
    {
        UMPLogManager::LogInfo(TEXT("All requirements met! Starting game..."), TEXT("MPGMGameplay"));
        CountdownReadyGame();
    }
    BroadcastPlayerListUpdate();
    return true;
}

void ULobbyManager::BroadcastPlayerListUpdate()
{
    UMPLogManager::LogDebug(TEXT("Broadcasting player list update to all clients"), TEXT("MPGMGameplay"));
    GameMode->ClientUpdateLobbyHUDs();
}

void ULobbyManager::ClientUpdateLobbyHUDs_Implementation()
{
    UMPLogManager::LogDebug(TEXT("Client received lobby update request"), TEXT("MPGMGameplay"));
    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        if (eachPlayer && eachPlayer->lobbyManagerHUD && eachPlayer->lobbyManagerHUD->lobbyHUD)
        {
            eachPlayer->lobbyManagerHUD->lobbyHUD->UpdatePlayerLists();
        }
        else if (eachPlayer && !eachPlayer->lobbyManagerHUD)
        {
            UMPLogManager::LogWarning(FString::Printf(TEXT("Player %s has no lobby manager HUD for update"), *eachPlayer->GetName()), TEXT("MPGMGameplay"));
        }
        else if (eachPlayer && eachPlayer->lobbyManagerHUD && !eachPlayer->lobbyManagerHUD->lobbyHUD)
        {
            UMPLogManager::LogWarning(FString::Printf(TEXT("Player %s lobby manager has no lobby HUD for update"), *eachPlayer->GetName()), TEXT("MPGMGameplay"));
        }
    }
} 