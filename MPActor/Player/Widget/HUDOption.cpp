#include "HUDOption.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "../../../HighLevel/MPGI.h"
#include "../../../HighLevel/Managers/ManagerLog.h"
#include "../../../HighLevel/Managers/ManagerLocalization.h"
#include "../MPControllerPlayer.h"

void UHUDOption::NativeConstruct()
{
	Super::NativeConstruct();

	if (!ValidateRootWidget())
	{
		UManagerLog::LogError(TEXT("Root widget validation failed! Option screen will not work."), TEXT("HUDOption"));
		return;
	}

	// Bind button events
	if (backButton) backButton->OnClicked.AddDynamic(this, &UHUDOption::OnBackClicked);
	if (applyButton) applyButton->OnClicked.AddDynamic(this, &UHUDOption::OnApplyClicked);
	if (resetButton) resetButton->OnClicked.AddDynamic(this, &UHUDOption::OnResetClicked);

	// Bind input events for ComboBoxes and CheckBox
	if (windowModeComboBox) windowModeComboBox->OnSelectionChanged.AddDynamic(this, &UHUDOption::OnWindowModeChanged);
	if (resolutionComboBox) resolutionComboBox->OnSelectionChanged.AddDynamic(this, &UHUDOption::OnResolutionChanged);
	if (graphicsQualityComboBox) graphicsQualityComboBox->OnSelectionChanged.AddDynamic(this, &UHUDOption::OnGraphicsQualityChanged);
	if (vsyncCheckBox) vsyncCheckBox->OnCheckStateChanged.AddDynamic(this, &UHUDOption::OnVSyncChanged);
	if (languageComboBox) languageComboBox->OnSelectionChanged.AddDynamic(this, &UHUDOption::OnLanguageChanged);

	// Populate and load settings
	PopulateResolutions();
	LoadCurrentSettings();
	UpdateTexts();
}

void UHUDOption::NativeDestruct()
{
	if (backButton) backButton->OnClicked.RemoveAll(this);
	if (applyButton) applyButton->OnClicked.RemoveAll(this);
	if (resetButton) resetButton->OnClicked.RemoveAll(this);
	if (windowModeComboBox) windowModeComboBox->OnSelectionChanged.RemoveAll(this);
	if (resolutionComboBox) resolutionComboBox->OnSelectionChanged.RemoveAll(this);
	if (graphicsQualityComboBox) graphicsQualityComboBox->OnSelectionChanged.RemoveAll(this);
	if (vsyncCheckBox) vsyncCheckBox->OnCheckStateChanged.RemoveAll(this);
	if (languageComboBox) languageComboBox->OnSelectionChanged.RemoveAll(this);

	Super::NativeDestruct();
}

void UHUDOption::UpdateTexts()
{
	Super::UpdateTexts();
	if (!ValidateRootWidget()) return;

	titleText->SetText(GetLocalizedText(titleTextKey));
	windowModeLabel->SetText(GetLocalizedText(windowModeTextKey));
	resolutionLabel->SetText(GetLocalizedText(resolutionTextKey));
	graphicsLabel->SetText(GetLocalizedText(graphicsTextKey));
	syncLabel->SetText(GetLocalizedText(syncTextKey));
	languageLabel->SetText(GetLocalizedText(languageTextKey));

	// Buttons
	if (UTextBlock* txt = Cast<UTextBlock>(backButton->GetChildAt(0))) txt->SetText(GetLocalizedText(backTextKey));
	if (UTextBlock* txt = Cast<UTextBlock>(applyButton->GetChildAt(0))) txt->SetText(GetLocalizedText(applyTextKey));
	if (UTextBlock* txt = Cast<UTextBlock>(resetButton->GetChildAt(0))) txt->SetText(GetLocalizedText(resetTextKey));
}

bool UHUDOption::ValidateRootWidget()
{
	return rootCanvas != nullptr;
}

void UHUDOption::SetPreviousHUD(EHUDType hudType)
{
	previousHUD = hudType;
}

void UHUDOption::LoadCurrentSettings()
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (!UserSettings) return;

	// Populate ComboBox options before setting selected ones
	// Window Mode
	windowModeComboBox->ClearOptions();
	windowModeComboBox->AddOption(GetWindowModeString(EWindowMode::Fullscreen));
	windowModeComboBox->AddOption(GetWindowModeString(EWindowMode::WindowedFullscreen));
	windowModeComboBox->AddOption(GetWindowModeString(EWindowMode::Windowed));

	// Graphics
	graphicsQualityComboBox->ClearOptions();
	graphicsQualityComboBox->AddOption(GetGraphicsQualityString(0)); // Low
	graphicsQualityComboBox->AddOption(GetGraphicsQualityString(1)); // Medium
	graphicsQualityComboBox->AddOption(GetGraphicsQualityString(2)); // High
	graphicsQualityComboBox->AddOption(GetGraphicsQualityString(3)); // Ultra

	// Language
	UMPGI* GI = GetGameInstance<UMPGI>();
	if (GI && GI->GetLocalizationManager())
	{
		languageComboBox->ClearOptions();
		languageComboBox->AddOption(GetLanguageString(ELanguage::EEnglish));
		languageComboBox->AddOption(GetLanguageString(ELanguage::EChinese));
		languageComboBox->AddOption(GetLanguageString(ELanguage::ESpanish));
	}

	UpdateUIFromSettings();
}

void UHUDOption::LoadDefaultSettings()
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (!UserSettings) return;

	UserSettings->SetToDefaults();
	UserSettings->ApplySettings(false);
	LoadCurrentSettings();
}

void UHUDOption::ApplyCurrentSettings()
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (!UserSettings) return;

	// Window Mode
	EWindowMode::Type NewWindowMode = GetWindowModeFromString(windowModeComboBox->GetSelectedOption());
	UserSettings->SetFullscreenMode(NewWindowMode);

	// Resolution
	FIntPoint NewResolution = GetResolutionFromString(resolutionComboBox->GetSelectedOption());
	UserSettings->SetScreenResolution(NewResolution);

	// Graphics
	int32 NewQuality = GetGraphicsQualityFromString(graphicsQualityComboBox->GetSelectedOption());
	UserSettings->SetOverallScalabilityLevel(NewQuality);

	// VSync
	UserSettings->SetVSyncEnabled(vsyncCheckBox->IsChecked());

	// Apply all settings
	UserSettings->ApplySettings(false);

	// Language is applied instantly, but save it with other settings
	SaveSettings();
}

void UHUDOption::SaveSettings()
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (UserSettings)
	{
		UserSettings->SaveSettings();
	}
	UMPGI* GI = GetGameInstance<UMPGI>();
	if (GI)
	{
		GI->SaveGame(); // Saves language preference
	}
}

void UHUDOption::PopulateResolutions()
{
	FDisplayMetrics DisplayMetrics;
	FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);
	
	const FMonitorInfo& Monitor = DisplayMetrics.MonitorInfo[0]; // Assuming primary monitor
	
	// A basic list of common resolutions, you can expand this
	availableResolutions = { {1280, 720}, {1600, 900}, {1920, 1080}, {2560, 1440}, {3840, 2160} };

	resolutionComboBox->ClearOptions();
	for (const FIntPoint& Res : availableResolutions)
	{
		resolutionComboBox->AddOption(FString::Printf(TEXT("%d x %d"), Res.X, Res.Y));
	}
}

void UHUDOption::UpdateUIFromSettings()
{
	UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
	if (!UserSettings) return;

	// Window Mode
	windowModeComboBox->SetSelectedOption(GetWindowModeString(UserSettings->GetFullscreenMode()));

	// Resolution
	resolutionComboBox->SetSelectedOption(FString::Printf(TEXT("%d x %d"), UserSettings->GetScreenResolution().X, UserSettings->GetScreenResolution().Y));

	// Graphics
	graphicsQualityComboBox->SetSelectedOption(GetGraphicsQualityString(UserSettings->GetOverallScalabilityLevel()));

	// VSync
	vsyncCheckBox->SetIsChecked(UserSettings->IsVSyncEnabled());

	// Language
	UMPGI* GI = GetGameInstance<UMPGI>();
	if (GI)
	{
		languageComboBox->SetSelectedOption(GetLanguageString(GI->GetCurrentLanguage()));
	}
}

void UHUDOption::NavigateToPreviousHUD()
{
	if (owner)
	{
		owner->RemoveHUD(EHUDType::EOption);
		owner->AttachHUD(previousHUD, 0);
	}
}

void UHUDOption::OnBackClicked()
{
	NavigateToPreviousHUD();
}

void UHUDOption::OnApplyClicked()
{
	ApplyCurrentSettings();
}

void UHUDOption::OnResetClicked()
{
	LoadDefaultSettings();
}

void UHUDOption::OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	// Can add logic here to apply instantly if desired
}

void UHUDOption::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	// Can add logic here to apply instantly if desired
}

void UHUDOption::OnGraphicsQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	// Can add logic here to apply instantly if desired
}

void UHUDOption::OnVSyncChanged(bool bIsChecked)
{
	// Can add logic here to apply instantly if desired
}

void UHUDOption::OnLanguageChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	ApplyLanguageChange(SelectedItem);
}

void UHUDOption::ApplyLanguageChange(const FString& languageString)
{
	ELanguage NewLanguage = GetLanguageFromString(languageString);
	UMPGI* GI = GetGameInstance<UMPGI>();
	if (GI)
	{
		GI->SetCurrentLanguage(NewLanguage);
		UpdateTexts(); // Refresh HUD text after language change
	}
}

// String Conversion Helpers
FString UHUDOption::GetWindowModeString(EWindowMode::Type InWindowMode)
{
	switch (InWindowMode)
	{
		case EWindowMode::Fullscreen: return TEXT("Fullscreen");
		case EWindowMode::WindowedFullscreen: return TEXT("Windowed Fullscreen");
		case EWindowMode::Windowed: return TEXT("Windowed");
	}
	return TEXT("");
}

EWindowMode::Type UHUDOption::GetWindowModeFromString(const FString& InString)
{
	if (InString.Equals(TEXT("Fullscreen"))) return EWindowMode::Fullscreen;
	if (InString.Equals(TEXT("Windowed Fullscreen"))) return EWindowMode::WindowedFullscreen;
	if (InString.Equals(TEXT("Windowed"))) return EWindowMode::Windowed;
	return EWindowMode::Fullscreen;
}

FString UHUDOption::GetGraphicsQualityString(int32 InQuality)
{
	switch (InQuality)
	{
		case 0: return TEXT("Low");
		case 1: return TEXT("Medium");
		case 2: return TEXT("High");
		case 3: return TEXT("Ultra");
	}
	return TEXT("High");
}

int32 UHUDOption::GetGraphicsQualityFromString(const FString& InString)
{
	if (InString.Equals(TEXT("Low"))) return 0;
	if (InString.Equals(TEXT("Medium"))) return 1;
	if (InString.Equals(TEXT("High"))) return 2;
	if (InString.Equals(TEXT("Ultra"))) return 3;
	return 2;
}

FString UHUDOption::GetLanguageString(ELanguage InLanguage)
{
	switch (InLanguage)
	{
		case ELanguage::EEnglish: return TEXT("English");
		case ELanguage::EChinese: return TEXT("Chinese");
		case ELanguage::ESpanish: return TEXT("Spanish");
	}
	return TEXT("English");
}

ELanguage UHUDOption::GetLanguageFromString(const FString& InString)
{
	if (InString.Equals(TEXT("Chinese"))) return ELanguage::EChinese;
	if (InString.Equals(TEXT("Spanish"))) return ELanguage::ESpanish;
	return ELanguage::EEnglish;
}

FIntPoint UHUDOption::GetResolutionFromString(const FString& InString)
{
	TArray<FString> Parts;
	InString.ParseIntoArray(Parts, TEXT("x"), true);
	if (Parts.Num() == 2)
	{
		return FIntPoint(FCString::Atoi(*Parts[0]), FCString::Atoi(*Parts[1]));
	}
	return FIntPoint(1920, 1080);
}