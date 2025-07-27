#pragma once

// [Meow-Phone Project]
//
// This is the UI widget for the main game lobby. It is responsible for displaying the
// lists of players on the Human and Cat teams, and providing buttons for players to
// switch teams or for the host to add/remove bots.
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_Lobby`).
// 2. In the UMG editor, you must create all the UI elements and name them to match the `BindWidget` properties (e.g., `UScrollBox` named `humanPlayersScrollBox`, `UButton` named `humanJoinButton`).
// 3. In the Blueprint's defaults, you MUST set the `Lobby Entry Class` property. This requires you to first create another Widget Blueprint for a single player row (inheriting from `UHUDLobbyEntry`) and then assign that `WBP_LobbyEntry` class here.
// 4. This widget is managed by the `UHUDLobbyManager`. The manager is responsible for creating this widget and calling its `UpdatePlayerLists` function whenever the lobby state changes.
// 5. The button click events (`OnHumanJoinButtonClicked`, etc.) are bound in C++. They call functions on the `AMPControllerPlayer` to send requests to the server (e.g., to switch teams or add a bot).
//
// Necessary things to define:
// - All `BindWidget` properties must have corresponding widgets in the child Blueprint.
// - `lobbyEntryClass` MUST be assigned to a valid `UHUDLobbyEntry` child Blueprint.
//
// How it interacts with other classes:
// - UMPHUD: The base HUD class.
// - UHUDLobbyManager: The parent/manager widget that controls this one.
// - UHUDLobbyEntry: This widget dynamically creates instances of the lobby entry widget for each player in the game and adds them to its `ScrollBox` elements.
// - AMPPlayerState: The `UpdatePlayerLists` function gets the `PlayerArray` from the Game State, iterates through all the `AMPPlayerState` objects, and uses their data (name, team, ready status) to create and populate the `UHUDLobbyEntry` widgets.
// - AMPControllerPlayer: Button clicks on this UI result in calls to the Player Controller to send `ServerRequest...` RPCs to the server.

#include "MPHUD.h"
#include "HUDLobby.generated.h"

class UScrollBox;
class UButton;
class UTextBlock;
class UHUDLobbyEntry;

UCLASS()
class MEOWPHONE_API UHUDLobby : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDLobby();

protected:
    virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	// UI Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UScrollBox* humanPlayersScrollBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UScrollBox* catPlayersScrollBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* humanJoinButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* humanJoinButtonText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* catJoinButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* catJoinButtonText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* humanAddButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* humanAddButtonText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* countdownText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* catAddButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* catAddButtonText;

	// Localization keys
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString joinHumanTextKey = TEXT("JOIN_HUMAN");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString joinCatTextKey = TEXT("JOIN_CAT");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString addHumanTextKey = TEXT("ADD_HUMAN");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString addCatTextKey = TEXT("ADD_CAT");

	// Lobby entry class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby List")
	TSubclassOf<UHUDLobbyEntry> lobbyEntryClass;

	// Lobby state
	UPROPERTY(BlueprintReadOnly, Category = "Lobby State")
	bool isReady;

	UPROPERTY(BlueprintReadOnly, Category = "Lobby State")
	bool isHost;

	UPROPERTY(BlueprintReadOnly, Category = "Lobby State")
	ETeam currentTeam;

	// Player lists
	UPROPERTY(BlueprintReadOnly, Category = "Player Lists")
	TArray<UHUDLobbyEntry*> humanPlayersList;

	UPROPERTY(BlueprintReadOnly, Category = "Player Lists")
	TArray<UHUDLobbyEntry*> catPlayersList;

public:
	// Update player lists
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void UpdatePlayerLists();

	// Clear player lists
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void ClearPlayerLists();

	// Update ready state
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void UpdateReadyState(bool inIsReady);

	// Update host visibility
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void UpdateHostVisibility();

	// Update team-specific button visibility
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void UpdateTeamButtonVisibility();

	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void UpdateCountdownText(int32 secondsRemaining);
protected:
	// Button click handlers
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void OnHumanJoinButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void OnCatJoinButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void OnHumanAddButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void OnCatAddButtonClicked();

	// Entry event handlers
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void OnRemoveBotClicked(int32 playerIndex);


	// Create lobby entry widget
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	UHUDLobbyEntry* CreateLobbyEntry(const FString& playerName, bool isBot, bool isLobbyReady, 
		ETeam team, int32 playerIndex, class AMPControllerPlayer* playerController, 
		class AMPAIController* aiController);

public:
	// Get current team from player controller
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	ETeam GetCurrentTeam() const;

	// Check if current player is host
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	bool IsHost() const;

	// Update UI state
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void UpdateUI();

	// Delegate for team changes
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamChanged, ETeam, NewTeam);
	UPROPERTY(BlueprintAssignable, Category = "Lobby Events")
	FOnTeamChanged OnTeamChanged;
};