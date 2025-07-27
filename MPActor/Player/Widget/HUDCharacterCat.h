#pragma once

// [Meow-Phone Project]
//
// This is the specific in-game HUD for the Cat character. It inherits the common
// elements from `UHUDCharacter` (like item slots) and adds cat-specific UI widgets,
// such as the struggle bar (for when being held), the ability icon, and the human team's
// win progression bar.
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_CatHUD`). This is the widget that will actually be created and displayed for cat players.
// 2. In the `WBP_CatHUD` UMG editor, you must design the layout and include widgets that are named to match the `BindWidget` properties from this class and its parent. For example, you need a `ProgressBar` named `struggleBar` and an `Image` named `abilitySlot`.
// 3. The `AMPControllerPlayer` is responsible for creating an instance of your `WBP_CatHUD` and adding it to the screen when the possessed pawn is a cat.
// 4. The `Tick` event in the `WBP_CatHUD` Blueprint graph is responsible for calling the `Update...` functions. It gets the possessed `AMPCharacterCat` and queries its state (e.g., gets the struggle percentage, gets the current ability) to pass into the update functions.
//
// Necessary things to define:
// - The child Widget Blueprint MUST have widgets named `struggleBar`, `abilitySlot`, and `opponentWinProgression` to match the `BindWidget` declarations.
// - The child Widget Blueprint must also have the widgets required by the parent class (`UHUDCharacter`).
//
// How it interacts with other classes:
// - UHUDCharacter: Inherits its base functionality and common widgets.
// - AMPControllerPlayer: Creates and displays an instance of the `WBP_CatHUD` Blueprint.
// - AMPCharacterCat: The Blueprint widget logic continuously gets the possessed cat character to read its state (e.g., `GetStruggleBarPercentage()`) and update the UI accordingly.
// - AMPGS (Game State): The logic for updating `opponentWinProgression` will get the Game State and read the `caughtCatsPercentage` to display the human team's progress.

#include "HUDCharacter.h"
#include "HUDCharacterCat.generated.h"

class UProgressBar;
class UImage;

UCLASS()
class UHUDCharacterCat : public UHUDCharacter
{
    GENERATED_BODY()

public:
    UHUDCharacterCat();

protected:
    virtual void NativeConstruct() override;

    // Cat-specific UI components
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UProgressBar* struggleBar;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UImage* abilitySlot;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UProgressBar* opponentWinProgression;

public:
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateStruggleBar(float progress, bool visible);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateAbilitySlot(UTexture2D* abilityIcon, bool visible);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateOpponentWinProgression(float progress);
};