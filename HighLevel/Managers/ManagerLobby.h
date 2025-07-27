#pragma once

// [Meow-Phone Project]
//
// This manager handles all the core logic for the pre-game lobby. It is responsible for tracking
// player readiness, team assignments, and managing the countdown to start the match.
//
// How to utilize in Blueprint:
// 1. This manager is created and owned by the Game Mode (`AMPGMGameplay` or a specific Lobby Game Mode). It is not placed in the world.
// 2. The Lobby UI (`HUDLobby`) interacts heavily with this manager. For example, when a player clicks the "Ready" button, the UI will get the `UManagerLobby` instance from the Game Mode and call `SetPlayerReady`.
// 3. Functions like `AssignPlayerToTeam` and `SwitchPlayerTeam` are called from the UI to allow players to change teams.
// 4. `AutoAssignTeams` would be called by the UI or game logic if there's a button to balance or randomize teams.
// 5. The manager's functions are generally not called directly from level Blueprints, but rather through the Game Mode and driven by UI events.
//
// Necessary things to define:
// - This manager is primarily logic-driven and does not require asset properties to be set in the Blueprint editor. Its state is managed internally based on function calls from the UI and player controllers.
//
// How it interacts with other classes:
// - UManagerMP: Inherits from the base manager class.
// - AMPGMGameplay: The Game Mode owns this manager and is the main entry point for the UI to access it. The Game Mode will also call `StartLobby` when the lobby state begins and will initiate the match start when this manager determines everyone is ready.
// - AMPControllerPlayer: This manager directly manipulates player controllers to set their ready status and team affiliation. It also uses them as keys to identify players.
// - HUDLobby / Lobby UI: The UI is the primary driver of this manager's functions. It calls functions based on player input (clicking buttons) and listens for updates from the manager (like `ClientUpdateLobbyHUDs` and `ClientUpdateReadyCountdown`) to refresh the display.
// - ETeam (Enum): Used extensively to manage team assignments and player counts.

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
protected:
    FTimerHandle readyTimerHandle;
	FTimerHandle restartLobbyTimerHandle;

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

    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void ClearAllTimers();
}; 