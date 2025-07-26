#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CommonEnum.h"
#include "LobbyManager.generated.h"

class AMPGMGameplay;
class AMPControllerPlayer;

UCLASS()
class ULobbyManager : public UObject
{
    GENERATED_BODY()

public:
    void Initialize(AMPGMGameplay* InGameMode);

    void StartLobby();
    bool SetPlayerReady(AMPControllerPlayer* Player, bool bReady);
    bool CheckReadyToStartGame() const;
    bool CheckAtLeastOnePlayerReady() const;
    bool CheckBothTeamHasPlayers() const;
    bool CheckHalfPlayersAreReady() const;
    bool AssignPlayerToTeam(AMPControllerPlayer* player, ETeam team);
    bool SwitchPlayerTeam(AMPControllerPlayer* player, ETeam newTeam);
    void AutoAssignTeams();
    int GetTeamPlayerCount(ETeam team) const;
    void CountdownReadyGame();
    void EndReadyTime();
    void BroadcastPlayerListUpdate();

    UFUNCTION(Client, Reliable)
    void ClientUpdateLobbyHUDs();

    UFUNCTION(Client, Reliable)
    void ClientUpdateReadyCountdown(int32 secondsRemaining);

private:
    UPROPERTY()
    AMPGMGameplay* GameMode;
}; 