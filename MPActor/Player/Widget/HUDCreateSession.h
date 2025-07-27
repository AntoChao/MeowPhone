#pragma once

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