#include "HUDSearchSession.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "HighLevel/MPGI.h"
#include "HighLevel/MPLogManager.h"
#include "MPActor/Player/MPControllerPlayer.h"
#include "CommonStruct.h"

UHUDSearchSession::UHUDSearchSession()
{
	// Initialize default values
	isSearching = false;
	sessionEntryClass = UHUDSearchSessionEntry::StaticClass();
}

void UHUDSearchSession::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Validate root widget first
	if (!ValidateRootWidget())
	{
		UMPLogManager::LogError(TEXT("Root widget validation failed!"), TEXT("HUDSearchSession"));
		return;
	}
	
	// Update texts after construction
	UpdateTexts();
	
	// Bind button events
	if (backButton)
	{
		backButton->OnClicked.AddDynamic(this, &UHUDSearchSession::OnBackButtonClicked);
	}
	
	if (refreshButton)
	{
		refreshButton->OnClicked.AddDynamic(this, &UHUDSearchSession::OnRefreshButtonClicked);
	}
	
	// Start initial search
	StartSessionSearch();
}

void UHUDSearchSession::NativeDestruct()
{
	// Unbind button events
	if (backButton)
	{
		backButton->OnClicked.RemoveAll(this);
	}
	
	if (refreshButton)
	{
		refreshButton->OnClicked.RemoveAll(this);
	}
	
	// Clear session list
	ClearSessionList();
	
	Super::NativeDestruct();
}

void UHUDSearchSession::UpdateTexts()
{
	Super::UpdateTexts();
	
	// Update title
	if (titleText)
	{
		titleText->SetText(GetLocalizedText(titleTextKey));
	}
	
	// Update button texts
	if (backButtonText)
	{
		backButtonText->SetText(GetLocalizedText(backTextKey));
	}
	
	if (refreshButtonText)
	{
		refreshButtonText->SetText(GetLocalizedText(refreshTextKey));
	}
	
	// Update status texts
	if (searchingText)
	{
		searchingText->SetText(GetLocalizedText(searchingTextKey));
	}
	
	if (noSessionsText)
	{
		noSessionsText->SetText(GetLocalizedText(noSessionsTextKey));
	}
}

bool UHUDSearchSession::ValidateRootWidget()
{
	if (!sessionListScrollBox)
	{
		UMPLogManager::LogError(TEXT("Session list scroll box is missing!"), TEXT("HUDSearchSession"));
		return false;
	}
	return true;
}

void UHUDSearchSession::StartSessionSearch()
{
	if (isSearching)
	{
		UMPLogManager::LogWarning(TEXT("Session search already in progress"), TEXT("HUDSearchSession"));
		return;
	}
	
	isSearching = true;
	UpdateSearchUI();
	
	UMPLogManager::LogInfo(TEXT("Starting session search"), TEXT("HUDSearchSession"));
	
	// Get game instance and start search
	UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
	if (gameInstance)
	{
		gameInstance->SearchForSessions();
		// The search completion will be handled by OnSearchCompleted callback
	}
	else
	{
		UMPLogManager::LogError(TEXT("Could not get game instance for session search"), TEXT("HUDSearchSession"));
		StopSessionSearch();
	}
}

void UHUDSearchSession::StopSessionSearch()
{
	isSearching = false;
	UpdateSearchUI();
	
	UMPLogManager::LogInfo(TEXT("Session search stopped"), TEXT("HUDSearchSession"));
}

void UHUDSearchSession::UpdateSessionList()
{
	// Get game instance and session list
	UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
	if (!gameInstance)
	{
		UMPLogManager::LogError(TEXT("Could not get game instance for updating session list"), TEXT("HUDSearchSession"));
		return;
	}
	
	int32 sessionCount = gameInstance->GetSessionListCount();
	UMPLogManager::LogInfo(FString::Printf(TEXT("Updating session list with %d results"), sessionCount), TEXT("HUDSearchSession"));
	
	// Clear existing list
	ClearSessionList();
	
	// Create entries for each session
	for (int32 i = 0; i < sessionCount; ++i)
	{
		FSessionInfo sessionInfo = gameInstance->GetSessionInfo(i);
		UHUDSearchSessionEntry* entry = CreateSessionEntry(sessionInfo, i);
		
		if (entry)
		{
			sessionEntries.Add(entry);
			
			// Add to scroll box
			if (sessionListScrollBox)
			{
				UScrollBoxSlot* slot = Cast<UScrollBoxSlot>(sessionListScrollBox->AddChild(entry));
				if (slot)
				{
					slot->SetPadding(FMargin(5.0f, 2.0f, 5.0f, 2.0f));
				}
			}
		}
	}
	
	// Stop searching
	StopSessionSearch();
	
	// Update UI based on results
	UpdateSearchUI();
}

void UHUDSearchSession::ClearSessionList()
{
	// Remove all entries from scroll box
	if (sessionListScrollBox)
	{
		sessionListScrollBox->ClearChildren();
	}
	
	// Clear array
	sessionEntries.Empty();
	
	UMPLogManager::LogDebug(TEXT("Session list cleared"), TEXT("HUDSearchSession"));
}

void UHUDSearchSession::JoinSession(int32 sessionIndex)
{
	if (sessionIndex < 0 || sessionIndex >= sessionEntries.Num())
	{
		UMPLogManager::LogError(FString::Printf(TEXT("Invalid session index: %d"), sessionIndex), TEXT("HUDSearchSession"));
		return;
	}
	
	UHUDSearchSessionEntry* entry = sessionEntries[sessionIndex];
	if (!entry || !entry->availableToJoin)
	{
		UMPLogManager::LogWarning(FString::Printf(TEXT("Cannot join session at index %d - not available"), sessionIndex), TEXT("HUDSearchSession"));
		return;
	}
	
	UMPLogManager::LogInfo(FString::Printf(TEXT("Joining session: %s (Index: %d)"), *entry->sessionName, sessionIndex), TEXT("HUDSearchSession"));
	
	// Get game instance and join session
	UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
	if (gameInstance)
	{
		gameInstance->JoinSession(sessionIndex);
	}
	else
	{
		UMPLogManager::LogError(TEXT("Could not get game instance for joining session"), TEXT("HUDSearchSession"));
	}
}

void UHUDSearchSession::OnBackButtonClicked()
{
	UMPLogManager::LogInfo(TEXT("Back button clicked"), TEXT("HUDSearchSession"));
	
	if (!owner)
	{
		UMPLogManager::LogError(TEXT("Owner is null, cannot navigate"), TEXT("HUDSearchSession"));
		return;
	}
	
	// Remove current HUD and add Session General HUD
	owner->RemoveHUD(EHUDType::ESearchSession);
	owner->AttachHUD(EHUDType::ESessionGeneral, 0);
	
	UMPLogManager::LogInfo(TEXT("Switched back to Session General HUD"), TEXT("HUDSearchSession"));
}

void UHUDSearchSession::OnRefreshButtonClicked()
{
	UMPLogManager::LogInfo(TEXT("Refresh button clicked"), TEXT("HUDSearchSession"));
	
	// Start new search
	StartSessionSearch();
}

void UHUDSearchSession::OnSessionEntryJoinClicked(int32 sessionIndex)
{
	UMPLogManager::LogInfo(FString::Printf(TEXT("Session entry join clicked for index: %d"), sessionIndex), TEXT("HUDSearchSession"));
	
	// Join the session
	JoinSession(sessionIndex);
}

void UHUDSearchSession::OnSearchCompleted(bool searchCompleted)
{
	UMPLogManager::LogInfo(FString::Printf(TEXT("Search completed with result: %s"), searchCompleted ? TEXT("Success") : TEXT("Failed")), TEXT("HUDSearchSession"));
	
	if (searchCompleted)
	{
		// Update the session list with the new results
		UpdateSessionList();
	}
	else
	{
		// Search failed, stop searching and show error
		StopSessionSearch();
		UMPLogManager::LogWarning(TEXT("Session search failed"), TEXT("HUDSearchSession"));
	}
}

void UHUDSearchSession::UpdateSearchUI()
{
	// Update searching text visibility
	if (searchingText)
	{
		searchingText->SetVisibility(isSearching ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	
	// Update no sessions text visibility
	if (noSessionsText)
	{
		bool showNoSessions = !isSearching && sessionEntries.Num() == 0;
		noSessionsText->SetVisibility(showNoSessions ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	
	// Update refresh button state
	if (refreshButton)
	{
		refreshButton->SetIsEnabled(!isSearching);
	}
	
	UMPLogManager::LogDebug(FString::Printf(TEXT("Search UI updated - Searching: %s, Entries: %d"), 
		isSearching ? TEXT("Yes") : TEXT("No"), sessionEntries.Num()), TEXT("HUDSearchSession"));
}

UHUDSearchSessionEntry* UHUDSearchSession::CreateSessionEntry(const FSessionInfo& sessionInfo, int32 index)
{
	if (!sessionEntryClass)
	{
		UMPLogManager::LogError(TEXT("Session entry class is not set"), TEXT("HUDSearchSession"));
		return nullptr;
	}
	
	// Create the entry widget
	UHUDSearchSessionEntry* entry = CreateWidget<UHUDSearchSessionEntry>(this, sessionEntryClass);
	if (!entry)
	{
		UMPLogManager::LogError(TEXT("Failed to create session entry widget"), TEXT("HUDSearchSession"));
		return nullptr;
	}
	
	// Extract session information from FSessionInfo
	FString sessionName = sessionInfo.sessionName;
	FString hostName = sessionInfo.hostName;
	int32 currentPlayers = sessionInfo.curPlayersNum;
	int32 maxPlayers = sessionInfo.maxPlayersNum;
	int32 ping = sessionInfo.ping;
	bool availableToJoin = IsSessionAvailableToJoin(sessionInfo);
	
	// Initialize the entry
	entry->InitializeEntry(sessionName, hostName, currentPlayers, maxPlayers, ping, availableToJoin, index);
	
	// Bind join event
	entry->OnJoinSessionClicked.AddDynamic(this, &UHUDSearchSession::OnSessionEntryJoinClicked);
	
	UMPLogManager::LogDebug(FString::Printf(TEXT("Created session entry: %s (Index: %d)"), *sessionName, index), TEXT("HUDSearchSession"));
	
	return entry;
}

bool UHUDSearchSession::IsSessionAvailableToJoin(const FSessionInfo& sessionInfo)
{
	// Check if session is not full
	bool notFull = sessionInfo.curPlayersNum < sessionInfo.maxPlayersNum;
	
	// Check if session is not in progress (you might want to add more conditions here)
	bool notInProgress = true; // This could be enhanced based on your game logic
	
	return notFull && notInProgress;
}

