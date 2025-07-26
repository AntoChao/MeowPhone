#include "HUDSearchSessionEntry.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "HighLevel/MPGI.h"
#include "HighLevel/MPLogManager.h"

UHUDSearchSessionEntry::UHUDSearchSessionEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Initialize default values
	sessionName = TEXT("");
	hostName = TEXT("");
	currentPlayersNumber = 0;
	maxPlayersNumber = 0;
	ping = 0;
	availableToJoin = false;
	sessionIndex = -1;
}

void UHUDSearchSessionEntry::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button click event
	if (joinButton)
	{
		joinButton->OnClicked.AddDynamic(this, &UHUDSearchSessionEntry::OnJoinButtonClicked);
	}

	// Update button state
	UpdateButtonState();
}

void UHUDSearchSessionEntry::NativeDestruct()
{
	// Unbind button click event
	if (joinButton)
	{
		joinButton->OnClicked.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UHUDSearchSessionEntry::InitializeEntry(const FString& inSessionName, const FString& inHostName, 
	int32 inCurrentPlayersNumber, int32 inMaxPlayersNumber, int32 inPing, bool inAvailableToJoin, int32 inSessionIndex)
{
	sessionName = inSessionName;
	hostName = inHostName;
	currentPlayersNumber = inCurrentPlayersNumber;
	maxPlayersNumber = inMaxPlayersNumber;
	ping = inPing;
	availableToJoin = inAvailableToJoin;
	sessionIndex = inSessionIndex;

	// Update UI components
	if (sessionNameText)
	{
		sessionNameText->SetText(FText::FromString(sessionName));
	}

	if (hostNameText)
	{
		hostNameText->SetText(FText::FromString(hostName));
	}

	if (currentPlayersNumberText)
	{
		currentPlayersNumberText->SetText(FText::FromString(FString::FromInt(currentPlayersNumber)));
	}

	if (maxPlayersNumberText)
	{
		maxPlayersNumberText->SetText(FText::FromString(FString::FromInt(maxPlayersNumber)));
	}

	if (pingText)
	{
		pingText->SetText(FText::FromString(FString::FromInt(ping)));
	}

	// Update button state
	UpdateButtonState();

	UMPLogManager::LogDebug(FString::Printf(TEXT("Session entry initialized: %s (Host: %s, Players: %d/%d, Ping: %d, Available: %s)"), 
		*sessionName, *hostName, currentPlayersNumber, maxPlayersNumber, ping, availableToJoin ? TEXT("Yes") : TEXT("No")), TEXT("HUDSearchSessionEntry"));
}

void UHUDSearchSessionEntry::UpdateAvailability(bool inAvailableToJoin)
{
	availableToJoin = inAvailableToJoin;
	UpdateButtonState();

	UMPLogManager::LogDebug(FString::Printf(TEXT("Session availability updated: %s (Available: %s)"), 
		*sessionName, availableToJoin ? TEXT("Yes") : TEXT("No")), TEXT("HUDSearchSessionEntry"));
}

void UHUDSearchSessionEntry::UpdatePing(int32 inPing)
{
	ping = inPing;
	
	if (pingText)
	{
		pingText->SetText(FText::FromString(FString::FromInt(ping)));
	}

	UMPLogManager::LogDebug(FString::Printf(TEXT("Session ping updated: %s (Ping: %d)"), 
		*sessionName, ping), TEXT("HUDSearchSessionEntry"));
}

void UHUDSearchSessionEntry::UpdatePlayerCount(int32 inCurrentPlayersNumber)
{
	currentPlayersNumber = inCurrentPlayersNumber;
	
	if (currentPlayersNumberText)
	{
		currentPlayersNumberText->SetText(FText::FromString(FString::FromInt(currentPlayersNumber)));
	}

	UMPLogManager::LogDebug(FString::Printf(TEXT("Session player count updated: %s (Players: %d/%d)"), 
		*sessionName, currentPlayersNumber, maxPlayersNumber), TEXT("HUDSearchSessionEntry"));
}

void UHUDSearchSessionEntry::OnJoinButtonClicked()
{
	if (availableToJoin && sessionIndex >= 0)
	{
		UMPLogManager::LogInfo(FString::Printf(TEXT("Join button clicked for session: %s (Index: %d)"), 
			*sessionName, sessionIndex), TEXT("HUDSearchSessionEntry"));
		
		// Broadcast the join session event
		OnJoinSessionClicked.Broadcast(sessionIndex);
	}
	else
	{
		UMPLogManager::LogWarning(FString::Printf(TEXT("Join button clicked but session not available: %s (Available: %s, Index: %d)"), 
			*sessionName, availableToJoin ? TEXT("Yes") : TEXT("No"), sessionIndex), TEXT("HUDSearchSessionEntry"));
	}
}

void UHUDSearchSessionEntry::UpdateButtonState()
{
	if (joinButton)
	{
		joinButton->SetIsEnabled(availableToJoin);
	}

	if (joinButtonText)
	{
		// Get localized text for join button
		UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
		if (gameInstance && gameInstance->GetLocalizationManager())
		{
			FText joinText = gameInstance->GetLocalizationManager()->GetLocalizedText(TEXT("JOIN"));
			joinButtonText->SetText(joinText);
		}
		else
		{
			joinButtonText->SetText(FText::FromString(TEXT("Join")));
		}
	}
} 