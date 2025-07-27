#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MPHUD.h"
#include "../../../CommonEnum.h"
#include "../../../CommonStruct.h"
#include "GameFramework/GameUserSettings.h" // For EWindowMode::Type
#include "HUDOption.generated.h"

class UTextBlock;
class UButton;
class UCanvasPanel;
class UComboBoxString;
class UCheckBox;

UCLASS()
class MEOWPHONE_API UHUDOption : public UMPHUD
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	virtual void UpdateTexts() override;
	virtual bool ValidateRootWidget() override;
	void SetPreviousHUD(EHUDType hudType);

protected:
	// Widget Bindings
	UPROPERTY(meta = (BindWidget)) UCanvasPanel* rootCanvas;
	UPROPERTY(meta = (BindWidget)) UTextBlock* titleText;
	UPROPERTY(meta = (BindWidget)) UTextBlock* windowModeLabel;
	UPROPERTY(meta = (BindWidget)) UComboBoxString* windowModeComboBox;
	UPROPERTY(meta = (BindWidget)) UTextBlock* resolutionLabel;
	UPROPERTY(meta = (BindWidget)) UComboBoxString* resolutionComboBox;
	UPROPERTY(meta = (BindWidget)) UTextBlock* graphicsLabel;
	UPROPERTY(meta = (BindWidget)) UComboBoxString* graphicsQualityComboBox;
	UPROPERTY(meta = (BindWidget)) UTextBlock* syncLabel;
	UPROPERTY(meta = (BindWidget)) UCheckBox* vsyncCheckBox;
	UPROPERTY(meta = (BindWidget)) UTextBlock* languageLabel;
	UPROPERTY(meta = (BindWidget)) UComboBoxString* languageComboBox;
	UPROPERTY(meta = (BindWidget)) UButton* backButton;
	UPROPERTY(meta = (BindWidget)) UButton* applyButton;
	UPROPERTY(meta = (BindWidget)) UButton* resetButton;

	// Delegate Handlers
	UFUNCTION() void OnBackClicked();
	UFUNCTION() void OnApplyClicked();
	UFUNCTION() void OnResetClicked();
	UFUNCTION() void OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnGraphicsQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnVSyncChanged(bool bIsChecked);
	UFUNCTION() void OnLanguageChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	// Helper Functions
	void LoadCurrentSettings();
	void LoadDefaultSettings();
	void ApplyCurrentSettings();
	void SaveSettings();
	void PopulateResolutions();
	void UpdateUIFromSettings();
	void ApplyLanguageChange(const FString& languageString);
	void NavigateToPreviousHUD();

	// String/Enum Conversion Helpers
	FString GetWindowModeString(EWindowMode::Type InWindowMode);
	EWindowMode::Type GetWindowModeFromString(const FString& InString);
	FString GetGraphicsQualityString(int32 InQuality);
	int32 GetGraphicsQualityFromString(const FString& InString);
	FString GetLanguageString(ELanguage InLanguage);
	ELanguage GetLanguageFromString(const FString& InString);
	FIntPoint GetResolutionFromString(const FString& InString);

	// State
	UPROPERTY() EHUDType previousHUD;
	UPROPERTY() TArray<FIntPoint> availableResolutions;

	// Localization Keys
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys") FString titleTextKey = TEXT("SETTINGS_TITLE");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys") FString windowModeTextKey = TEXT("SETTINGS_WINDOW_MODE");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys") FString resolutionTextKey = TEXT("SETTINGS_RESOLUTION");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys") FString graphicsTextKey = TEXT("SETTINGS_GRAPHICS");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys") FString syncTextKey = TEXT("SETTINGS_SYNC");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys") FString languageTextKey = TEXT("SETTINGS_LANGUAGE");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys") FString backTextKey = TEXT("SETTINGS_BACK");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys") FString applyTextKey = TEXT("SETTINGS_APPLY");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys") FString resetTextKey = TEXT("SETTINGS_RESET");
};