#pragma once

// [Meow-Phone Project]
//
// This class is the UI widget for the Cat Customization screen. It provides the player
// with options to change their cat's race, hat, and selected ability.
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_CustomCat`).
// 2. In the UMG editor, you must create all the UI elements (`Button`, `TextBlock`, etc.) and name them to match the `BindWidget` properties below (e.g., a `Button` named `raceLeftButton`, a `TextBlock` named `raceText`).
// 3. This widget is typically displayed during a character customization phase before a match begins, managed by the `UManagerMatch` and created by the `AMPControllerPlayer`.
// 4. The button click events are already bound in C++. When a player clicks a left/right button (e.g., `OnCatRaceLeftClicked`), the C++ logic cycles through the enum values for the customization option.
// 5. After each change, `UpdateCharacterPreview` is called, which then calls a function on the `UManagerPreview` (via the Game Mode) to update the visual appearance of the preview character in the world.
// 6. The `SaveCustomization` function is called to store the final choices in the player's `AMPPlayerState`.
//
// Necessary things to define:
// - All `BindWidget` properties in this header must have corresponding, correctly named widgets in the child Widget Blueprint.
//
// How it interacts with other classes:
// - UMPHUD: The base HUD class.
// - UManagerPreview (via Game Mode): When an option is changed, this UI tells the preview manager to update the character model, showing the new race or hat.
// - AMPPlayerState: When customization is complete, this UI saves the selected enums (`selectedCatRace`, `selectedHat`, etc.) to the player's Player State so the choices are remembered and replicated to others.
// - E... Enums (`ECatRace`, `EHat`, `EAbility`): The UI's core logic cycles through the values of these enums to provide the customization options.

#include "CoreMinimal.h"
#include "MPHUD.h"
#include "HUDCustomCat.generated.h"

/*
 * UHUDCustomCat
 * All preview logic is handled by GameMode and PlayerController.
 * There is no local preview manager or viewport. All preview updates go through the main world system.
 */
UCLASS()
class MEOWPHONE_API UHUDCustomCat : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDCustomCat();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

public:
    // UI Components - Customization Controls
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UPanelWidget* customizationPanel;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* titleText;

    // Race Controls
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* raceLabel;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* raceLeftButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* raceRightButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* raceText;

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
    FString customCatTitleKey = TEXT("CUSTOMIZE_CAT");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString raceLabelKey = TEXT("CAT_RACE");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString hatLabelKey = TEXT("HAT");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString abilityLabelKey = TEXT("ABILITY");

    // Customization state
    UPROPERTY(BlueprintReadOnly, Category = "Customization State")
    ECatRace selectedCatRace;

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
    void OnCatRaceLeftClicked();

    UFUNCTION(BlueprintCallable, Category = "Customization")
    void OnCatRaceRightClicked();

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
    FText GetCatRaceDisplayText(ECatRace catRace) const;

    UFUNCTION(BlueprintCallable, Category = "Customization")
    FText GetHatDisplayText(EHat hat) const;

    UFUNCTION(BlueprintCallable, Category = "Customization")
    FText GetAbilityDisplayText(EAbility ability) const;
};