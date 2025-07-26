#include "HUDOption.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableTextBox.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/ComboBoxString.h"
#include "Components/ScrollBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/ScaleBox.h"
#include "Components/AspectRatioBox.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/ScrollPanel.h"
#include "HighLevel/MPGI.h"
#include "HighLevel/MPLogManager.h"
#include "MPActor/Player/MPControllerPlayer.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/UserInterfaceSettings.h"
#include "Rendering/SlateRenderer.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "GenericPlatform/GenericPlatformApplicationMisc.h"
#include "Engine/Engine.h"
#include "Engine/GameUserSettings.h"
#include "HAL/IConsoleManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Scalability.h"

void UHUDOption::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Validate root widget first
    if (!ValidateRootWidget())
    {
		UMPLogManager::LogError(TEXT("Root widget validation failed!"), TEXT("HUDOption"));
        return;
    }
    
    // Update texts after construction
    UpdateTexts();
    
	// Bind button events
    if (backButton)
    {
        backButton->OnClicked.AddDynamic(this, &UHUDOption::OnBackClicked);
    }
    
    if (applyButton)
    {
        applyButton->OnClicked.AddDynamic(this, &UHUDOption::OnApplyClicked);
    }

	if (resetButton)
{
		resetButton->OnClicked.AddDynamic(this, &UHUDOption::OnResetClicked);
    }
    
	// Bind input events
	if (windowModeComboBox)
    {
		windowModeComboBox->OnSelectionChanged.AddDynamic(this, &UHUDOption::OnWindowModeChanged);
	}
	
	if (resolutionComboBox)
	{
		resolutionComboBox->OnSelectionChanged.AddDynamic(this, &UHUDOption::OnResolutionChanged);
	}
	
	if (graphicsQualityComboBox)
	{
		graphicsQualityComboBox->OnSelectionChanged.AddDynamic(this, &UHUDOption::OnGraphicsQualityChanged);
	}
	
	if (vsyncCheckBox)
	{
		vsyncCheckBox->OnCheckStateChanged.AddDynamic(this, &UHUDOption::OnVSyncChanged);
	}
	
	if (languageComboBox)
	{
		languageComboBox->OnSelectionChanged.AddDynamic(this, &UHUDOption::OnLanguageChanged);
	}
	
	// Load current settings
	LoadCurrentSettings();
}

void UHUDOption::NativeDestruct()
{
	// Unbind button events
    if (backButton)
    {
        backButton->OnClicked.RemoveAll(this);
    }
    
    if (applyButton)
    {
        applyButton->OnClicked.RemoveAll(this);
    }
	
	if (resetButton)
	{
		resetButton->OnClicked.RemoveAll(this);
	}
	
	// Unbind input events
	if (windowModeComboBox)
	{
		windowModeComboBox->OnSelectionChanged.RemoveAll(this);
	}
	
	if (resolutionComboBox)
	{
		resolutionComboBox->OnSelectionChanged.RemoveAll(this);
	}
	
	if (graphicsQualityComboBox)
	{
		graphicsQualityComboBox->OnSelectionChanged.RemoveAll(this);
	}
	
	if (vsyncCheckBox)
	{
		vsyncCheckBox->OnCheckStateChanged.RemoveAll(this);
	}
	
	if (languageComboBox)
	{
		languageComboBox->OnSelectionChanged.RemoveAll(this);
    }
    
    Super::NativeDestruct();
}

void UHUDOption::UpdateTexts()
{
    Super::UpdateTexts();
    
	// Update title
	if (titleText)
	{
		titleText->SetText(GetLocalizedText(titleTextKey));
	}
	
	// Update button texts
	if (backButton)
	{
		UTextBlock* buttonText = Cast<UTextBlock>(backButton->GetChildAt(0));
		if (buttonText)
		{
			buttonText->SetText(GetLocalizedText(backTextKey));
		}
	}
	
	if (applyButton)
	{
		UTextBlock* buttonText = Cast<UTextBlock>(applyButton->GetChildAt(0));
		if (buttonText)
		{
			buttonText->SetText(GetLocalizedText(applyTextKey));
		}
	}
	
	if (resetButton)
	{
		UTextBlock* buttonText = Cast<UTextBlock>(resetButton->GetChildAt(0));
		if (buttonText)
		{
			buttonText->SetText(GetLocalizedText(resetTextKey));
		}
	}
}

bool UHUDOption::ValidateRootWidget()
{
    if (!rootCanvas)
    {
		UMPLogManager::LogError(TEXT("Root canvas is missing!"), TEXT("HUDOption"));
        return false;
    }
    return true;
}

void UHUDOption::SetPreviousHUD(EHUDType hudType)
{
    previousHUD = hudType;
	UMPLogManager::LogInfo(FString::Printf(TEXT("Previous HUD set to %d"), (int32)hudType), TEXT("HUDOption"));
}

void UHUDOption::LoadCurrentSettings()
{
	// Get user settings from game instance
	UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
	if (!gameInstance)
	{
		UMPLogManager::LogWarning(TEXT("Could not get user settings, using defaults"), TEXT("HUDOption"));
		LoadDefaultSettings();
        return;
    }
    
	// Load window mode
	EWindowMode::Type currentEngineMode = EWindowMode::Fullscreen;
	if (GEngine && GEngine->GameViewport)
	{
		currentEngineMode = GEngine->GameViewport->GetWindow()->GetWindowMode();
	}
	
	// Validate window mode
	if (currentEngineMode < EWindowMode::Fullscreen || currentEngineMode > EWindowMode::WindowedFullscreen)
	{
		UMPLogManager::LogWarning(FString::Printf(TEXT("Invalid window mode detected (%d), using FullScreen"), currentEngineMode), TEXT("HUDOption"));
		currentEngineMode = EWindowMode::Fullscreen;
	}
	
	// Load resolution
	FIntPoint currentResolution = FIntPoint(1920, 1080);
	if (GEngine && GEngine->GameViewport)
	{
		currentResolution = GEngine->GameViewport->GetWindow()->GetSizeInScreen();
	}
	
	// Load graphics quality
	int32 currentQuality = 2; // Default to High
	if (GEngine)
	{
		// Use Scalability to get current quality
		Scalability::FQualityLevels QualityLevels = Scalability::GetQualityLevels();
		// We'll use the average of the main settings as a simple approach
		int32 sum = QualityLevels.ResolutionQuality + QualityLevels.ViewDistanceQuality + QualityLevels.AntiAliasingQuality + QualityLevels.ShadowQuality + QualityLevels.PostProcessQuality + QualityLevels.TextureQuality + QualityLevels.EffectsQuality + QualityLevels.FoliageQuality;
		int32 avg = sum / 8;
		if (avg < 25) currentQuality = 0; // Low
		else if (avg < 50) currentQuality = 1; // Medium
		else if (avg < 75) currentQuality = 2; // High
		else currentQuality = 3; // Ultra
	}
	
	// Load vsync
	bool vsyncEnabled = true;
	if (GEngine && GEngine->GameViewport)
	{
		vsyncEnabled = GEngine->GameViewport->GetWindow()->IsVSyncEnabled();
	}
	
	// Load language
	ELanguage currentLanguage = ELanguage::EEnglish;
	if (gameInstance->GetLocalizationManager())
	{
		currentLanguage = gameInstance->GetLocalizationManager()->GetCurrentLanguage();
	}
	
	UMPLogManager::LogInfo(FString::Printf(TEXT("Auto-detected settings - Window: %d, Res: %dx%d, Graphics: %d, Sync: %d"),
		currentEngineMode, currentResolution.X, currentResolution.Y, currentQuality, vsyncEnabled ? 1 : 0), TEXT("HUDOption"));
	
	// Update UI components
	UpdateUIComponents(currentEngineMode, currentResolution, currentQuality, vsyncEnabled, currentLanguage);
}

void UHUDOption::UpdateUIComponents(EWindowMode::Type windowMode, FIntPoint resolution, int32 graphicsQuality, bool vsync, ELanguage language)
{
	// Update window mode combo box
	if (windowModeComboBox)
	{
		FString windowModeString = GetWindowModeString(windowMode);
		windowModeComboBox->SetSelectedOption(windowModeString);
	}
	
	// Update resolution combo box
	if (resolutionComboBox)
	{
		FString resolutionString = FString::Printf(TEXT("%dx%d"), resolution.X, resolution.Y);
		resolutionComboBox->SetSelectedOption(resolutionString);
	}
	
	// Update graphics quality combo box
	if (graphicsQualityComboBox)
	{
		FString qualityString = GetGraphicsQualityString(graphicsQuality);
		graphicsQualityComboBox->SetSelectedOption(qualityString);
	}
	
	// Update vsync check box
	if (vsyncCheckBox)
	{
		vsyncCheckBox->SetIsChecked(vsync);
	}
	
	// Update language combo box
	if (languageComboBox)
	{
		FString languageString = GetLanguageString(language);
		languageComboBox->SetSelectedOption(languageString);
	}
}

void UHUDOption::OnBackClicked()
{
	UMPLogManager::LogInfo(TEXT("Back clicked"), TEXT("HUDOption"));
	
	if (!owner)
	{
		UMPLogManager::LogError(TEXT("Owner is null, cannot navigate"), TEXT("HUDOption"));
		return;
	}
	
	// Remove current HUD and add previous HUD
	owner->RemoveHUD(EHUDType::EOption);
	owner->AttachHUD(previousHUD, 0);
	
	UMPLogManager::LogInfo(FString::Printf(TEXT("Navigated back to HUD %d"), (int32)previousHUD), TEXT("HUDOption"));
}

void UHUDOption::OnApplyClicked()
{
	UMPLogManager::LogInfo(TEXT("Apply clicked"), TEXT("HUDOption"));
	
	// Apply current settings
	ApplyCurrentSettings();
}

void UHUDOption::OnResetClicked()
{
	UMPLogManager::LogInfo(TEXT("Reset clicked"), TEXT("HUDOption"));
	
	// Reset to default settings
	LoadDefaultSettings();
}

void UHUDOption::OnWindowModeChanged(FString selectedItem, ESelectInfo::Type selectionType)
{
	UMPLogManager::LogDebug(FString::Printf(TEXT("Window mode changed to: %s"), *selectedItem), TEXT("HUDOption"));
}

void UHUDOption::OnResolutionChanged(FString selectedItem, ESelectInfo::Type selectionType)
{
	UMPLogManager::LogDebug(FString::Printf(TEXT("Resolution changed to: %s"), *selectedItem), TEXT("HUDOption"));
}

void UHUDOption::OnGraphicsQualityChanged(FString selectedItem, ESelectInfo::Type selectionType)
{
	UMPLogManager::LogDebug(FString::Printf(TEXT("Graphics quality changed to: %s"), *selectedItem), TEXT("HUDOption"));
}

void UHUDOption::OnVSyncChanged(bool isChecked)
{
	UMPLogManager::LogDebug(FString::Printf(TEXT("VSync changed to: %s"), isChecked ? TEXT("On") : TEXT("Off")), TEXT("HUDOption"));
}

void UHUDOption::OnLanguageChanged(FString selectedItem, ESelectInfo::Type selectionType)
{
	UMPLogManager::LogDebug(FString::Printf(TEXT("Language changed to: %s"), *selectedItem), TEXT("HUDOption"));
	
	// Apply language change immediately
	ApplyLanguageChange(selectedItem);
}

void UHUDOption::ApplyLanguageChange(const FString& languageString)
{
	ELanguage newLanguage = GetLanguageFromString(languageString);
	
	UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
	if (gameInstance && gameInstance->GetLocalizationManager())
	{
		gameInstance->GetLocalizationManager()->SetCurrentLanguage(newLanguage);
		UMPLogManager::LogInfo(TEXT("Language already applied immediately"), TEXT("HUDOption"));
	}
}

void UHUDOption::ApplyCurrentSettings()
{
	// Get current UI values
	EWindowMode::Type windowMode = GetWindowModeFromUI();
	FIntPoint resolution = GetResolutionFromUI();
	int32 graphicsQuality = GetGraphicsQualityFromUI();
	bool vsync = GetVSyncFromUI();
	
	// Apply window mode
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetWindow()->SetWindowMode(windowMode);
		UMPLogManager::LogInfo(FString::Printf(TEXT("Applied window mode: %d"), windowMode), TEXT("HUDOption"));
	}
	else
	{
		UMPLogManager::LogError(TEXT("Could not get user settings for window mode"), TEXT("HUDOption"));
	}
	
	// Apply resolution
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetWindow()->Resize(resolution);
		UMPLogManager::LogInfo(FString::Printf(TEXT("Applied resolution: %dx%d"), resolution.X, resolution.Y), TEXT("HUDOption"));
	}
	else
	{
		UMPLogManager::LogError(TEXT("Could not get user settings for resolution"), TEXT("HUDOption"));
	}
	
	// Apply graphics quality
	if (GEngine)
	{
		Scalability::FQualityLevels QualityLevels;
		// Map UI value to all quality settings
		int32 value = 0;
		if (graphicsQuality == 0) value = 0; // Low
		else if (graphicsQuality == 1) value = 25; // Medium
		else if (graphicsQuality == 2) value = 50; // High
		else value = 75; // Ultra
		QualityLevels.ResolutionQuality = value;
		QualityLevels.ViewDistanceQuality = value;
		QualityLevels.AntiAliasingQuality = value;
		QualityLevels.ShadowQuality = value;
		QualityLevels.PostProcessQuality = value;
		QualityLevels.TextureQuality = value;
		QualityLevels.EffectsQuality = value;
		QualityLevels.FoliageQuality = value;
		Scalability::SetQualityLevels(QualityLevels);
		UMPLogManager::LogInfo(FString::Printf(TEXT("Applied graphics quality: %d"), graphicsQuality), TEXT("HUDOption"));
	}
	
	// Apply vsync
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetWindow()->SetVSyncEnabled(vsync);
		UMPLogManager::LogInfo(FString::Printf(TEXT("Applied sync mode: %s"), vsync ? TEXT("On") : TEXT("Off")), TEXT("HUDOption"));
	}
	else
	{
		UMPLogManager::LogError(TEXT("Could not get user settings for sync mode"), TEXT("HUDOption"));
	}
	
	// Save settings
	SaveSettings();
}

void UHUDOption::LoadDefaultSettings()
{
	// Set default values
	EWindowMode::Type defaultWindowMode = EWindowMode::Fullscreen;
	FIntPoint defaultResolution = FIntPoint(1920, 1080);
	int32 defaultGraphicsQuality = 2; // High
	bool defaultVSync = true;
	ELanguage defaultLanguage = ELanguage::EEnglish;
	
	// Update UI components
	UpdateUIComponents(defaultWindowMode, defaultResolution, defaultGraphicsQuality, defaultVSync, defaultLanguage);
}

void UHUDOption::SaveSettings()
{
    UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
    if (gameInstance)
    {
		gameInstance->SaveGame();
	}
}

// Helper functions for UI conversion
EWindowMode::Type UHUDOption::GetWindowModeFromUI()
{
	if (windowModeComboBox)
	{
		FString selectedOption = windowModeComboBox->GetSelectedOption();
		return GetWindowModeFromString(selectedOption);
	}
	return EWindowMode::Fullscreen;
}

FIntPoint UHUDOption::GetResolutionFromUI()
{
	if (resolutionComboBox)
	{
		FString selectedOption = resolutionComboBox->GetSelectedOption();
		return GetResolutionFromString(selectedOption);
	}
	return FIntPoint(1920, 1080);
}

int32 UHUDOption::GetGraphicsQualityFromUI()
{
	if (graphicsQualityComboBox)
	{
		FString selectedOption = graphicsQualityComboBox->GetSelectedOption();
		return GetGraphicsQualityFromString(selectedOption);
	}
	return 2; // High
}

bool UHUDOption::GetVSyncFromUI()
{
	if (vsyncCheckBox)
	{
		return vsyncCheckBox->IsChecked();
	}
	return true;
}

// String conversion helpers
FString UHUDOption::GetWindowModeString(EWindowMode::Type windowMode)
{
	switch (windowMode)
	{
	case EWindowMode::Fullscreen:
		return TEXT("Fullscreen");
	case EWindowMode::WindowedFullscreen:
		return TEXT("Borderless");
	case EWindowMode::Windowed:
		return TEXT("Windowed");
	default:
		return TEXT("Fullscreen");
	}
}

EWindowMode::Type UHUDOption::GetWindowModeFromString(const FString& windowModeString)
{
	if (windowModeString == TEXT("Fullscreen"))
		return EWindowMode::Fullscreen;
	else if (windowModeString == TEXT("Borderless"))
		return EWindowMode::WindowedFullscreen;
	else if (windowModeString == TEXT("Windowed"))
		return EWindowMode::Windowed;
	else
		return EWindowMode::Fullscreen;
}

FString UHUDOption::GetGraphicsQualityString(int32 quality)
{
	switch (quality)
	{
	case 0:
		return TEXT("Low");
	case 1:
		return TEXT("Medium");
	case 2:
		return TEXT("High");
	case 3:
		return TEXT("Ultra");
	default:
		return TEXT("High");
	}
}

int32 UHUDOption::GetGraphicsQualityFromString(const FString& qualityString)
{
	if (qualityString == TEXT("Low"))
		return 0;
	else if (qualityString == TEXT("Medium"))
		return 1;
	else if (qualityString == TEXT("High"))
		return 2;
	else if (qualityString == TEXT("Ultra"))
		return 3;
	else
		return 2;
}

FString UHUDOption::GetLanguageString(ELanguage language)
{
	switch (language)
	{
	case ELanguage::EEnglish:
		return TEXT("English");
	case ELanguage::EChinese:
		return TEXT("中文");
	case ELanguage::EJapanese:
		return TEXT("日本語");
	default:
		return TEXT("English");
	}
}

ELanguage UHUDOption::GetLanguageFromString(const FString& languageString)
{
	if (languageString == TEXT("English"))
		return ELanguage::EEnglish;
	else if (languageString == TEXT("中文"))
		return ELanguage::EChinese;
	else if (languageString == TEXT("日本語"))
		return ELanguage::EJapanese;
	else
		return ELanguage::EEnglish;
}

FIntPoint UHUDOption::GetResolutionFromString(const FString& resolutionString)
{
	FString left, right;
	if (resolutionString.Split(TEXT("x"), &left, &right))
	{
		int32 width = FCString::Atoi(*left);
		int32 height = FCString::Atoi(*right);
		return FIntPoint(width, height);
	}
	return FIntPoint(1920, 1080);
}