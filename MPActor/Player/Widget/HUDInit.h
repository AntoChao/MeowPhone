#pragma once

// [Meow-Phone Project]
//
// This class is the UI for the game's Main Menu. It serves as the initial entry point
// for the player, providing buttons to start or join a game, open options, view credits,
// and quit the game.
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_MainMenu`).
// 2. In the UMG editor, you must create all the `Button` and `TextBlock` widgets and name them to match the `BindWidget` properties below (e.g., a `Button` named `generalSessionButton`, a `TextBlock` named `startGameTXTBlock`).
// 3. This widget is created by the `AMPControllerPlayer` when the `InitLevel` is loaded. The `AMPGMInit` Game Mode is configured to use a HUD class that creates this widget.
// 4. In `NativeConstruct`, the C++ code binds all the button click events to their respective functions (`GoSessionGeneralWidget`, `QuitGame`, etc.). You do not need to do this in the Blueprint graph.
// 5. When a button is clicked, its function is called. These functions then tell the `AMPControllerPlayer` to open up other widgets (e.g., `GoOptionWidget` tells the controller to show the options menu).
//
// Necessary things to define:
// - All `BindWidget` properties MUST have corresponding, correctly named widgets in the child Widget Blueprint.
//
// How it interacts with other classes:
// - UMPHUD: The base HUD class.
// - AMPControllerPlayer: It is created by the player controller and calls functions on the controller to navigate to other UI screens. For example, `GoSessionGeneralWidget` calls `controller->AttachHUD(EHUDType::SessionGeneral, ...)`
// - UKismetSystemLibrary::QuitGame: The `QuitGame` function calls this to exit the application.

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDInit.generated.h"

class UButton;
class UTextBlock;
class UCanvasPanel;
struct FLocalizedText;

UCLASS()
class UHUDInit : public UMPHUD
{
    GENERATED_BODY()

// common Init HUD 
protected:
    // Root widget (required for all UserWidgets)
    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* rootCanvas;
    
    // Text keys for localization
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
    FString startGameTextKey = TEXT("HUD_INIT_START_GAME");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
    FString optionTextKey = TEXT("HUD_INIT_OPTIONS");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
    FString creditTextKey = TEXT("HUD_INIT_CREDITS");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
    FString quitTextKey = TEXT("HUD_INIT_QUIT");

    // text components
    UPROPERTY(meta = (BindWidget))
    UTextBlock* startGameTXTBlock;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* optionTXTBlock;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* creditTXTBlock;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* quitTXTBlock;

    // buttons
    UPROPERTY(meta = (BindWidget))
    UButton* generalSessionButton;
    UPROPERTY(meta = (BindWidget))
    UButton* optionButton;
    UPROPERTY(meta = (BindWidget))
    UButton* creditButton;
    UPROPERTY(meta = (BindWidget))
    UButton* quitButton;

    // bind functions
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
    void GoSessionGeneralWidget();
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
    void GoOptionWidget();
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
    void GoCreditWidget();
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
    void QuitGame();
    
    // Override parent functions
    virtual void UpdateTexts() override;
    virtual bool ValidateRootWidget() override;
    virtual void NativeDestruct() override;
};