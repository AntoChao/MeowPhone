#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"
#include "HUDCustomHuman.generated.h"

/*
 * UHUDCustomHuman
 * All preview logic is handled by GameMode and PlayerController.
 * There is no local preview manager or viewport. All preview updates go through the main world system.
 */
UCLASS()
class MEOWPHONE_API UHUDCustomHuman : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDCustomHuman();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

public:
    // UI Components - Customization Controls
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UPanelWidget* customizationPanel;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* titleText;

    // Character Controls
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* characterLabel;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* characterLeftButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* characterRightButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* characterText;

    // Hat Controls
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* hatLabel;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* hatLeftButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* hatRightButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* hatText;

    // Ability Controls
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* abilityLabel;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* abilityLeftButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* abilityRightButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* abilityText;

    // Localization keys
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString customHumanTitleKey = TEXT("CUSTOMIZE_HUMAN");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString characterLabelKey = TEXT("HUMAN_CHARACTER");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString hatLabelKey = TEXT("HAT");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString abilityLabelKey = TEXT("ABILITY");

    // Customization state
    UPROPERTY(BlueprintReadOnly, Category = "Customization State")
    EHumanProfession selectedHumanProfession;

    UPROPERTY(BlueprintReadOnly, Category = "Customization State")
    EHat selectedHat;

    UPROPERTY(BlueprintReadOnly, Category = "Customization State")
    EAbility selectedAbility;

public:
    // Update customization options
    UFUNCTION(BlueprintCallable, Category = "Customization")
    void UpdateCustomizationOptions();

    // Update character preview
    UFUNCTION(BlueprintCallable, Category = "Customization")
    void UpdateCharacterPreview();

    // Save customization to player state
    UFUNCTION(BlueprintCallable, Category = "Customization")
    void SaveCustomization();

    // Load customization from player state
    UFUNCTION(BlueprintCallable, Category = "Customization")
    void LoadCustomization();

    // Get current team from player controller
    UFUNCTION(BlueprintCallable, Category = "Customization")
    ETeam GetCurrentTeam() const;

protected:
    // Button click handlers
    UFUNCTION(BlueprintCallable, Category = "Customization")
    void OnHumanProfessionLeftClicked();

    UFUNCTION(BlueprintCallable, Category = "Customization")
    void OnHumanProfessionRightClicked();

    UFUNCTION(BlueprintCallable, Category = "Customization")
    void OnHatLeftClicked();

    UFUNCTION(BlueprintCallable, Category = "Customization")
    void OnHatRightClicked();

    UFUNCTION(BlueprintCallable, Category = "Customization")
    void OnAbilityLeftClicked();

    UFUNCTION(BlueprintCallable, Category = "Customization")
    void OnAbilityRightClicked();

    // Update UI
    UFUNCTION(BlueprintCallable, Category = "Customization")
    void UpdateUI();

    // Get display text for enums
    UFUNCTION(BlueprintCallable, Category = "Customization")
    FText GetHumanProfessionDisplayText(EHumanProfession profession) const;

    UFUNCTION(BlueprintCallable, Category = "Customization")
    FText GetHatDisplayText(EHat hat) const;

    UFUNCTION(BlueprintCallable, Category = "Customization")
    FText GetAbilityDisplayText(EAbility ability) const;
};