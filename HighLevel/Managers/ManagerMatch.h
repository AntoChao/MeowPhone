#pragma once

// [Meow-Phone Project]
//
// This manager is the primary state machine for the entire gameplay match. It controls the
// flow of the game, from the initial character customization phase, through the preparation
// period, into the main gameplay loop, and finally to the end-of-game condition checks.
//
// How to utilize in Blueprint:
// 1. This manager is created and owned by the `AMPGMGameplay` Game Mode. It should not be placed in the level manually.
// 2. The Game Mode is responsible for driving this manager. When the lobby completes, the Game Mode will call `StartCustomizeCharacter` to begin the match flow.
// 3. The functions in this manager are not typically called from Blueprints directly, as they are part of a very specific, sequential flow controlled by the Game Mode and internal timers.
// 4. The main interaction point for other systems is `RegisterPlayerDeath`, which should be called by a character or player controller when they die to inform the manager of this critical game event.
//
// Necessary things to define:
// - This manager is logic-driven and does not have properties that need to be configured in the Blueprint editor. Its behavior is determined by the C++ implementation of its state-flow functions.
//
// How it interacts with other classes:
// - UManagerMP: Inherits from the base manager class.
// - AMPGMGameplay: The Game Mode owns this manager and initiates the match sequence by calling its `Start...` functions. It is the sole driver of this manager.
// - Setup Functions (`SetupMap`, `SetupPlayers`, etc.): These internal functions are responsible for coordinating with various Factory and other Manager classes to populate the world with items, environments, and player pawns at the correct time.
// - AMPControllerPlayer: It receives notifications about player deaths via `RegisterPlayerDeath`.
// - TimerManager (`FTimerHandle`): It relies heavily on timers to manage the duration of each phase of the game (customization, preparation, gameplay).
// - HUDs: It is responsible for telling the HUDs when to appear and disappear, for example, calling `RemoveGameplayHUD` at the end of a match.

#include "CoreMinimal.h"
#include "ManagerMP.h"
#include "ManagerMatch.generated.h"

class AMPGMGameplay;
class AMPControllerPlayer;

UCLASS()
class UManagerMatch : public UManagerMP
{
    GENERATED_BODY()

protected:
    FTimerHandle customCharacterTimerHandle;
    FTimerHandle prepareTimerHandle;
    FTimerHandle gameplayTimerHandle;

    void SetupMap();
    void SetupMapItems();
    void SetupMapEnvActors();
    void SetupPlayers();
    void SetupAIs();

    void RemoveGameplayHUD();

public:
    // Character Customization
    void StartCustomizeCharacter();
    void CountdownCustomizeCharacter();
    void EndCustomizeCharacter();

    // Game Flow
    void StartGame();
    void SetupGame();
    void StartPrepareTime();
    void CountdownPrepareGame();
    void EndPrepareTime();
    void StartGameplayTime();
    void CountdownGameplayGame();
    bool CheckIfGameEnd();
    void EndGameplayTime();

    void RegisterPlayerDeath(AMPControllerPlayer* diedPlayer, FVector diedPlayerLocation, FRotator diedPlayerRotation);
    void DisplayProgressionStatus();
}; 