#pragma once

// [Meow-Phone Project]
//
// This class is a simple navigation UI that acts as a hub for multiplayer options.
// It provides the player with the choice to either create a new session (host) or
// search for an existing one (join).
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_SessionGeneral`).
// 2. In the UMG editor, you must create the `Button` and `TextBlock` widgets and name them to match the `BindWidget` properties below (e.g., `createSessionButton`, `titleText`).
// 3. This widget is shown when the player clicks the "Start Game" button on the main menu (`HUDInit`).
// 4. The button click events are bound in C++. They call functions that tell the `AMPControllerPlayer` to show the next appropriate screen (either `HUDCreateSession` or `HUDSearchSession`). The "Back" button tells the controller to remove this widget and go back to the main menu.
//
// Necessary things to define:
// - All `BindWidget` properties MUST have corresponding, correctly named widgets in the child Widget Blueprint.
//
// How it interacts with other classes:
// - UMPHUD: The base HUD class.
// - AMPControllerPlayer: This widget is created by the controller. Its button clicks call functions on the controller to navigate to the "Create Session" or "Search Session" screens, or to go back.
// - HUDInit: The main menu, which opens this widget.
// - HUDCreateSession / HUDSearchSession: The two screens that this widget can navigate to.

#include "CoreMinimal.h"
#include "MPHUD.h"
#include "../../../CommonEnum.h"
#include "HUDSessionGeneral.generated.h"

class UTextBlock;
class UButton;
class UCanvasPanel;

/**
 * HUD for session navigation - simple hub for session management
 * Features: Create session, search session, back to init
 */
UCLASS()
class MEOWPHONE_API UHUDSessionGeneral : public UMPHUD
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // Override parent functions
    virtual void UpdateTexts() override;
    virtual bool ValidateRootWidget() override;

protected:
    // Root widget
    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* rootCanvas;

    // Main buttons
    UPROPERTY(meta = (BindWidget))
    UButton* createSessionButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* searchSessionButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* backButton;

    UPROPERTY(meta = (BindWidget)) 
    UTextBlock* titleText;

    // Localization keys
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString sessionTitleTextKey = TEXT("SESSION_TITLE");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString createSessionTextKey = TEXT("SESSION_CREATE");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString searchSessionTextKey = TEXT("SESSION_SEARCH");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString backTextKey = TEXT("SESSION_BACK");

    // Button click functions
    UFUNCTION()
    void OnCreateSessionClicked();
    
    UFUNCTION()
    void OnSearchSessionClicked();
    
    UFUNCTION()
    void OnBackClicked();
};