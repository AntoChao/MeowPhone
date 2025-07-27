#include "HUDLobbyManager.h"
#include "Components/CanvasPanel.h"
#include "Components/Overlay.h"
#include "Components/PanelWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "HUDLobby.h"
#include "HUDCustomCat.h"
#include "HUDCustomHuman.h"
#include "../MPControllerPlayer.h"
#include "../MPPlayerState.h"
#include "../../../HighLevel/MPGI.h"
#include "../../../HighLevel/Managers/ManagerLog.h"

UHUDManagerLobby::UHUDManagerLobby()
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
		UManagerLog::LogError(TEXT("Lobby HUD class is null in constructor"), TEXT("HUDManagerLobby"));
	}
	if (!customCatHUDClass)
	{
		UManagerLog::LogError(TEXT("Custom Cat HUD class is null in constructor"), TEXT("HUDManagerLobby"));
	}
	if (!customHumanHUDClass)
	{
		UManagerLog::LogError(TEXT("Custom Human HUD class is null in constructor"), TEXT("HUDManagerLobby"));
	}
}

void UHUDManagerLobby::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Find UI components
	FindUIComponents();
	
	// Bind button events (always do this, even if validation fails)
	BindButtonEvents();
	
	// Validate that all required components are found
	if (!ValidateUIComponents())
	{
		UManagerLog::LogError(TEXT("Required UI components not found - HUD may not function correctly"), TEXT("HUDManagerLobby"));
		// Don't return early - we'll try to initialize anyway when SetOwner is called
	}
	
	// Note: InitializeManager() will be called when SetOwner() is called
	// to ensure proper initialization order
	
	UManagerLog::LogInfo(TEXT("HUDManagerLobby constructed"), TEXT("HUDManagerLobby"));
}

bool UHUDManagerLobby::ValidateUIComponents() const
{
	bool isValid = true;
	
	if (!lobbyOverlay)
	{
		UManagerLog::LogError(TEXT("Lobby overlay not found"), TEXT("HUDManagerLobby"));
		isValid = false;
	}
	
	if (!customizationOverlay)
	{
		UManagerLog::LogError(TEXT("Customization overlay not found"), TEXT("HUDManagerLobby"));
		isValid = false;
	}
	
	if (!backButton)
	{
		UManagerLog::LogError(TEXT("Back button not found"), TEXT("HUDManagerLobby"));
		isValid = false;
	}
	
	if (!readyButton)
	{
		UManagerLog::LogError(TEXT("Ready button not found"), TEXT("HUDManagerLobby"));
		isValid = false;
	}
	
	if (!countdownText)
	{
		UManagerLog::LogWarning(TEXT("Countdown text not found - countdown will not be displayed"), TEXT("HUDManagerLobby"));
		// This is not critical, so we don't set isValid = false
	}
	
	return isValid;
}

void UHUDManagerLobby::SetOwner(AMPControllerPlayer* theOwner)
{
	Super::SetOwner(theOwner);
	
	// Initialize the manager now that we have the owner
	if (owner && !lobbyHUD) // Only initialize if not already done
	{
		// Check if UI components are ready
		if (!lobbyOverlay || !customizationOverlay || !backButton || !readyButton)
		{
			UManagerLog::LogDebug(TEXT("UI components not ready yet - will initialize when available"), TEXT("HUDManagerLobby"));
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
		
		UManagerLog::LogDebug(TEXT("Owner updated for existing HUDs"), TEXT("HUDManagerLobby"));
	}
}

void UHUDManagerLobby::NativeDestruct()
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
	
	UManagerLog::LogInfo(TEXT("Lobby Manager HUD destroyed"), TEXT("HUDManagerLobby"));
}

void UHUDManagerLobby::InitializeManager()
{
	// Ensure we have an owner before initializing
	if (!owner)
	{
		UManagerLog::LogWarning(TEXT("Cannot initialize manager - owner is null"), TEXT("HUDManagerLobby"));
		return;
	}
	
	// Create HUD widgets
	CreateHUDWidgets();
	
	// Initialize state
	RefreshCurrentTeam();
	isHost = IsHost();
	
	// Update UI
	UpdateUI();
	
	UManagerLog::LogInfo(TEXT("Lobby Manager initialized"), TEXT("HUDManagerLobby"));
}

void UHUDManagerLobby::RefreshCurrentTeam()
{
	currentTeam = GetCurrentTeam();
	UManagerLog::LogDebug(FString::Printf(TEXT("Current team refreshed: %d"), (int32)currentTeam), TEXT("HUDManagerLobby"));
}

void UHUDManagerLobby::CreateHUDWidgets()
{
	// Check if required overlays are available
	if (!lobbyOverlay)
	{
		UManagerLog::LogError(TEXT("Lobby overlay is null - cannot create lobby HUD"), TEXT("HUDManagerLobby"));
		return;
	}
	
	if (!customizationOverlay)
	{
		UManagerLog::LogError(TEXT("Customization overlay is null - cannot create customization HUDs"), TEXT("HUDManagerLobby"));
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
			lobbyHUD->OnTeamChanged.AddDynamic(this, &UHUDManagerLobby::OnTeamChanged);
			
			UManagerLog::LogInfo(TEXT("Lobby HUD created and added to overlay"), TEXT("HUDManagerLobby"));
		}
		else
		{
			UManagerLog::LogError(TEXT("Failed to create lobby HUD"), TEXT("HUDManagerLobby"));
		}
	}
	else
	{
		UManagerLog::LogError(TEXT("Lobby HUD class is null - cannot create lobby HUD"), TEXT("HUDManagerLobby"));
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
			UManagerLog::LogInfo(TEXT("Custom Cat HUD created and added to overlay"), TEXT("HUDManagerLobby"));
		}
		else
		{
			UManagerLog::LogError(TEXT("Failed to create custom cat HUD"), TEXT("HUDManagerLobby"));
		}
	}
	else
	{
		UManagerLog::LogError(TEXT("Custom Cat HUD class is null - cannot create custom cat HUD"), TEXT("HUDManagerLobby"));
	}
	
	if (customHumanHUDClass)
	{
		customHumanHUD = CreateWidget<UHUDCustomHuman>(this, customHumanHUDClass);
		if (customHumanHUD)
		{
			customHumanHUD->SetOwner(owner);
			customHumanHUD->SetVisibility(ESlateVisibility::Hidden);
			customizationOverlay->AddChild(customHumanHUD);
			UManagerLog::LogInfo(TEXT("Custom Human HUD created and added to overlay"), TEXT("HUDManagerLobby"));
		}
		else
		{
			UManagerLog::LogError(TEXT("Failed to create custom human HUD"), TEXT("HUDManagerLobby"));
		}
	}
	else
	{
		UManagerLog::LogError(TEXT("Custom Human HUD class is null - cannot create custom human HUD"), TEXT("HUDManagerLobby"));
	}
}

void UHUDManagerLobby::ShowCustomizationHUD(ETeam team)
{
	UManagerLog::LogInfo(FString::Printf(TEXT("Showing customization HUD for team: %d"), (int32)team), TEXT("HUDManagerLobby"));
	
	// Validate team value
	if (team == ETeam::ENone)
	{
		UManagerLog::LogWarning(TEXT("Cannot show customization HUD for ENone team"), TEXT("HUDManagerLobby"));
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
			UManagerLog::LogInfo(TEXT("Cat customization HUD shown"), TEXT("HUDManagerLobby"));
		}
		else
		{
			UManagerLog::LogError(TEXT("Cat customization HUD is null"), TEXT("HUDManagerLobby"));
		}
		break;
		
	case ETeam::EHuman:
		if (customHumanHUD)
		{
			customHumanHUD->SetVisibility(ESlateVisibility::Visible);
			isCustomizationActive = true;
			UManagerLog::LogInfo(TEXT("Human customization HUD shown"), TEXT("HUDManagerLobby"));
		}
		else
		{
			UManagerLog::LogError(TEXT("Human customization HUD is null"), TEXT("HUDManagerLobby"));
		}
		break;
		
	default:
		UManagerLog::LogWarning(FString::Printf(TEXT("Invalid team for customization HUD: %d"), (int32)team), TEXT("HUDManagerLobby"));
		break;
	}
}

void UHUDManagerLobby::HideCustomizationHUD()
{
	UManagerLog::LogInfo(TEXT("Hiding customization HUD"), TEXT("HUDManagerLobby"));
	
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

void UHUDManagerLobby::UpdateReadyState(bool inIsReady)
{
	isReady = inIsReady;
	UpdateReadyButtonText();
	
	UManagerLog::LogDebug(FString::Printf(TEXT("Ready state updated: %s"), 
		isReady ? TEXT("Ready") : TEXT("Not Ready")), TEXT("HUDManagerLobby"));
}

void UHUDManagerLobby::UpdateReadyButtonText()
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
		
		UManagerLog::LogDebug(FString::Printf(TEXT("Ready button text updated: %s"), 
			isReady ? TEXT("Ready") : TEXT("Not Ready")), TEXT("HUDManagerLobby"));
	}
}

void UHUDManagerLobby::UpdateCountdownText(int32 secondsRemaining)
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
		
		UManagerLog::LogDebug(FString::Printf(TEXT("Countdown updated: %d seconds"), secondsRemaining), TEXT("HUDManagerLobby"));
	}
}

void UHUDManagerLobby::SetReadyButtonEnabled(bool enabled)
{
	if (readyButton)
	{
		readyButton->SetIsEnabled(enabled);
	}
}

ETeam UHUDManagerLobby::GetCurrentTeam() const
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
			UManagerLog::LogWarning(TEXT("Player state is null - cannot get current team"), TEXT("HUDManagerLobby"));
		}
	}
	else
	{
		UManagerLog::LogWarning(TEXT("Owner is null - cannot get current team"), TEXT("HUDManagerLobby"));
	}
	return ETeam::ENone;
}

bool UHUDManagerLobby::IsHost() const
{
	UWorld* world = GetWorld();
	if (!world)
	{
		UManagerLog::LogWarning(TEXT("World is null - cannot determine host status"), TEXT("HUDManagerLobby"));
		return false;
	}
	
	UMPGI* gameInstance = Cast<UMPGI>(world->GetGameInstance());
	if (gameInstance)
	{
		return gameInstance->IsHost();
	}
	
	UManagerLog::LogWarning(TEXT("Game instance is null - cannot determine host status"), TEXT("HUDManagerLobby"));
	return false;
}

void UHUDManagerLobby::OnBackButtonClicked()
{
	UManagerLog::LogInfo(TEXT("Back button clicked"), TEXT("HUDManagerLobby"));
	
	// Handle back button based on current state
	if (isCustomizationActive)
	{
		// If customization is active, hide it and return to lobby only
		UManagerLog::LogInfo(TEXT("Hiding customization and returning to lobby"), TEXT("HUDManagerLobby"));
		HideCustomizationHUD();
	}
	else
	{
		// If only lobby is active, leave the session
		UWorld* world = GetWorld();
		if (!world)
		{
			UManagerLog::LogError(TEXT("World is null - cannot leave session"), TEXT("HUDManagerLobby"));
			return;
		}
		
		UMPGI* gameInstance = Cast<UMPGI>(world->GetGameInstance());
		if (gameInstance)
		{
			UManagerLog::LogInfo(TEXT("Leaving session"), TEXT("HUDManagerLobby"));
			gameInstance->EndSession();
		}
		else
		{
			UManagerLog::LogError(TEXT("Could not get game instance for leaving session"), TEXT("HUDManagerLobby"));
		}
	}
}

void UHUDManagerLobby::OnReadyButtonClicked()
{
	UManagerLog::LogInfo(TEXT("Ready button clicked"), TEXT("HUDManagerLobby"));
	
	if (owner)
	{
		bool newReadyState = !isReady;
		UManagerLog::LogInfo(FString::Printf(TEXT("Setting ready state to: %s"), 
			newReadyState ? TEXT("Ready") : TEXT("Not Ready")), TEXT("HUDManagerLobby"));
		owner->SetReadyState(newReadyState);
	}
	else
	{
		UManagerLog::LogError(TEXT("Owner is null, cannot change ready state"), TEXT("HUDManagerLobby"));
	}
}

void UHUDManagerLobby::UpdateUI()
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
		UManagerLog::LogWarning(TEXT("Back button text is null"), TEXT("HUDManagerLobby"));
	}
	
	UpdateReadyButtonText();
	
	// Update lobby HUD if it exists
	if (lobbyHUD)
	{
		lobbyHUD->UpdateUI();
	}
	else
	{
		UManagerLog::LogWarning(TEXT("Lobby HUD is null during UI update"), TEXT("HUDManagerLobby"));
	}
	
	UManagerLog::LogDebug(TEXT("UI updated"), TEXT("HUDManagerLobby"));
}

void UHUDManagerLobby::OnTeamChanged(ETeam newTeam)
{
	UManagerLog::LogInfo(FString::Printf(TEXT("Team changed to: %d"), (int32)newTeam), TEXT("HUDManagerLobby"));
	
	// Validate team value
	if (newTeam == ETeam::ENone)
	{
		UManagerLog::LogWarning(TEXT("Team changed to ENone - hiding customization HUD"), TEXT("HUDManagerLobby"));
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

void UHUDManagerLobby::BindButtonEvents()
{
	if (backButton)
	{
		backButton->OnClicked.AddDynamic(this, &UHUDManagerLobby::OnBackButtonClicked);
		UManagerLog::LogDebug(TEXT("Back button event bound"), TEXT("HUDManagerLobby"));
	}
	else
	{
		UManagerLog::LogError(TEXT("Back button is null - cannot bind event"), TEXT("HUDManagerLobby"));
	}
	
	if (readyButton)
	{
		readyButton->OnClicked.AddDynamic(this, &UHUDManagerLobby::OnReadyButtonClicked);
		UManagerLog::LogDebug(TEXT("Ready button event bound"), TEXT("HUDManagerLobby"));
	}
	else
	{
		UManagerLog::LogError(TEXT("Ready button is null - cannot bind event"), TEXT("HUDManagerLobby"));
	}
} 

void UHUDManagerLobby::FindUIComponents()
{
	// Find UI components using GetWidgetFromName
	// Note: These should be bound via meta = (BindWidget) in Blueprint
	// This method is a fallback in case binding fails
	
	if (!lobbyOverlay)
	{
		UManagerLog::LogWarning(TEXT("Lobby overlay not found via binding - trying GetWidgetFromName"), TEXT("HUDManagerLobby"));
		lobbyOverlay = Cast<UOverlay>(GetWidgetFromName(TEXT("LobbyOverlay")));
	}
	
	if (!customizationOverlay)
	{
		UManagerLog::LogWarning(TEXT("Customization overlay not found via binding - trying GetWidgetFromName"), TEXT("HUDManagerLobby"));
		customizationOverlay = Cast<UOverlay>(GetWidgetFromName(TEXT("CustomizationOverlay")));
	}
	
	if (!backButton)
	{
		UManagerLog::LogWarning(TEXT("Back button not found via binding - trying GetWidgetFromName"), TEXT("HUDManagerLobby"));
		backButton = Cast<UButton>(GetWidgetFromName(TEXT("BackButton")));
	}
	
	if (!readyButton)
	{
		UManagerLog::LogWarning(TEXT("Ready button not found via binding - trying GetWidgetFromName"), TEXT("HUDManagerLobby"));
		readyButton = Cast<UButton>(GetWidgetFromName(TEXT("ReadyButton")));
	}
	
	if (!countdownText)
	{
		UManagerLog::LogWarning(TEXT("Countdown text not found via binding - trying GetWidgetFromName"), TEXT("HUDManagerLobby"));
		countdownText = Cast<UTextBlock>(GetWidgetFromName(TEXT("CountdownText")));
	}
	
	if (!backButtonText)
	{
		UManagerLog::LogWarning(TEXT("Back button text not found via binding - trying GetWidgetFromName"), TEXT("HUDManagerLobby"));
		backButtonText = Cast<UTextBlock>(GetWidgetFromName(TEXT("BackButtonText")));
	}
	
	if (!readyButtonText)
	{
		UManagerLog::LogWarning(TEXT("Ready button text not found via binding - trying GetWidgetFromName"), TEXT("HUDManagerLobby"));
		readyButtonText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ReadyButtonText")));
	}
	
	UManagerLog::LogDebug(TEXT("UI components search completed"), TEXT("HUDManagerLobby"));
} 