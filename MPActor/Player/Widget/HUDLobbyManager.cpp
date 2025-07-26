#include "HUDLobbyManager.h"
#include "Components/CanvasPanel.h"
#include "Components/Overlay.h"
#include "Components/PanelWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "HUDLobby.h"
#include "HUDCustomCat.h"
#include "HUDCustomHuman.h"
#include "MPControllerPlayer.h"
#include "MPPlayerState.h"
#include "../HighLevel/MPGI.h"
#include "../HighLevel/MPLogManager.h"

UHUDLobbyManager::UHUDLobbyManager()
{
	// Initialize default values
	isReady = false;
	isHost = false;
	currentTeam = ETeam::ENone;
	isCustomizationActive = false;
	
	// Set default HUD classes (these can be overridden in Blueprint)
	lobbyHUDClass = UHUDLobby::StaticClass();
	customCatHUDClass = UHUDCustomCat::StaticClass();
	customHumanHUDClass = UHUDCustomHuman::StaticClass();
	
	// Validate that the classes are valid
	if (!lobbyHUDClass)
	{
		UMPLogManager::LogError(TEXT("Lobby HUD class is null in constructor"), TEXT("HUDLobbyManager"));
	}
	if (!customCatHUDClass)
	{
		UMPLogManager::LogError(TEXT("Custom Cat HUD class is null in constructor"), TEXT("HUDLobbyManager"));
	}
	if (!customHumanHUDClass)
	{
		UMPLogManager::LogError(TEXT("Custom Human HUD class is null in constructor"), TEXT("HUDLobbyManager"));
	}
}

void UHUDLobbyManager::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Find UI components
	FindUIComponents();
	
	// Bind button events (always do this, even if validation fails)
	BindButtonEvents();
	
	// Validate that all required components are found
	if (!ValidateUIComponents())
	{
		UMPLogManager::LogError(TEXT("Required UI components not found - HUD may not function correctly"), TEXT("HUDLobbyManager"));
		// Don't return early - we'll try to initialize anyway when SetOwner is called
	}
	
	// Note: InitializeManager() will be called when SetOwner() is called
	// to ensure proper initialization order
	
	UMPLogManager::LogInfo(TEXT("HUDLobbyManager constructed"), TEXT("HUDLobbyManager"));
}

bool UHUDLobbyManager::ValidateUIComponents() const
{
	bool isValid = true;
	
	if (!lobbyOverlay)
	{
		UMPLogManager::LogError(TEXT("Lobby overlay not found"), TEXT("HUDLobbyManager"));
		isValid = false;
	}
	
	if (!customizationOverlay)
	{
		UMPLogManager::LogError(TEXT("Customization overlay not found"), TEXT("HUDLobbyManager"));
		isValid = false;
	}
	
	if (!backButton)
	{
		UMPLogManager::LogError(TEXT("Back button not found"), TEXT("HUDLobbyManager"));
		isValid = false;
	}
	
	if (!readyButton)
	{
		UMPLogManager::LogError(TEXT("Ready button not found"), TEXT("HUDLobbyManager"));
		isValid = false;
	}
	
	if (!countdownText)
	{
		UMPLogManager::LogWarning(TEXT("Countdown text not found - countdown will not be displayed"), TEXT("HUDLobbyManager"));
		// This is not critical, so we don't set isValid = false
	}
	
	return isValid;
}

void UHUDLobbyManager::SetOwner(AMPControllerPlayer* theOwner)
{
	Super::SetOwner(theOwner);
	
	// Initialize the manager now that we have the owner
	if (owner && !lobbyHUD) // Only initialize if not already done
	{
		// Check if UI components are ready
		if (!lobbyOverlay || !customizationOverlay || !backButton || !readyButton)
		{
			UMPLogManager::LogDebug(TEXT("UI components not ready yet - will initialize when available"), TEXT("HUDLobbyManager"));
			// Don't return early - we'll try to initialize anyway
		}
		
		InitializeManager();
	}
	else if (owner && lobbyHUD)
	{
		// Owner changed but already initialized - just update the owner for child HUDs
		if (lobbyHUD)
		{
			lobbyHUD->SetOwner(owner);
		}
		if (customCatHUD)
		{
			customCatHUD->SetOwner(owner);
		}
		if (customHumanHUD)
		{
			customHumanHUD->SetOwner(owner);
		}
		
		// Refresh current team in case PlayerState is now available
		RefreshCurrentTeam();
		
		UMPLogManager::LogDebug(TEXT("Owner updated for existing HUDs"), TEXT("HUDLobbyManager"));
	}
}

void UHUDLobbyManager::NativeDestruct()
{
	// Unbind button events
	if (backButton)
	{
		backButton->OnClicked.RemoveAll(this);
	}
	
	if (readyButton)
	{
		readyButton->OnClicked.RemoveAll(this);
	}

	// Unbind team change events
	if (lobbyHUD)
	{
		lobbyHUD->OnTeamChanged.RemoveAll(this);
	}

	Super::NativeDestruct();
	
	UMPLogManager::LogInfo(TEXT("Lobby Manager HUD destroyed"), TEXT("HUDLobbyManager"));
}

void UHUDLobbyManager::InitializeManager()
{
	// Ensure we have an owner before initializing
	if (!owner)
	{
		UMPLogManager::LogWarning(TEXT("Cannot initialize manager - owner is null"), TEXT("HUDLobbyManager"));
		return;
	}
	
	// Create HUD widgets
	CreateHUDWidgets();
	
	// Initialize state
	RefreshCurrentTeam();
	isHost = IsHost();
	
	// Update UI
	UpdateUI();
	
	UMPLogManager::LogInfo(TEXT("Lobby Manager initialized"), TEXT("HUDLobbyManager"));
}

void UHUDLobbyManager::RefreshCurrentTeam()
{
	currentTeam = GetCurrentTeam();
	UMPLogManager::LogDebug(FString::Printf(TEXT("Current team refreshed: %d"), (int32)currentTeam), TEXT("HUDLobbyManager"));
}

void UHUDLobbyManager::CreateHUDWidgets()
{
	// Check if required overlays are available
	if (!lobbyOverlay)
	{
		UMPLogManager::LogError(TEXT("Lobby overlay is null - cannot create lobby HUD"), TEXT("HUDLobbyManager"));
		return;
	}
	
	if (!customizationOverlay)
	{
		UMPLogManager::LogError(TEXT("Customization overlay is null - cannot create customization HUDs"), TEXT("HUDLobbyManager"));
		return;
	}
	
	// Create lobby HUD
	if (lobbyHUDClass)
	{
		lobbyHUD = CreateWidget<UHUDLobby>(this, lobbyHUDClass);
		if (lobbyHUD)
		{
			lobbyHUD->SetOwner(owner);
			lobbyOverlay->AddChild(lobbyHUD);
			
			// Bind to team change events
			lobbyHUD->OnTeamChanged.AddDynamic(this, &UHUDLobbyManager::OnTeamChanged);
			
			UMPLogManager::LogInfo(TEXT("Lobby HUD created and added to overlay"), TEXT("HUDLobbyManager"));
		}
		else
		{
			UMPLogManager::LogError(TEXT("Failed to create lobby HUD"), TEXT("HUDLobbyManager"));
		}
	}
	else
	{
		UMPLogManager::LogError(TEXT("Lobby HUD class is null - cannot create lobby HUD"), TEXT("HUDLobbyManager"));
	}
	
	// Create customization HUDs (initially hidden)
	if (customCatHUDClass)
	{
		customCatHUD = CreateWidget<UHUDCustomCat>(this, customCatHUDClass);
		if (customCatHUD)
		{
			customCatHUD->SetOwner(owner);
			customCatHUD->SetVisibility(ESlateVisibility::Hidden);
			customizationOverlay->AddChild(customCatHUD);
			UMPLogManager::LogInfo(TEXT("Custom Cat HUD created and added to overlay"), TEXT("HUDLobbyManager"));
		}
		else
		{
			UMPLogManager::LogError(TEXT("Failed to create custom cat HUD"), TEXT("HUDLobbyManager"));
		}
	}
	else
	{
		UMPLogManager::LogError(TEXT("Custom Cat HUD class is null - cannot create custom cat HUD"), TEXT("HUDLobbyManager"));
	}
	
	if (customHumanHUDClass)
	{
		customHumanHUD = CreateWidget<UHUDCustomHuman>(this, customHumanHUDClass);
		if (customHumanHUD)
		{
			customHumanHUD->SetOwner(owner);
			customHumanHUD->SetVisibility(ESlateVisibility::Hidden);
			customizationOverlay->AddChild(customHumanHUD);
			UMPLogManager::LogInfo(TEXT("Custom Human HUD created and added to overlay"), TEXT("HUDLobbyManager"));
		}
		else
		{
			UMPLogManager::LogError(TEXT("Failed to create custom human HUD"), TEXT("HUDLobbyManager"));
		}
	}
	else
	{
		UMPLogManager::LogError(TEXT("Custom Human HUD class is null - cannot create custom human HUD"), TEXT("HUDLobbyManager"));
	}
}

void UHUDLobbyManager::ShowCustomizationHUD(ETeam team)
{
	UMPLogManager::LogInfo(FString::Printf(TEXT("Showing customization HUD for team: %d"), (int32)team), TEXT("HUDLobbyManager"));
	
	// Validate team value
	if (team == ETeam::ENone)
	{
		UMPLogManager::LogWarning(TEXT("Cannot show customization HUD for ENone team"), TEXT("HUDLobbyManager"));
		HideCustomizationHUD();
		return;
	}
	
	// Hide both customization HUDs first
	if (customCatHUD)
	{
		customCatHUD->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if (customHumanHUD)
	{
		customHumanHUD->SetVisibility(ESlateVisibility::Hidden);
	}
	
	// Show the appropriate customization HUD
	switch (team)
	{
	case ETeam::ECat:
		if (customCatHUD)
		{
			customCatHUD->SetVisibility(ESlateVisibility::Visible);
			isCustomizationActive = true;
			UMPLogManager::LogInfo(TEXT("Cat customization HUD shown"), TEXT("HUDLobbyManager"));
		}
		else
		{
			UMPLogManager::LogError(TEXT("Cat customization HUD is null"), TEXT("HUDLobbyManager"));
		}
		break;
		
	case ETeam::EHuman:
		if (customHumanHUD)
		{
			customHumanHUD->SetVisibility(ESlateVisibility::Visible);
			isCustomizationActive = true;
			UMPLogManager::LogInfo(TEXT("Human customization HUD shown"), TEXT("HUDLobbyManager"));
		}
		else
		{
			UMPLogManager::LogError(TEXT("Human customization HUD is null"), TEXT("HUDLobbyManager"));
		}
		break;
		
	default:
		UMPLogManager::LogWarning(FString::Printf(TEXT("Invalid team for customization HUD: %d"), (int32)team), TEXT("HUDLobbyManager"));
		break;
	}
}

void UHUDLobbyManager::HideCustomizationHUD()
{
	UMPLogManager::LogInfo(TEXT("Hiding customization HUD"), TEXT("HUDLobbyManager"));
	
	if (customCatHUD)
	{
		customCatHUD->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if (customHumanHUD)
	{
		customHumanHUD->SetVisibility(ESlateVisibility::Hidden);
	}
	
	isCustomizationActive = false;
}

void UHUDLobbyManager::UpdateReadyState(bool inIsReady)
{
	isReady = inIsReady;
	UpdateReadyButtonText();
	
	UMPLogManager::LogDebug(FString::Printf(TEXT("Ready state updated: %s"), 
		isReady ? TEXT("Ready") : TEXT("Not Ready")), TEXT("HUDLobbyManager"));
}

void UHUDLobbyManager::UpdateReadyButtonText()
{
	if (readyButtonText)
	{
		FString textKey = isReady ? readyTextKey : notReadyTextKey;
		FText readyText = GetLocalizedText(textKey);
		
		// Fallback to key if localization fails
		if (readyText.IsEmpty())
		{
			readyText = FText::FromString(textKey);
		}
		
		readyButtonText->SetText(readyText);
		
		UMPLogManager::LogDebug(FString::Printf(TEXT("Ready button text updated: %s"), 
			isReady ? TEXT("Ready") : TEXT("Not Ready")), TEXT("HUDLobbyManager"));
	}
}

void UHUDLobbyManager::UpdateCountdownText(int32 secondsRemaining)
{
	if (countdownText)
	{
		// Handle negative values (shouldn't happen but safety check)
		if (secondsRemaining < 0)
		{
			secondsRemaining = 0;
		}
		
		FString countdownString = FString::Printf(TEXT("%d"), secondsRemaining);
		countdownText->SetText(FText::FromString(countdownString));
		
		UMPLogManager::LogDebug(FString::Printf(TEXT("Countdown updated: %d seconds"), secondsRemaining), TEXT("HUDLobbyManager"));
	}
}

void UHUDLobbyManager::SetReadyButtonEnabled(bool enabled)
{
	if (readyButton)
	{
		readyButton->SetIsEnabled(enabled);
	}
}

ETeam UHUDLobbyManager::GetCurrentTeam() const
{
	if (owner)
	{
		AMPPlayerState* playerState = Cast<AMPPlayerState>(owner->PlayerState);
		if (playerState)
		{
			return playerState->playerTeam;
		}
		else
		{
			UMPLogManager::LogWarning(TEXT("Player state is null - cannot get current team"), TEXT("HUDLobbyManager"));
		}
	}
	else
	{
		UMPLogManager::LogWarning(TEXT("Owner is null - cannot get current team"), TEXT("HUDLobbyManager"));
	}
	return ETeam::ENone;
}

bool UHUDLobbyManager::IsHost() const
{
	UWorld* world = GetWorld();
	if (!world)
	{
		UMPLogManager::LogWarning(TEXT("World is null - cannot determine host status"), TEXT("HUDLobbyManager"));
		return false;
	}
	
	UMPGI* gameInstance = Cast<UMPGI>(world->GetGameInstance());
	if (gameInstance)
	{
		return gameInstance->IsHost();
	}
	
	UMPLogManager::LogWarning(TEXT("Game instance is null - cannot determine host status"), TEXT("HUDLobbyManager"));
	return false;
}

void UHUDLobbyManager::OnBackButtonClicked()
{
	UMPLogManager::LogInfo(TEXT("Back button clicked"), TEXT("HUDLobbyManager"));
	
	// Handle back button based on current state
	if (isCustomizationActive)
	{
		// If customization is active, hide it and return to lobby only
		UMPLogManager::LogInfo(TEXT("Hiding customization and returning to lobby"), TEXT("HUDLobbyManager"));
		HideCustomizationHUD();
	}
	else
	{
		// If only lobby is active, leave the session
		UWorld* world = GetWorld();
		if (!world)
		{
			UMPLogManager::LogError(TEXT("World is null - cannot leave session"), TEXT("HUDLobbyManager"));
			return;
		}
		
		UMPGI* gameInstance = Cast<UMPGI>(world->GetGameInstance());
		if (gameInstance)
		{
			UMPLogManager::LogInfo(TEXT("Leaving session"), TEXT("HUDLobbyManager"));
			gameInstance->EndSession();
		}
		else
		{
			UMPLogManager::LogError(TEXT("Could not get game instance for leaving session"), TEXT("HUDLobbyManager"));
		}
	}
}

void UHUDLobbyManager::OnReadyButtonClicked()
{
	UMPLogManager::LogInfo(TEXT("Ready button clicked"), TEXT("HUDLobbyManager"));
	
	if (owner)
	{
		bool newReadyState = !isReady;
		UMPLogManager::LogInfo(FString::Printf(TEXT("Setting ready state to: %s"), 
			newReadyState ? TEXT("Ready") : TEXT("Not Ready")), TEXT("HUDLobbyManager"));
		owner->SetReadyState(newReadyState);
	}
	else
	{
		UMPLogManager::LogError(TEXT("Owner is null, cannot change ready state"), TEXT("HUDLobbyManager"));
	}
}

void UHUDLobbyManager::UpdateUI()
{
	// Update button texts
	if (backButtonText)
	{
		FText backText = GetLocalizedText(backTextKey);
		if (backText.IsEmpty())
		{
			backText = FText::FromString(backTextKey);
		}
		backButtonText->SetText(backText);
	}
	else
	{
		UMPLogManager::LogWarning(TEXT("Back button text is null"), TEXT("HUDLobbyManager"));
	}
	
	UpdateReadyButtonText();
	
	// Update lobby HUD if it exists
	if (lobbyHUD)
	{
		lobbyHUD->UpdateUI();
	}
	else
	{
		UMPLogManager::LogWarning(TEXT("Lobby HUD is null during UI update"), TEXT("HUDLobbyManager"));
	}
	
	UMPLogManager::LogDebug(TEXT("UI updated"), TEXT("HUDLobbyManager"));
}

void UHUDLobbyManager::OnTeamChanged(ETeam newTeam)
{
	UMPLogManager::LogInfo(FString::Printf(TEXT("Team changed to: %d"), (int32)newTeam), TEXT("HUDLobbyManager"));
	
	// Validate team value
	if (newTeam == ETeam::ENone)
	{
		UMPLogManager::LogWarning(TEXT("Team changed to ENone - hiding customization HUD"), TEXT("HUDLobbyManager"));
		HideCustomizationHUD();
		return;
	}
	
	currentTeam = newTeam;
	
	// Show appropriate customization HUD for the new team
	ShowCustomizationHUD(newTeam);
	// --- Ensure preview is always updated after a team switch ---
	if (newTeam == ETeam::ECat && customCatHUD)
	{
		customCatHUD->UpdateCharacterPreview();
	}
	else if (newTeam == ETeam::EHuman && customHumanHUD)
	{
		customHumanHUD->UpdateCharacterPreview();
	}
	if (owner)
	{
		owner->FocusPreviewCamera();
	}
	// -----------------------------------------------------------
} 

void UHUDLobbyManager::BindButtonEvents()
{
	if (backButton)
	{
		backButton->OnClicked.AddDynamic(this, &UHUDLobbyManager::OnBackButtonClicked);
		UMPLogManager::LogDebug(TEXT("Back button event bound"), TEXT("HUDLobbyManager"));
	}
	else
	{
		UMPLogManager::LogError(TEXT("Back button is null - cannot bind event"), TEXT("HUDLobbyManager"));
	}
	
	if (readyButton)
	{
		readyButton->OnClicked.AddDynamic(this, &UHUDLobbyManager::OnReadyButtonClicked);
		UMPLogManager::LogDebug(TEXT("Ready button event bound"), TEXT("HUDLobbyManager"));
	}
	else
	{
		UMPLogManager::LogError(TEXT("Ready button is null - cannot bind event"), TEXT("HUDLobbyManager"));
	}
} 

void UHUDLobbyManager::FindUIComponents()
{
	// Find UI components using GetWidgetFromName
	// Note: These should be bound via meta = (BindWidget) in Blueprint
	// This method is a fallback in case binding fails
	
	if (!lobbyOverlay)
	{
		UMPLogManager::LogWarning(TEXT("Lobby overlay not found via binding - trying GetWidgetFromName"), TEXT("HUDLobbyManager"));
		lobbyOverlay = Cast<UOverlay>(GetWidgetFromName(TEXT("LobbyOverlay")));
	}
	
	if (!customizationOverlay)
	{
		UMPLogManager::LogWarning(TEXT("Customization overlay not found via binding - trying GetWidgetFromName"), TEXT("HUDLobbyManager"));
		customizationOverlay = Cast<UOverlay>(GetWidgetFromName(TEXT("CustomizationOverlay")));
	}
	
	if (!backButton)
	{
		UMPLogManager::LogWarning(TEXT("Back button not found via binding - trying GetWidgetFromName"), TEXT("HUDLobbyManager"));
		backButton = Cast<UButton>(GetWidgetFromName(TEXT("BackButton")));
	}
	
	if (!readyButton)
	{
		UMPLogManager::LogWarning(TEXT("Ready button not found via binding - trying GetWidgetFromName"), TEXT("HUDLobbyManager"));
		readyButton = Cast<UButton>(GetWidgetFromName(TEXT("ReadyButton")));
	}
	
	if (!countdownText)
	{
		UMPLogManager::LogWarning(TEXT("Countdown text not found via binding - trying GetWidgetFromName"), TEXT("HUDLobbyManager"));
		countdownText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CountdownText")));
	}
	
	if (!backButtonText)
	{
		UMPLogManager::LogWarning(TEXT("Back button text not found via binding - trying GetWidgetFromName"), TEXT("HUDLobbyManager"));
		backButtonText = Cast<UTextBlock>(GetWidgetFromName(TEXT("BackButtonText")));
	}
	
	if (!readyButtonText)
	{
		UMPLogManager::LogWarning(TEXT("Ready button text not found via binding - trying GetWidgetFromName"), TEXT("HUDLobbyManager"));
		readyButtonText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ReadyButtonText")));
	}
	
	UMPLogManager::LogDebug(TEXT("UI components search completed"), TEXT("HUDLobbyManager"));
} 