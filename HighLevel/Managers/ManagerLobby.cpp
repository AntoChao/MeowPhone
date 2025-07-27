#include "../Managers/ManagerLobby.h"
#include "TimerManager.h"

#include "../MPGMGameplay.h"
#include "../Managers/ManagerLog.h"
#include "../MPGS.h"
#include "../../MPActor/Player/Widget/HUDManagerLobby.h"
#include "../../MPActor/Player/Widget/HUDLobby.h"
#include "../../MPActor/Player/MPControllerPlayer.h"
#include "../../MPActor/Player/MPPlayerState.h"

void UManagerLobby::StartLobby()
{
    if (!GameMode || !GameMode->GetGameState()) return;
    GameMode->GetGameState()->curGameplayStatus = EGPStatus::ELobby;

    // Log debug mode status
    if (GameMode->GetSinglePlayerDebugMode() || GameMode->GetMultiplayerDebugMode())
    {
        UManagerLog::LogInfo(FString::Printf(TEXT("Debug mode status: %s"), *GameMode->GetDebugModeStatus()), TEXT("MPGMGameplay"));
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

bool UManagerLobby::CheckReadyToStartGame() const
{
    if (GameMode->GetSinglePlayerDebugMode())
    {
        return CheckAtLeastOnePlayerReady();
    }
    else if (GameMode->GetMultiplayerDebugMode())
    {
        return CheckAtLeastOnePlayerReady();
    }
    else
    {
        return CheckBothTeamHasPlayers() && CheckHalfPlayersAreReady();
    }
}

bool UManagerLobby::CheckAtLeastOnePlayerReady() const
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

bool UManagerLobby::CheckBothTeamHasPlayers() const
{
    if (GameMode->GetSinglePlayerDebugMode() || GameMode->GetMultiplayerDebugMode())
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

bool UManagerLobby::CheckHalfPlayersAreReady() const
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

bool UManagerLobby::AssignPlayerToTeam(AMPControllerPlayer* player, ETeam team)
{
    if (!player) return false;
    AMPPlayerState* playerState = Cast<AMPPlayerState>(player->PlayerState);
    if (!playerState) return false;
    int currentTeamCount = GetTeamPlayerCount(team);
    int otherTeamCount = GetTeamPlayerCount(team == ETeam::EHuman ? ETeam::ECat : ETeam::EHuman);
    if (currentTeamCount <= otherTeamCount + 1)
    {
        playerState->playerTeam = team;
        UManagerLog::LogInfo(FString::Printf(TEXT("Player %s assigned to %s team"), *playerState->playerName, team == ETeam::EHuman ? TEXT("Human") : TEXT("Cat")), TEXT("MPGMGameplay"));
        return true;
    }
    UManagerLog::LogWarning(FString::Printf(TEXT("Cannot assign player to %s team - teams would be unbalanced"), team == ETeam::EHuman ? TEXT("Human") : TEXT("Cat")), TEXT("MPGMGameplay"));
    return false;
}

bool UManagerLobby::SwitchPlayerTeam(AMPControllerPlayer* player, ETeam newTeam)
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
        UManagerLog::LogInfo(FString::Printf(TEXT("Player %s switched from %s to %s team"), *playerState->playerName, currentTeam == ETeam::EHuman ? TEXT("Human") : TEXT("Cat"), newTeam == ETeam::EHuman ? TEXT("Human") : TEXT("Cat")), TEXT("MPGMGameplay"));
        return true;
    }
    UManagerLog::LogWarning(FString::Printf(TEXT("Cannot switch player to %s team - would unbalance teams"), newTeam == ETeam::EHuman ? TEXT("Human") : TEXT("Cat")), TEXT("MPGMGameplay"));
    return false;
}

void UManagerLobby::AutoAssignTeams()
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
    UManagerLog::LogInfo(FString::Printf(TEXT("Auto-assigned %d players to teams"), unassignedPlayers.Num()), TEXT("MPGMGameplay"));
}

int UManagerLobby::GetTeamPlayerCount(ETeam team) const
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

void UManagerLobby::CountdownReadyGame()
{
    if (!CheckReadyToStartGame())
    {
        GameMode->GetGameState()->isMostPlayerReady = false;
        if (GameMode->GetWorld())
        {
            GameMode->GetWorld()->GetTimerManager().ClearTimer(GameMode->readyTimerHandle);
        }
        UManagerLog::LogInfo(TEXT("Lobby: Countdown CANCELLED - conditions no longer met"), TEXT("MPGMGameplay"));
        return;
    }
    if (GameMode->GetGameState()->curReadyTime > 0)
    {
        UWorld* serverWorld = GameMode->GetWorld();
        if (serverWorld)
        {
            GameMode->GetGameState()->curReadyTime -= 1;
            ClientUpdateReadyCountdown(GameMode->GetGameState()->curReadyTime);
            UManagerLog::LogInfo(FString::Printf(TEXT("Lobby: Countdown %d seconds remaining"), GameMode->GetGameState()->curReadyTime), TEXT("MPGMGameplay"));
            serverWorld->GetTimerManager().ClearTimer(GameMode->readyTimerHandle);
            FTimerDelegate readyTimerDel;
            readyTimerDel.BindUFunction(GameMode, FName("CountdownReadyGame"));
            serverWorld->GetTimerManager().SetTimer(GameMode->readyTimerHandle, readyTimerDel, 1, false);
        }
    }
    else
    {
        UManagerLog::LogInfo(TEXT("Lobby: Countdown FINISHED - starting game"), TEXT("MPGMGameplay"));
        EndReadyTime();
    }
}

void UManagerLobby::EndReadyTime()
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

bool UManagerLobby::SetPlayerReady(AMPControllerPlayer* player, bool isReady)
{
    if (!player)
    {
        UManagerLog::LogError(TEXT("SetPlayerReadyState called with null player"), TEXT("MPGMGameplay"));
        return false;
    }
    AMPPlayerState* playerState = Cast<AMPPlayerState>(player->PlayerState);
    if (!playerState)
    {
        UManagerLog::LogError(TEXT("Player state is null"), TEXT("MPGMGameplay"));
        return false;
    }
    playerState->isPlayerReady = isReady;
    UManagerLog::LogInfo(FString::Printf(TEXT("Player %s ready state set to: %s"), *playerState->playerName, isReady ? TEXT("Ready") : TEXT("Not Ready")), TEXT("MPGMGameplay"));
    if (isReady && CheckReadyToStartGame())
    {
        UManagerLog::LogInfo(TEXT("All requirements met! Starting game..."), TEXT("MPGMGameplay"));
        CountdownReadyGame();
    }
    BroadcastPlayerListUpdate();
    return true;
}

void UManagerLobby::BroadcastPlayerListUpdate()
{
    UManagerLog::LogDebug(TEXT("Broadcasting player list update to all clients"), TEXT("MPGMGameplay"));
    ClientUpdateLobbyHUDs();
}

void UManagerLobby::ClientUpdateLobbyHUDs_Implementation()
{
    UManagerLog::LogDebug(TEXT("Client received lobby update request"), TEXT("MPGMGameplay"));
    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        if (eachPlayer && eachPlayer->GetManagerLobbyHUD() && eachPlayer->GetManagerLobbyHUD()->lobbyHUD)
        {
            eachPlayer->GetManagerLobbyHUD()->lobbyHUD->UpdatePlayerLists();
        }
        else if (eachPlayer && !eachPlayer->GetManagerLobbyHUD())
        {
            UManagerLog::LogWarning(FString::Printf(TEXT("Player %s has no lobby manager HUD for update"), *eachPlayer->GetName()), TEXT("MPGMGameplay"));
        }
        else if (eachPlayer && eachPlayer->GetManagerLobbyHUD() && !eachPlayer->GetManagerLobbyHUD()->lobbyHUD)
        {
            UManagerLog::LogWarning(FString::Printf(TEXT("Player %s lobby manager has no lobby HUD for update"), *eachPlayer->GetName()), TEXT("MPGMGameplay"));
        }
    }
} 

// Countdown update RPCs
void UManagerLobby::ClientUpdateReadyCountdown_Implementation(int32 secondsRemaining)
{
    for (AMPControllerPlayer* eachPlayer : GameMode->allPlayersControllers)
    {
        if (eachPlayer)
        {
            eachPlayer->UpdateLobbyHUDCountdownText(secondsRemaining);
        }
    }
}