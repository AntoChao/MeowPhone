#pragma once

// [Meow-Phone Project]
//
// This class is the UI for the "Search Session" or "Join Game" screen. It acts as a server
// browser, allowing the player to refresh a list of available multiplayer sessions and join one.
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_SearchSession`).
// 2. In the UMG editor, you must create all the UI elements and name them to match the `BindWidget` properties (e.g., a `UScrollBox` named `sessionListScrollBox`).
// 3. In the Blueprint's defaults, you MUST set the `Session Entry Class`. This requires you to first create a separate widget for a single server row (inheriting from `UHUDSearchSessionEntry`) and then assign that Blueprint class here.
// 4. When this widget is shown, its `NativeConstruct` calls `StartSessionSearch`. This function gets the `UMPGI` (Game Instance) and tells it to begin searching for online sessions.
// 5. The Game Instance performs the search asynchronously. When the search is complete, the `UMPGI` calls the `OnSearchCompleted` function on this widget.
// 6. `OnSearchCompleted` then calls `UpdateSessionList`, which gets the search results from the Game Instance. It iterates through the results, creates an instance of your `sessionEntryClass` for each one, populates it with the session data, and adds it to the `sessionListScrollBox`.
//
// Necessary things to define:
// - All `BindWidget` properties must have corresponding widgets in the child Blueprint.
// - `sessionEntryClass` MUST be assigned to a valid `UHUDSearchSessionEntry` child Blueprint.
//
// How it interacts with other classes:
// - UMPHUD: The base HUD class.
// - UMPGI (Game Instance): This widget's primary interaction is with the Game Instance. It tells the GI to `SearchForSessions`, and the GI calls `OnSearchCompleted` back on this widget when the async search finishes. It also gets the results from the GI and tells the GI which session to `JoinSessions`.
// - UHUDSearchSessionEntry: This widget dynamically creates and populates instances of the session entry widget to build its server list.
// - UScrollBox: The UMG widget used to hold the list of server entries.

#include "CoreMinimal.h"
#include "MPHUD.h"
#include "HUDSearchSessionEntry.h"
#include "HUDSearchSession.generated.h"

UCLASS()
class MEOWPHONE_API UHUDSearchSession : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDSearchSession();

protected:
    virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void UpdateTexts() override;
	virtual bool ValidateRootWidget() override;

public:
	// UI Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* titleText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* backButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* backButtonText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* refreshButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* refreshButtonText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UScrollBox* sessionListScrollBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* searchingText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* noSessionsText;

	// Localization keys
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString titleTextKey = TEXT("SEARCH_SESSIONS_TITLE");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString backTextKey = TEXT("BACK");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString refreshTextKey = TEXT("REFRESH");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString searchingTextKey = TEXT("SEARCHING_SESSIONS");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString noSessionsTextKey = TEXT("NO_SESSIONS_FOUND");

	// Session list entry class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session List")
	TSubclassOf<UHUDSearchSessionEntry> sessionEntryClass;

	// Search state
	UPROPERTY(BlueprintReadOnly, Category = "Search State")
	bool isSearching;

	// Session entries
	UPROPERTY(BlueprintReadOnly, Category = "Session List")
	TArray<UHUDSearchSessionEntry*> sessionEntries;

public:
	// Start session search
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	void StartSessionSearch();

	// Stop session search
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	void StopSessionSearch();

	// Update session list with search results
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	void UpdateSessionList();

	// Clear session list
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	void ClearSessionList();

	// Join session
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	void JoinSession(int32 sessionIndex);

protected:
	// Button click handlers
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	void OnBackButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Session Search")
	void OnRefreshButtonClicked();

	// Session entry join handler
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	void OnSessionEntryJoinClicked(int32 sessionIndex);

	// Create session entry widget
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	UHUDSearchSessionEntry* CreateSessionEntry(const FSessionInfo& sessionInfo, int32 index);

public:
	// Search completion handler (called from MPGI)
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	void OnSearchCompleted(bool searchCompleted);

	// Update UI state based on search status
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	void UpdateSearchUI();

	// Calculate if session is available to join
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	bool IsSessionAvailableToJoin(const FSessionInfo& sessionInfo);
};