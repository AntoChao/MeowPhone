#pragma once

// [Meow-Phone Project]
//
// This class represents the "Create Session" or "Host Game" UI screen. It allows the player
// to configure and launch a new game session, supporting both single-player and multiplayer modes.
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_CreateSession`).
// 2. In the UMG editor for your widget, you must create all the UI elements (`Button`, `TextBlock`, `EditableTextBox`, etc.) and name them exactly as specified in the `meta = (BindWidget)` declarations below (e.g., a `Button` named `startButton`, a `TextBlock` named `titleText`).
// 3. This widget is created and displayed by the `AMPControllerPlayer` when requested, typically from the main menu.
// 4. All the button click events (`OnBackClicked`, `OnStartClicked`) and input change events (`OnSessionNameChanged`, etc.) are already bound to their C++ functions in `NativeConstruct`. You do not need to re-bind them in the Blueprint graph.
// 5. The core logic resides in C++. When the player clicks the "Start" button, the `OnStartClicked` function gathers all the data from the input fields and calls the appropriate function on the Game Instance (`UMPGI`) to either `CreateSession` (for multiplayer) or `StartSinglePlayerGame`.
//
// Necessary things to define:
// - All `BindWidget` properties declared in this header MUST have corresponding, correctly named and typed widgets in the child Widget Blueprint for the UI to function.
// - The `Localization Keys` can be overridden in the Blueprint's defaults to change the keys used for text.
//
// How it interacts with other classes:
// - UMPHUD: The base class for all HUDs in the project.
// - AMPControllerPlayer: Creates and displays this widget.
// - UMPGI (Game Instance): This is the most important interaction. After collecting all the user's choices, this UI calls functions on the Game Instance to actually create the online session or start the single-player level.
// - UManagerLocalization: Uses the localization manager (via `UMPHUD`'s `GetLocalizedText` helper) to populate all its text fields.

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "../../../CommonEnum.h"
#include "HUDCreateSession.generated.h"

class UTextBlock;
class UButton;
class UCanvasPanel;
class UEditableTextBox;
class UCheckBox;

/**
 * HUD for session creation configuration
 * Features: Session mode toggle, multiplayer settings, session creation
 */
UCLASS()
class MEOWPHONE_API UHUDCreateSession : public UMPHUD
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

    // Title
    UPROPERTY(meta = (BindWidget))
    UTextBlock* titleText;

    // Session Mode Section
    UPROPERTY(meta = (BindWidget))
    UTextBlock* sessionModeLabel;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* sessionModeValue;
    
    UPROPERTY(meta = (BindWidget))
    UButton* sessionModeButton;

    // Multiplayer-only fields
    UPROPERTY(meta = (BindWidget))
    UTextBlock* sessionNameLabel;
    
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* sessionNameInput;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* hostNameLabel;
    
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* hostNameInput;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* passwordLabel;
    
    UPROPERTY(meta = (BindWidget))
    UCheckBox* passwordCheckBox;
    
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* passwordInput;

    // Navigation buttons
    UPROPERTY(meta = (BindWidget))
    UButton* backButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* startButton;

    // Current state
    UPROPERTY()
    bool isMultiplayerMode;
    
    UPROPERTY()
    FString sessionName;
    
    UPROPERTY()
    FString hostName;
    
    UPROPERTY()
    bool usePassword;
    
    UPROPERTY()
    FString password;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    int32 maxSessionNameLength = 100;

    // Localization keys
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString sessionTitleTextKey = TEXT("SESSION_CREATE_TITLE");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString sessionModeTextKey = TEXT("SESSION_MODE");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString singlePlayerTextKey = TEXT("SINGLE_PLAYER");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString multiplayerTextKey = TEXT("MULTIPLAYER");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString sessionNameTextKey = TEXT("SESSION_NAME");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString hostNameTextKey = TEXT("HOST_NAME");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString passwordTextKey = TEXT("PASSWORD");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString backTextKey = TEXT("SESSION_BACK");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString startTextKey = TEXT("SESSION_START");

    // Button click functions
    UFUNCTION()
    void OnSessionModeClicked();
    
    UFUNCTION()
    void OnBackClicked();
    
    UFUNCTION()
    void OnStartClicked();

    // Input change functions
    UFUNCTION()
    void OnSessionNameChanged(const FText& newText);
    
    UFUNCTION()
    void OnHostNameChanged(const FText& newText);
    
    UFUNCTION()
    void OnPasswordCheckChanged(bool isChecked);
    
    UFUNCTION()
    void OnPasswordChanged(const FText& newText);

    // Helper functions
    void UpdateSessionModeDisplay();
    void UpdateMultiplayerFieldsVisibility();
    void UpdatePasswordFieldVisibility();
    void InitializeHostName();
    void ValidateInputs();
    
    // Session creation
    void CreateMultiplayerSession();
    void StartSinglePlayerGame();
};