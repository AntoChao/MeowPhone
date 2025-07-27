#pragma once

// [Meow-Phone Project]
//
// This class is the UI widget for the in-game Pause Menu. It appears when the player
// pauses the game during a match and provides options to continue, open the settings menu,
// or quit back to the main menu.
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_PauseMenu`).
// 2. In the UMG editor, you must create the `Button` and `TextBlock` widgets and name them to match the `BindWidget` properties below (e.g., `continueButton`, `optionText`).
// 3. This widget is created and managed by the `AMPControllerPlayer`. It is typically shown when the player presses an "Open Menu" input action during gameplay.
// 4. The button click events are bound in C++.
//    - `OnContinueClicked` removes this widget, returning to the game.
//    - `OnOptionClicked` tells the Player Controller to show the Options widget.
//    - `OnQuitClicked` tells the Game Instance to end the session and return to the main menu level.
//
// Necessary things to define:
// - All `BindWidget` properties MUST have corresponding, correctly named widgets in the child Widget Blueprint.
//
// How it interacts with other classes:
// - UMPHUD: The base HUD class.
// - AMPControllerPlayer: Creates and displays this widget. The button clicks call functions on the controller to either close this widget or open another one (the options screen).
// - UMPGI (Game Instance): The "Quit" button functionality ultimately calls a function on the Game Instance (`EndSession` or `OpenLevel`) to leave the current match.

#include "MPHUD.h"
#include "HUDMenu.generated.h"

class UButton;
class UTextBlock;
class AMPControllerPlayer;

UCLASS()
class UHUDMenu : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDMenu();

protected:
    virtual void NativeConstruct() override;

public:
    // Buttons and TextBlocks
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* continueButton;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* continueText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* optionButton;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* optionText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* quitButton;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* quitText;

    // Localization keys
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString continueTextKey = TEXT("continue");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString optionTextKey = TEXT("option");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString quitTextKey = TEXT("quit");

    // Update button texts from localization
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateButtonTexts();

    // Button handlers
    UFUNCTION()
    void OnContinueClicked();
    UFUNCTION()
    void OnOptionClicked();
    UFUNCTION()
    void OnQuitClicked();

    // Called by OptionHUD to return to menu if previousHUD is EMenu
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ReturnFromOptionHUD();
};