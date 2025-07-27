#pragma once

#include "CoreMinimal.h"
#include "ManagerMP.h"
#include "../../CommonEnum.h"
#include "ManagerLobby.generated.h"

class AMPGMGameplay;
class AMPControllerPlayer;

UCLASS()
class UManagerLobby : public UManagerMP
{
    GENERATED_BODY()

public:
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