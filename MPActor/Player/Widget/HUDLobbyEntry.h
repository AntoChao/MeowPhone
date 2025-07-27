#pragma once

// [Meow-Phone Project]
//
// This class represents a single player's entry or row in the lobby UI. Its job is to
// display the name and ready status of one player. For bots, it also provides a "remove"
// button that is only visible to the host.
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_LobbyEntry`). This will be the visual template for each player row.
// 2. In the UMG editor, you must create `TextBlock` widgets named `playerNameText` and `readyStatusText`, and a `Button` named `removeButton`.
// 3. You do not create this widget yourself. The main `UHUDLobby` widget creates instances of this class. You must assign your `WBP_LobbyEntry` Blueprint to the `Lobby Entry Class` property on the `WBP_Lobby` widget.
// 4. The `UHUDLobby` widget calls `InitializeEntry` on each instance it creates, passing in all the necessary data (name, team, etc.) to populate the row.
// 5. The remove button's `OnClicked` event is bound in C++. It fires the `OnRemoveBotClicked` delegate, which the parent `UHUDLobby` widget listens to in order to handle the remove request.
//
// Necessary things to define:
// - The child Widget Blueprint must have correctly named widgets for `playerNameText`, `readyStatusText`, and `removeButton`.
//
// How it interacts with other classes:
// - UUserWidget: The base class.
// - UHUDLobby: The parent widget that creates, populates, and manages instances of this widget. It also listens to the `OnRemoveBotClicked` delegate.
// - Player Data: This widget is purely data-driven. It holds variables like `playerName` and `isReady` but does not fetch them itself; they are pushed into it by the parent `UHUDLobby` widget via the `InitializeEntry` function.

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../../CommonEnum.h"
#include "HUDLobbyEntry.generated.h"

class AMPControllerPlayer;
class AMPAIController;

UCLASS()
class MEOWPHONE_API UHUDLobbyEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	UHUDLobbyEntry(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	// UI Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* playerNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* readyStatusText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* removeButton;

	// Player data
	UPROPERTY(BlueprintReadOnly, Category = "Player Data")
	FString playerName;

	UPROPERTY(BlueprintReadOnly, Category = "Player Data")
	bool isBot;

	UPROPERTY(BlueprintReadOnly, Category = "Player Data")
	bool isReady;

	UPROPERTY(BlueprintReadOnly, Category = "Player Data")
	ETeam playerTeam;

	UPROPERTY(BlueprintReadOnly, Category = "Player Data")
	int32 playerIndex;

	UPROPERTY(BlueprintReadOnly, Category = "Player Data")
	AMPControllerPlayer* playerController;

	UPROPERTY(BlueprintReadOnly, Category = "Player Data")
	AMPAIController* aiController;

	// Localization keys
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString readyTextKey = TEXT("READY");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString notReadyTextKey = TEXT("NOT_READY");

	// Callback for remove button
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoveBotClicked, int32, playerIndex);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRemoveBotClicked OnRemoveBotClicked;

public:
	// Initialize the entry with player data
	UFUNCTION(BlueprintCallable, Category = "Lobby Entry")
	void InitializeEntry(const FString& inPlayerName, bool inIsBot, bool inIsReady, 
		ETeam inPlayerTeam, int32 inPlayerIndex, AMPControllerPlayer* inPlayerController, 
		AMPAIController* inAIController);

	// Update ready status
	UFUNCTION(BlueprintCallable, Category = "Lobby Entry")
	void UpdateReadyStatus(bool inIsReady);

	// Update player name
	UFUNCTION(BlueprintCallable, Category = "Lobby Entry")
	void UpdatePlayerName(const FString& inPlayerName);

	// Update host visibility (show/hide remove button for bots)
	UFUNCTION(BlueprintCallable, Category = "Lobby Entry")
	void UpdateHostVisibility(bool isHost);

protected:
	// Button click handler
	UFUNCTION(BlueprintCallable, Category = "Lobby Entry")
	void OnRemoveButtonClicked();

	// Update UI based on current data
	UFUNCTION(BlueprintCallable, Category = "Lobby Entry")
	void UpdateUI();
}; 