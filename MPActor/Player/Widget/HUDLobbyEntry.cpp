#include "HUDLobbyEntry.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "HighLevel/MPGI.h"
#include "HighLevel/MPLogManager.h"
#include "MPActor/Player/MPControllerPlayer.h"
#include "MPActor/AI/MPAIController.h"

UHUDLobbyEntry::UHUDLobbyEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Initialize default values
	playerName = TEXT("");
	isBot = false;
	isReady = false;
	playerTeam = ETeam::EHuman;
	playerIndex = -1;
	playerController = nullptr;
	aiController = nullptr;
}

void UHUDLobbyEntry::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind button click event
	if (removeButton)
	{
		removeButton->OnClicked.AddDynamic(this, &UHUDLobbyEntry::OnRemoveButtonClicked);
	}

	// Update UI
	UpdateUI();
}

void UHUDLobbyEntry::NativeDestruct()
{
	// Unbind button click event
	if (removeButton)
	{
		removeButton->OnClicked.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UHUDLobbyEntry::InitializeEntry(const FString& inPlayerName, bool inIsBot, bool inIsReady, 
	ETeam inPlayerTeam, int32 inPlayerIndex, AMPControllerPlayer* inPlayerController, 
	AMPAIController* inAIController)
{
	playerName = inPlayerName;
	isBot = inIsBot;
	isReady = inIsReady;
	playerTeam = inPlayerTeam;
	playerIndex = inPlayerIndex;
	playerController = inPlayerController;
	aiController = inAIController;

	// Update UI
	UpdateUI();

	UMPLogManager::LogDebug(FString::Printf(TEXT("Lobby entry initialized: %s (Bot: %s, Ready: %s, Team: %d)"), 
		*playerName, isBot ? TEXT("Yes") : TEXT("No"), isReady ? TEXT("Yes") : TEXT("No"), (int32)playerTeam), TEXT("HUDLobbyEntry"));
}

void UHUDLobbyEntry::UpdateReadyStatus(bool inIsReady)
{
	isReady = inIsReady;
	
	if (readyStatusText)
	{
		// Get localized text
		UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
		if (gameInstance && gameInstance->GetLocalizationManager())
		{
			FString textKey = isReady ? readyTextKey : notReadyTextKey;
			FText readyText = gameInstance->GetLocalizationManager()->GetLocalizedText(textKey);
			readyStatusText->SetText(readyText);
		}
		else
		{
			FString statusText = isReady ? TEXT("Ready") : TEXT("Not Ready");
			readyStatusText->SetText(FText::FromString(statusText));
		}
	}

	UMPLogManager::LogDebug(FString::Printf(TEXT("Ready status updated: %s (Ready: %s)"), 
		*playerName, isReady ? TEXT("Yes") : TEXT("No")), TEXT("HUDLobbyEntry"));
}

void UHUDLobbyEntry::UpdatePlayerName(const FString& inPlayerName)
{
	playerName = inPlayerName;
	
	if (playerNameText)
	{
		playerNameText->SetText(FText::FromString(playerName));
	}

	UMPLogManager::LogDebug(FString::Printf(TEXT("Player name updated: %s"), *playerName), TEXT("HUDLobbyEntry"));
}

void UHUDLobbyEntry::UpdateHostVisibility(bool isHost)
{
	if (removeButton)
	{
		// Show remove button only for host and only on bots
		bool shouldShow = isHost && isBot;
		removeButton->SetVisibility(shouldShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	UMPLogManager::LogDebug(FString::Printf(TEXT("Host visibility updated: %s (Host: %s, Bot: %s)"), 
		*playerName, isHost ? TEXT("Yes") : TEXT("No"), isBot ? TEXT("Yes") : TEXT("No")), TEXT("HUDLobbyEntry"));
}

void UHUDLobbyEntry::OnRemoveButtonClicked()
{
	if (isBot && playerIndex >= 0)
	{
		UMPLogManager::LogInfo(FString::Printf(TEXT("Remove button clicked for bot: %s (Index: %d)"), 
			*playerName, playerIndex), TEXT("HUDLobbyEntry"));
		
		// Broadcast the remove bot event
		OnRemoveBotClicked.Broadcast(playerIndex);
	}
	else
	{
		UMPLogManager::LogWarning(FString::Printf(TEXT("Remove button clicked but not a bot: %s (Bot: %s, Index: %d)"), 
			*playerName, isBot ? TEXT("Yes") : TEXT("No"), playerIndex), TEXT("HUDLobbyEntry"));
	}
}

void UHUDLobbyEntry::UpdateUI()
{
	// Update player name
	if (playerNameText)
	{
		playerNameText->SetText(FText::FromString(playerName));
	}

	// Update ready status
	UpdateReadyStatus(isReady);

	// Don't set remove button visibility here - let UpdateHostVisibility handle it
	// This prevents conflicts with the host visibility logic
} 