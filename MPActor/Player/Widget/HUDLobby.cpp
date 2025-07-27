#include "HUDLobby.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "../MPControllerPlayer.h"
#include "../MPPlayerState.h"
#include "../../AI/MPAIController.h"
#include "../../../HighLevel/MPGMGameplay.h"
#include "../../../HighLevel/MPGI.h"
#include "../../../HighLevel/Managers/ManagerLog.h"
#include "../../../HighLevel/Managers/ManagerAIController.h"
#include "HUDLobbyEntry.h"

UHUDLobby::UHUDLobby()
{
	// Initialize default values
	isReady = false;
	isHost = false;
	currentTeam = ETeam::ENone;
}

void UHUDLobby::NativeConstruct()
{
    Super::NativeConstruct();

	// Bind button events
	if (humanJoinButton)
	{
		humanJoinButton->OnClicked.AddDynamic(this, &UHUDLobby::OnHumanJoinButtonClicked);
	}
	
	if (catJoinButton)
	{
		catJoinButton->OnClicked.AddDynamic(this, &UHUDLobby::OnCatJoinButtonClicked);
	}
	
	if (humanAddButton)
	{
		humanAddButton->OnClicked.AddDynamic(this, &UHUDLobby::OnHumanAddButtonClicked);
	}
	
	if (catAddButton)
	{
		catAddButton->OnClicked.AddDynamic(this, &UHUDLobby::OnCatAddButtonClicked);
	}

	// Initialize UI
	UpdateUI();
	UpdatePlayerLists();
	
	UManagerLog::LogInfo(TEXT("Lobby HUD constructed and initialized"), TEXT("HUDLobby"));
}

void UHUDLobby::NativeDestruct()
{
	// Unbind button events
	if (humanJoinButton)
	{
		humanJoinButton->OnClicked.RemoveAll(this);
	}
	
	if (catJoinButton)
	{
		catJoinButton->OnClicked.RemoveAll(this);
	}
	
	if (humanAddButton)
	{
		humanAddButton->OnClicked.RemoveAll(this);
	}
	
	if (catAddButton)
	{
		catAddButton->OnClicked.RemoveAll(this);
	}

	// Clear player lists
	ClearPlayerLists();

	Super::NativeDestruct();
	
	UManagerLog::LogInfo(TEXT("Lobby HUD destroyed"), TEXT("HUDLobby"));
}

void UHUDLobby::UpdatePlayerLists()
{
	UManagerLog::LogDebug(TEXT("Updating player lists"), TEXT("HUDLobby"));
	
	// Clear existing lists
	ClearPlayerLists();
	
	// Get GameMode to fetch player data
	AMPGMGameplay* gameMode = Cast<AMPGMGameplay>(GetWorld()->GetAuthGameMode());
	if (!gameMode)
	{
		UManagerLog::LogError(TEXT("Could not get GameMode for player list update"), TEXT("HUDLobby"));
		return;
	}
	
	// Get human players
	TArray<AMPControllerPlayer*> humanPlayers = gameMode->GetHumanPlayers();
	for (AMPControllerPlayer* player : humanPlayers)
	{
		if (player)
		{
			AMPPlayerState* playerState = Cast<AMPPlayerState>(player->PlayerState);
			if (playerState)
			{
				UHUDLobbyEntry* entry = CreateLobbyEntry(
					playerState->playerName,
					false, // Not a bot
					playerState->isPlayerReady,
					ETeam::EHuman,
					playerState->playerIndex,
					player,
					nullptr // No AI controller for human players
				);
				
				if (entry)
				{
					humanPlayersList.Add(entry);
					if (humanPlayersScrollBox)
					{
						humanPlayersScrollBox->AddChild(entry);
					}
				}
			}
		}
	}
	
	// Get cat players
	TArray<AMPControllerPlayer*> catPlayers = gameMode->GetCatPlayers();
	for (AMPControllerPlayer* player : catPlayers)
	{
		if (player)
		{
			AMPPlayerState* playerState = Cast<AMPPlayerState>(player->PlayerState);
			if (playerState)
			{
				UHUDLobbyEntry* entry = CreateLobbyEntry(
					playerState->playerName,
					false, // Not a bot
					playerState->isPlayerReady,
					ETeam::ECat,
					playerState->playerIndex,
					player,
					nullptr // No AI controller for human players
				);
				
				if (entry)
				{
					catPlayersList.Add(entry);
					if (catPlayersScrollBox)
					{
						catPlayersScrollBox->AddChild(entry);
					}
				}
			}
		}
	}
	
	// Get human AI bots
	TArray<AMPAIController*> humanAIs = gameMode->GetManagerAIController()->GetAllAIHumans();
	for (int32 i = 0; i < humanAIs.Num(); i++)
	{
		AMPAIController* aiController = humanAIs[i];
		if (aiController)
		{
			FString botName = FString::Printf(TEXT("Human Bot %d"), i + 1);
			UHUDLobbyEntry* entry = CreateLobbyEntry(
				botName,
				true, // Is a bot
				false, // Bots are always ready
				ETeam::EHuman,
				i, // Index in bot list
				nullptr, // No player controller for bots
				aiController
			);
			
			if (entry)
			{
				humanPlayersList.Add(entry);
				if (humanPlayersScrollBox)
				{
					humanPlayersScrollBox->AddChild(entry);
				}
			}
		}
	}
	
	// Get cat AI bots
	TArray<AMPAIController*> catAIs = gameMode->GetManagerAIController()->GetAllAICats();
	for (int32 i = 0; i < catAIs.Num(); i++)
	{
		AMPAIController* aiController = catAIs[i];
		if (aiController)
		{
			FString botName = FString::Printf(TEXT("Cat Bot %d"), i + 1);
			UHUDLobbyEntry* entry = CreateLobbyEntry(
				botName,
				true, // Is a bot
				false, // Bots are always ready
				ETeam::ECat,
				humanAIs.Num() + i, // Index in combined bot list
				nullptr, // No player controller for bots
				aiController
			);
			
			if (entry)
			{
				catPlayersList.Add(entry);
				if (catPlayersScrollBox)
				{
					catPlayersScrollBox->AddChild(entry);
				}
			}
		}
	}
	
	UManagerLog::LogDebug(FString::Printf(TEXT("Updated player lists - Humans: %d, Cats: %d"), 
		humanPlayersList.Num(), catPlayersList.Num()), TEXT("HUDLobby"));
}

void UHUDLobby::ClearPlayerLists()
{
	// Clear human players
	for (UHUDLobbyEntry* entry : humanPlayersList)
	{
		if (entry)
		{
			entry->RemoveFromParent();
		}
	}
	humanPlayersList.Empty();
	
	// Clear cat players
	for (UHUDLobbyEntry* entry : catPlayersList)
	{
		if (entry)
		{
			entry->RemoveFromParent();
		}
	}
	catPlayersList.Empty();
	
	// Clear scroll boxes
	if (humanPlayersScrollBox)
	{
		humanPlayersScrollBox->ClearChildren();
	}
	
	if (catPlayersScrollBox)
	{
		catPlayersScrollBox->ClearChildren();
	}
}

void UHUDLobby::UpdateReadyState(bool inIsReady)
{
	isReady = inIsReady;
	
	UManagerLog::LogDebug(FString::Printf(TEXT("Ready state updated: %s"), 
		isReady ? TEXT("Ready") : TEXT("Not Ready")), TEXT("HUDLobby"));
}

void UHUDLobby::UpdateHostVisibility()
{
	isHost = IsHost();
	
	// Update add bot buttons visibility
	if (humanAddButton)
	{
		humanAddButton->SetVisibility(isHost ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	
	if (catAddButton)
	{
		catAddButton->SetVisibility(isHost ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	
	// Update host visibility for all entries
	for (UHUDLobbyEntry* entry : humanPlayersList)
	{
		if (entry)
		{
			entry->UpdateHostVisibility(isHost);
		}
	}
	
	for (UHUDLobbyEntry* entry : catPlayersList)
	{
		if (entry)
		{
			entry->UpdateHostVisibility(isHost);
		}
	}
	
	UManagerLog::LogDebug(FString::Printf(TEXT("Host visibility updated: %s"), 
		isHost ? TEXT("Host") : TEXT("Client")), TEXT("HUDLobby"));
}

void UHUDLobby::UpdateTeamButtonVisibility()
{
	currentTeam = GetCurrentTeam();
	
	// Show join button for opposite team
	if (humanJoinButton)
	{
		bool showHumanJoin = (currentTeam == ETeam::ECat);
		humanJoinButton->SetVisibility(showHumanJoin ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	
	if (catJoinButton)
	{
		bool showCatJoin = (currentTeam == ETeam::EHuman);
		catJoinButton->SetVisibility(showCatJoin ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	
	UManagerLog::LogDebug(FString::Printf(TEXT("Team button visibility updated: Current Team: %d"), 
		(int32)currentTeam), TEXT("HUDLobby"));
}

void UHUDLobby::UpdateCountdownText(int32 secondsRemaining)
{
	if (countdownText)
	{
		FString countdownString = FString::Printf(TEXT("Game starts in %d seconds"), secondsRemaining);
		countdownText->SetText(FText::FromString(countdownString));
	}
	else
	{
		UManagerLog::LogWarning(TEXT("Countdown text widget is not set"), TEXT("HUDLobby"));
	}
}

void UHUDLobby::OnHumanJoinButtonClicked()
{
	UManagerLog::LogInfo(TEXT("Join Human button clicked"), TEXT("HUDLobby"));
	
	if (owner)
	{
		owner->RequestTeamSwitch(ETeam::EHuman);
		// Broadcast team change event
		OnTeamChanged.Broadcast(ETeam::EHuman);
	}
	else
	{
		UManagerLog::LogError(TEXT("Owner is null, cannot switch team"), TEXT("HUDLobby"));
	}
}

void UHUDLobby::OnCatJoinButtonClicked()
{
	UManagerLog::LogInfo(TEXT("Join Cat button clicked"), TEXT("HUDLobby"));
	
	if (owner)
	{
		owner->RequestTeamSwitch(ETeam::ECat);
		// Broadcast team change event
		OnTeamChanged.Broadcast(ETeam::ECat);
	}
	else
	{
		UManagerLog::LogError(TEXT("Owner is null, cannot switch team"), TEXT("HUDLobby"));
	}
}

void UHUDLobby::OnHumanAddButtonClicked()
{
	UManagerLog::LogInfo(TEXT("Add Human button clicked"), TEXT("HUDLobby"));
	
	if (owner && isHost)
	{
		owner->RequestAddBot(ETeam::EHuman);
	}
	else
	{
		UManagerLog::LogWarning(TEXT("Cannot add human bot - not host or owner is null"), TEXT("HUDLobby"));
	}
}

void UHUDLobby::OnCatAddButtonClicked()
{
	UManagerLog::LogInfo(TEXT("Add Cat button clicked"), TEXT("HUDLobby"));
	
	if (owner && isHost)
	{
		owner->RequestAddBot(ETeam::ECat);
	}
	else
	{
		UManagerLog::LogWarning(TEXT("Cannot add cat bot - not host or owner is null"), TEXT("HUDLobby"));
	}
}

void UHUDLobby::OnRemoveBotClicked(int32 playerIndex)
{
	UManagerLog::LogInfo(FString::Printf(TEXT("Remove bot clicked for index: %d"), playerIndex), TEXT("HUDLobby"));
	
	if (owner && isHost)
	{
		owner->RequestRemoveBot(playerIndex);
	}
	else
	{
		UManagerLog::LogWarning(TEXT("Cannot remove bot - not host or owner is null"), TEXT("HUDLobby"));
	}
}

void UHUDLobby::UpdateUI()
{
	UpdateHostVisibility();
	UpdateTeamButtonVisibility();
}

UHUDLobbyEntry* UHUDLobby::CreateLobbyEntry(const FString& playerName, bool isBot, bool isEntryReady, 
	ETeam team, int32 playerIndex, AMPControllerPlayer* playerController, AMPAIController* aiController)
{
	if (!lobbyEntryClass)
	{
		UManagerLog::LogError(TEXT("Lobby entry class is not set"), TEXT("HUDLobby"));
		return nullptr;
	}
	
	// Create the entry widget
	UHUDLobbyEntry* entry = CreateWidget<UHUDLobbyEntry>(this, lobbyEntryClass);
	if (!entry)
	{
		UManagerLog::LogError(TEXT("Failed to create lobby entry widget"), TEXT("HUDLobby"));
		return nullptr;
	}
	
	// Initialize the entry
	entry->InitializeEntry(playerName, isBot, isEntryReady, team, playerIndex, playerController, aiController);
	
	// Bind remove bot event
	entry->OnRemoveBotClicked.AddDynamic(this, &UHUDLobby::OnRemoveBotClicked);
	
	// Update host visibility
	entry->UpdateHostVisibility(isHost);
	
	UManagerLog::LogDebug(FString::Printf(TEXT("Created lobby entry: %s (Bot: %s, Team: %d)"), 
		*playerName, isBot ? TEXT("Yes") : TEXT("No"), (int32)team), TEXT("HUDLobby"));
	
	return entry;
}

ETeam UHUDLobby::GetCurrentTeam() const
{
	if (owner)
	{
		return owner->GetCurrentTeam();
	}
	return ETeam::ENone;
}

bool UHUDLobby::IsHost() const
{
	UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
	if (gameInstance)
	{
		return gameInstance->IsHost();
	}
	return false;
}