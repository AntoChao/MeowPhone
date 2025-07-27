#pragma once

// [Meow-Phone Project]
//
// This class represents a single row in the "Search Session" list. Its purpose is to
// display the information for one available multiplayer session, such as its name,
// player count, and a button to join it.
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_SessionEntry`). This will be the visual template for each server row.
// 2. In the UMG editor, you must create all the `TextBlock` and `Button` widgets and name them to match the `BindWidget` properties below (e.g., `sessionNameText`, `joinButton`).
// 3. You do not create this widget directly. The parent `UHUDSearchSession` widget creates instances of it. You must assign your `WBP_SessionEntry` Blueprint to the `Session Entry Class` property on the main `WBP_SearchSession` widget.
// 4. The `UHUDSearchSession` widget calls `InitializeEntry` on each instance it creates, passing in the session data to populate the text fields.
// 5. The join button's `OnClicked` event is bound in C++. It fires the `OnJoinSessionClicked` delegate, which the parent `UHUDSearchSession` widget listens to in order to handle the join request.
//
// Necessary things to define:
// - The child Widget Blueprint MUST have all the correctly named widgets to match the `BindWidget` properties.
//
// How it interacts with other classes:
// - UUserWidget: The base class.
// - UHUDSearchSession: The parent widget that creates, populates, and manages instances of this widget. It listens to the `OnJoinSessionClicked` delegate to know when the player wants to join this specific session.
// - FSessionInfo (Struct): Although not directly referenced, the data used to populate this widget (via `InitializeEntry`) is originally sourced from this struct in the `UMPGI`.

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../../CommonEnum.h"
#include "HUDSearchSessionEntry.generated.h"

UCLASS()
class MEOWPHONE_API UHUDSearchSessionEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	UHUDSearchSessionEntry(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	// Session data
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* sessionNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* hostNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* currentPlayersNumberText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* maxPlayersNumberText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* pingText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* joinButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* joinButtonText;

	// Session information
	UPROPERTY(BlueprintReadOnly)
	FString sessionName;

	UPROPERTY(BlueprintReadOnly)
	FString hostName;

	UPROPERTY(BlueprintReadOnly)
	int32 currentPlayersNumber;

	UPROPERTY(BlueprintReadOnly)
	int32 maxPlayersNumber;

	UPROPERTY(BlueprintReadOnly)
	int32 ping;

	UPROPERTY(BlueprintReadOnly)
	bool availableToJoin;

	UPROPERTY(BlueprintReadOnly)
	int32 sessionIndex;

	// Callback for join button
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoinSessionClicked, int32, sessionIndex);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnJoinSessionClicked OnJoinSessionClicked;

public:
	// Initialize the entry with session data
	UFUNCTION(BlueprintCallable, Category = "Session Entry")
	void InitializeEntry(const FString& inSessionName, const FString& inHostName, 
		int32 inCurrentPlayersNumber, int32 inMaxPlayersNumber, int32 inPing, bool inAvailableToJoin, int32 inSessionIndex);

	// Update availability
	UFUNCTION(BlueprintCallable, Category = "Session Entry")
	void UpdateAvailability(bool inAvailableToJoin);

	// Update ping
	UFUNCTION(BlueprintCallable, Category = "Session Entry")
	void UpdatePing(int32 inPing);

	// Update player count
	UFUNCTION(BlueprintCallable, Category = "Session Entry")
	void UpdatePlayerCount(int32 inCurrentPlayersNumber);

protected:
	// Button click handler
	UFUNCTION(BlueprintCallable, Category = "Session Entry")
	void OnJoinButtonClicked();

	// Update button state based on availability
	UFUNCTION(BlueprintCallable, Category = "Session Entry")
	void UpdateButtonState();
}; 