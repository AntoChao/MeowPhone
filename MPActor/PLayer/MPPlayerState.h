#pragma once

// [Meow-Phone Project]
//
// This is the Player State class. While the Player Controller exists only on the owning client
// and the server, the Player State exists on the server and is replicated to *every* client.
// It is the correct place to store replicated information about a specific player's state that
// all other players need to know about, such as their name, team, and readiness status.
//
// How to utilize in Blueprint:
// 1. A Blueprint should be created from this class (`BP_PlayerState`) and set as the `PlayerState Class` in the Game Modes (`AMPGMInit`, `AMPGMGameplay`).
// 2. Other Blueprints (especially UI widgets like the lobby) can get the Player State for a specific player from the `PlayerArray` in the Game State.
// 3. UI elements bind to the properties on this class to display information. For example, a lobby entry widget would read `playerName`, `playerTeam`, and `isPlayerReady` from the Player State of the player it represents.
// 4. Writing to these properties should only be done on the server (typically by the Game Mode in response to a request from a Player Controller). The engine's replication system handles sending the updates to all clients.
//
// Necessary things to define:
// - A `BP_PlayerState` Blueprint should be created and assigned in the Game Mode, but it generally doesn't require any further configuration in the editor itself.
//
// How it interacts with other classes:
// - APlayerState: The base class from which it inherits its core replication behavior.
// - AMPGMGameplay (Game Mode): The Game Mode on the server is responsible for changing the values in a player's Player State (e.g., setting `playerTeam` when a team switch is approved).
// - AMPControllerPlayer: The Player Controller on the server has a direct link to its Player State.
// - AMPGS (Game State): The Game State holds the `PlayerArray`, which is the list of all Player States in the current match. This is how clients can find the Player State of other players.
// - OnRep_... functions: These RepNotify functions are called on clients when the associated variable is updated. This is used to trigger UI updates. For example, when `isPlayerReady` changes, `OnRep_IsPlayerReady` is called, which could cause a "Ready" icon to appear or disappear in the lobby UI.

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "../../CommonEnum.h"
#include "MPPlayerState.generated.h"

/*
enum class ETeam : uint8;
enum class ECatRace : uint8;
enum class EAbility : uint8;
enum class EHumanProfession : uint8;*/

UCLASS(BlueprintType, Blueprintable)
class AMPPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    AMPPlayerState();

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Common playerstate
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Common Properties")
    FString playerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Common Properties")
    int playerIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_PlayerTeam, Category = "Common Properties")
    ETeam playerTeam;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Common Properties")
    ECatRace catRace;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Ability Properties")
    EAbility playerSelectedAbility;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Common Properties")
    EHumanProfession humanProfession;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Common Properties")
    EHat playerSelectedHat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_IsPlayerReady, Category = "Common Properties")
    bool isPlayerReady = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_IsPlayerDied, Category = "Common Properties")
    bool isPlayerDied = false;

protected:
    // Replication callbacks
    UFUNCTION()
    void OnRep_PlayerTeam();
    
    UFUNCTION()
    void OnRep_IsPlayerReady();
    
    UFUNCTION()
    void OnRep_IsPlayerDied();
};