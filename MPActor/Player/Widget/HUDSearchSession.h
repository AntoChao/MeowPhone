#pragma once

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

	// Search completion handler (called from MPGI)
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	void OnSearchCompleted(bool searchCompleted);

	// Update UI state based on search status
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	void UpdateSearchUI();

	// Create session entry widget
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	UHUDSearchSessionEntry* CreateSessionEntry(const FSessionInfo& sessionInfo, int32 index);

	// Calculate if session is available to join
	UFUNCTION(BlueprintCallable, Category = "Session Search")
	bool IsSessionAvailableToJoin(const FSessionInfo& sessionInfo);
};