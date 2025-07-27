#pragma once

// [Meow-Phone Project]
//
// This is the specific in-game HUD for the Human character. It inherits from `UHUDCharacter`
// and adds human-specific UI elements, namely the Health Bar and the cat team's
// win progression bar.
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_HumanHUD`). This is the actual widget that will be displayed for human players.
// 2. In the `WBP_HumanHUD` UMG editor, you must design the UI and include widgets named to match the `BindWidget` properties from this class and its parent. For example, you must have a `ProgressBar` named `hpBar` and another named `opponentWinProgression`.
// 3. The `AMPControllerPlayer` creates an instance of your `WBP_HumanHUD` and adds it to the viewport when the player possesses a human character.
// 4. In the `Tick` event of the `WBP_HumanHUD` Blueprint, you should get the possessed `AMPCharacterHuman` and call its `GetHealthPercentage()` function, passing the result to `UpdateHPBar`. Similarly, you should get the `AMPGS` (Game State) and use its `curMPProgressionPercentage` to call `UpdateOpponentWinProgression`.
//
// Necessary things to define:
// - The child Widget Blueprint MUST have widgets named `hpBar` and `opponentWinProgression`.
// - It must also have the widgets required by the parent `UHUDCharacter` class (e.g., `hintText`).
//
// How it interacts with other classes:
// - UHUDCharacter: Inherits common HUD elements and functionality.
// - AMPControllerPlayer: Creates and manages an instance of the derived Blueprint widget.
// - AMPCharacterHuman: The UI gets health information from the possessed human character to update the HP bar.
// - AMPGS (Game State): The UI gets the cat team's score from the Game State to update the opponent's progression bar.

#include "HUDCharacter.h"
#include "HUDCharacterHuman.generated.h"

class UProgressBar;

UCLASS()
class UHUDCharacterHuman : public UHUDCharacter
{
    GENERATED_BODY()

public:
    UHUDCharacterHuman();

protected:
    virtual void NativeConstruct() override;

    // Human-specific UI components
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UProgressBar* hpBar;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UProgressBar* opponentWinProgression;

public:
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateHPBar(float hpPercent);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateOpponentWinProgression(float progress);
};