#pragma once

// [Meow-Phone Project]
//
// This class is a high-level "manager" widget that controls the entire lobby and
// character customization UI. It acts as a container that holds and orchestrates
// several child widgets: the main lobby, the cat customization screen, and the human
// customization screen.
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_LobbyManager`). This is the top-level widget for the lobby scene.
// 2. In the UMG editor, you must create the basic layout, including `Overlay` widgets named `lobbyOverlay` and `customizationOverlay` to serve as containers for the child widgets. You also need the shared buttons like `backButton` and `readyButton`.
// 3. In the Blueprint's defaults, you MUST assign the `TSubclassOf` properties:
//    - `lobbyHUDClass`: Set this to your `WBP_Lobby` Blueprint.
//    - `customCatHUDClass`: Set this to your `WBP_CustomCat` Blueprint.
//    - `customHumanHUDClass`: Set this to your `WBP_CustomHuman` Blueprint.
// 4. When this manager widget is created, its C++ `NativeConstruct` calls `CreateHUDWidgets`. This function creates instances of the three child widgets you specified and places them into the appropriate `Overlay` containers.
// 5. This manager handles the overall state. For example, when a player joins a team, `OnTeamChanged` is called, which then calls `ShowCustomizationHUD` to make the correct customization panel visible.
//
// Necessary things to define:
// - All `BindWidget` properties must have corresponding widgets in the child Blueprint.
// - All three `...HUDClass` properties must be assigned to their respective child Widget Blueprints.
//
// How it interacts with other classes:
// - UMPHUD: The base HUD class.
// - AMPControllerPlayer: Creates this manager widget. This widget also calls functions on the controller to send requests to the server (e.g., `OnReadyButtonClicked` tells the controller to send a `ServerSetReadyState` RPC).
// - UHUDLobby, UHUDCustomCat, UHUDCustomHuman: This class creates, owns, and manages the visibility of these three child widgets. It acts as the central coordinator for the entire pre-game UI.
// - AMPPlayerState: It reads the local player's team and ready status from the Player State to correctly configure its own state and UI.

#include "MPHUD.h"
#include "HUDLobbyManager.generated.h"

class UCanvasPanel;
class UOverlay;
class UPanelWidget;
class UButton;
class UTextBlock;
class UHUDLobby;
class UHUDCustomCat;
class UHUDCustomHuman;

UCLASS()
class MEOWPHONE_API UHUDManagerLobby : public UMPHUD
{
	GENERATED_BODY()

public:
	UHUDManagerLobby();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	// UI Components - Main Layout
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCanvasPanel* mainCanvas;

	// Left Side - Lobby Area
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UOverlay* lobbyOverlay;

	// Right Side - Customization Area
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UOverlay* customizationOverlay;

	// Bottom - Shared Controls
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UPanelWidget* sharedControlPanel;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* backButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* backButtonText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* readyButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* readyButtonText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* countdownText;

	// HUD References
	UPROPERTY(BlueprintReadOnly, Category = "HUD References")
	UHUDLobby* lobbyHUD;

	UPROPERTY(BlueprintReadOnly, Category = "HUD References")
	UHUDCustomCat* customCatHUD;

	UPROPERTY(BlueprintReadOnly, Category = "HUD References")
	UHUDCustomHuman* customHumanHUD;

	// HUD Classes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Classes")
	TSubclassOf<UHUDLobby> lobbyHUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Classes")
	TSubclassOf<UHUDCustomCat> customCatHUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Classes")
	TSubclassOf<UHUDCustomHuman> customHumanHUDClass;

	// Localization keys
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString backTextKey = TEXT("BACK");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString readyTextKey = TEXT("READY");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString notReadyTextKey = TEXT("NOT_READY");

	// State
	UPROPERTY(BlueprintReadOnly, Category = "Manager State")
	bool isReady;

	UPROPERTY(BlueprintReadOnly, Category = "Manager State")
	bool isHost;

	UPROPERTY(BlueprintReadOnly, Category = "Manager State")
	ETeam currentTeam;

	UPROPERTY(BlueprintReadOnly, Category = "Manager State")
	bool isCustomizationActive;

public:
	// Initialize the manager
	UFUNCTION(BlueprintCallable, Category = "Manager")
	void InitializeManager();

	// Show/hide customization HUDs
	UFUNCTION(BlueprintCallable, Category = "Manager")
	void ShowCustomizationHUD(ETeam team);

	UFUNCTION(BlueprintCallable, Category = "Manager")
	void HideCustomizationHUD();

	// Update shared controls
	UFUNCTION(BlueprintCallable, Category = "Manager")
	void UpdateReadyState(bool inIsReady);

	UFUNCTION(BlueprintCallable, Category = "Manager")
	void UpdateReadyButtonText();

	UFUNCTION(BlueprintCallable, Category = "Manager")
	void UpdateCountdownText(int32 secondsRemaining);

	UFUNCTION(BlueprintCallable, Category = "Manager")
	void SetReadyButtonEnabled(bool enabled);

	// Get current team from player controller
	UFUNCTION(BlueprintCallable, Category = "Manager")
	ETeam GetCurrentTeam() const;

	// Refresh current team from owner
	UFUNCTION(BlueprintCallable, Category = "Manager")
	void RefreshCurrentTeam();

	// Check if current player is host
	UFUNCTION(BlueprintCallable, Category = "Manager")
	bool IsHost() const;

protected:
	// Button click handlers
	UFUNCTION(BlueprintCallable, Category = "Manager")
	void OnBackButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Manager")
	void OnReadyButtonClicked();

	// Create HUD widgets
	UFUNCTION(BlueprintCallable, Category = "Manager")
	void CreateHUDWidgets();

	// Find UI components
	void FindUIComponents();

	// Validate that all required UI components are found
	UFUNCTION(BlueprintCallable, Category = "Manager")
	bool ValidateUIComponents() const;

	// Bind button events
	void BindButtonEvents();

public:
	virtual void SetOwner(AMPControllerPlayer* theOwner) override;

	// Handle team changes from lobby
	UFUNCTION(BlueprintCallable, Category = "Manager")
	void OnTeamChanged(ETeam newTeam);

	// Update UI state
	UFUNCTION(BlueprintCallable, Category = "Manager")
	void UpdateUI();

}; 