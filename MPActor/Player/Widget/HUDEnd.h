#pragma once

// [Meow-Phone Project]
//
// This class is the UI widget for the End of Match screen. It displays the game's
// final result (Win/Loss), shows the final progression for both teams, and provides
// a button to return to the lobby.
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_EndScreen`).
// 2. In the UMG editor, you must create all the UI elements and name them to match the `BindWidget` properties below (e.g., a `TextBlock` named `resultText`, a `Button` named `returnLobbyButton`).
// 3. The `AMPControllerPlayer` creates and displays this widget when the game ends, which is triggered by the `UManagerMatch`.
// 4. The logic for populating this screen resides on the `AMPControllerPlayer`. The controller will get the final game result and team scores from the `AMPGS` (Game State) and call the `Set...` functions on this widget (`SetResultText`, `SetHumanProgression`, etc.) to update the display.
// 5. The `OnReturnLobbyClicked` function is bound in C++ and will call a function on the Game Mode (likely `RestartLobby`) to transition the game back to the lobby state.
//
// Necessary things to define:
// - All `BindWidget` properties in this header MUST have corresponding, correctly named widgets in the child Widget Blueprint.
//
// How it interacts with other classes:
// - UMPHUD: The base HUD class.
// - AMPControllerPlayer: Creates this widget and is responsible for calling its `Set...` functions to populate it with the final game data.
// - UManagerMatch: The match manager determines when the game is over and triggers the process that leads to this screen being shown.
// - AMPGS (Game State): The final scores displayed on this screen are read from the Game State.
// - AMPGMGameplay (Game Mode): The "Return to Lobby" button ultimately calls a function on the Game Mode to restart the level and return to the lobby map.

#include "MPHUD.h"

#include "HUDEnd.generated.h"

class UTextBlock;
class UButton;
class UProgressBar;

UCLASS()
class UHUDEnd : public UMPHUD
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    // Win/Lose text
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    class UTextBlock* resultText;

    // Progression bars for both teams
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    class UProgressBar* humanProgressionBar;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    class UProgressBar* catProgressionBar;

    // Return to lobby button
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    class UButton* returnLobbyButton;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    class UTextBlock* returnLobbyButtonText;

public:
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetResultText(const FString& resultKey);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetHumanProgression(float percent);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetCatProgression(float percent);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetReturnLobbyButtonText(const FString& key);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void OnReturnLobbyClicked();
};