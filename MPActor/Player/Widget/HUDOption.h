#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MPHUD.h"
#include "CommonEnum.h"
#include "CommonStruct.h"
#include "HUDOption.generated.h"

class UTextBlock;
class UButton;
class UCanvasPanel;

/**
 * HUD for game options and settings
 * Features: Window Mode, Resolution, Graphics, Sync, Language settings
 */
UCLASS()
class MEOWPHONE_API UHUDOption : public UMPHUD
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // Override parent functions
    virtual void UpdateTexts() override;
    virtual bool ValidateRootWidget() override;

    // Set previous HUD (called by other HUDs)
    UFUNCTION(BlueprintCallable, Category = "HUD Navigation")
    void SetPreviousHUD(EHUDType hudType);

private:
    // Root widget
    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* rootCanvas;

    // Window Mode Settings
    UPROPERTY(meta = (BindWidget))
    UTextBlock* windowModeLabel;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* windowModeValue;
    
    UPROPERTY(meta = (BindWidget))
    UButton* windowModeMinusButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* windowModePlusButton;

    // Resolution Settings
    UPROPERTY(meta = (BindWidget))
    UTextBlock* resolutionLabel;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* resolutionValue;
    
    UPROPERTY(meta = (BindWidget))
    UButton* resolutionMinusButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* resolutionPlusButton;

    // Graphics Settings
    UPROPERTY(meta = (BindWidget))
    UTextBlock* graphicsLabel;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* graphicsValue;
    
    UPROPERTY(meta = (BindWidget))
    UButton* graphicsMinusButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* graphicsPlusButton;

    // Sync Settings
    UPROPERTY(meta = (BindWidget))
    UTextBlock* syncLabel;
    
    UPROPERTY(meta = (BindWidget))
    UButton* syncButton;

    // Language Settings
    UPROPERTY(meta = (BindWidget))
    UTextBlock* languageLabel;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* languageValue;
    
    UPROPERTY(meta = (BindWidget))
    UButton* languageMinusButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* languagePlusButton;

    // Navigation Buttons
    UPROPERTY(meta = (BindWidget))
    UButton* backButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* applyButton;

    // Current Settings State
    UPROPERTY()
    EWindowMode currentWindowMode;
    
    UPROPERTY()
    FResolution currentResolution;
    
    UPROPERTY()
    EGraphicsQuality currentGraphicsQuality;
    
    UPROPERTY()
    ESyncMode currentSyncMode;
    
    // Note: Language is managed by Game Instance, not stored locally

    // Previous HUD for navigation
    UPROPERTY()
    EHUDType previousHUD;

    // Available resolutions
    UPROPERTY()
    TArray<FResolution> availableResolutions;

    // Localization keys for settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString windowModeTextKey = TEXT("SETTINGS_WINDOW_MODE");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString windowModeFullscreenTextKey = TEXT("SETTINGS_WINDOW_MODE_FULLSCREEN");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString windowModeFullscreenWindowTextKey = TEXT("SETTINGS_WINDOW_MODE_FULLSCREEN_WINDOW");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString windowModeWindowTextKey = TEXT("SETTINGS_WINDOW_MODE_WINDOW");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString resolutionTextKey = TEXT("SETTINGS_RESOLUTION");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString graphicsTextKey = TEXT("SETTINGS_GRAPHICS");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString graphicsUltraTextKey = TEXT("SETTINGS_GRAPHICS_ULTRA");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString graphicsHighTextKey = TEXT("SETTINGS_GRAPHICS_HIGH");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString graphicsMediumTextKey = TEXT("SETTINGS_GRAPHICS_MEDIUM");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString graphicsLowTextKey = TEXT("SETTINGS_GRAPHICS_LOW");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString syncTextKey = TEXT("SETTINGS_SYNC");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString syncOnTextKey = TEXT("SETTINGS_SYNC_ON");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString syncOffTextKey = TEXT("SETTINGS_SYNC_OFF");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString languageTextKey = TEXT("SETTINGS_LANGUAGE");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString languageEnglishTextKey = TEXT("SETTINGS_LANGUAGE_ENGLISH");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString languageChineseTextKey = TEXT("SETTINGS_LANGUAGE_CHINESE");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString languageSpanishTextKey = TEXT("SETTINGS_LANGUAGE_SPANISH");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString backTextKey = TEXT("SETTINGS_BACK");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString applyTextKey = TEXT("SETTINGS_APPLY");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString settingsAppliedTextKey = TEXT("SETTINGS_APPLIED");

    // Button Click Functions
    UFUNCTION()
    void OnWindowModeMinusClicked();
    
    UFUNCTION()
    void OnWindowModePlusClicked();
    
    UFUNCTION()
    void OnResolutionMinusClicked();
    
    UFUNCTION()
    void OnResolutionPlusClicked();
    
    UFUNCTION()
    void OnGraphicsMinusClicked();
    
    UFUNCTION()
    void OnGraphicsPlusClicked();
    
    UFUNCTION()
    void OnSyncClicked();
    
    UFUNCTION()
    void OnLanguageMinusClicked();
    
    UFUNCTION()
    void OnLanguagePlusClicked();
    
    UFUNCTION()
    void OnBackClicked();
    
    UFUNCTION()
    void OnApplyClicked();

    // Helper Functions
    void InitializeSettings();
    void AutoDetectSettings();
    void UpdateWindowModeDisplay();
    void UpdateResolutionDisplay();
    void UpdateGraphicsDisplay();
    void UpdateSyncDisplay();
    void UpdateLanguageDisplay();
    
    // Apply settings to Unreal Engine
    void ApplyWindowMode();
    void ApplyResolution();
    void ApplyGraphicsQuality();
    void ApplySyncMode();
    void ApplyLanguage();
    
    // Navigation helper
    void NavigateToPreviousHUD();
};