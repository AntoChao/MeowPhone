#include "HUDCreateSession.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableTextBox.h"
#include "Components/CheckBox.h"
#include "HighLevel/MPGI.h"
#include "MPActor/Player/MPControllerPlayer.h"

// Constructor removed - not needed for UserWidget

void UHUDCreateSession::NativeConstruct()
{
    Super::NativeConstruct();
    
    	// Validate root widget first
	if (!ValidateRootWidget())
	{
		UMPLogManager::LogError(TEXT("Root widget validation failed!"), TEXT("HUDCreateSession"));
		return;
	}
    
    // Initialize default values
    isMultiplayerMode = true; // Default to multiplayer
    sessionName = TEXT("");
    hostName = TEXT("");
    usePassword = false;
    password = TEXT("");
    
    // Initialize host name from Steam
    InitializeHostName();
    
    // Update UI
    UpdateSessionModeDisplay();
    UpdateMultiplayerFieldsVisibility();
    UpdatePasswordFieldVisibility();
    
    // Update texts after construction
    UpdateTexts();
    
    // Bind button events
    if (sessionModeButton)
    {
        sessionModeButton->OnClicked.AddDynamic(this, &UHUDCreateSession::OnSessionModeClicked);
    }
    
    if (backButton)
    {
        backButton->OnClicked.AddDynamic(this, &UHUDCreateSession::OnBackClicked);
    }
    
    if (startButton)
    {
        startButton->OnClicked.AddDynamic(this, &UHUDCreateSession::OnStartClicked);
    }
    
    // Bind input events
    if (sessionNameInput)
    {
        sessionNameInput->OnTextChanged.AddDynamic(this, &UHUDCreateSession::OnSessionNameChanged);
    }
    
    if (hostNameInput)
    {
        hostNameInput->OnTextChanged.AddDynamic(this, &UHUDCreateSession::OnHostNameChanged);
    }
    
    if (passwordCheckBox)
    {
        passwordCheckBox->OnCheckStateChanged.AddDynamic(this, &UHUDCreateSession::OnPasswordCheckChanged);
    }
    
    if (passwordInput)
    {
        passwordInput->OnTextChanged.AddDynamic(this, &UHUDCreateSession::OnPasswordChanged);
    }
}

void UHUDCreateSession::NativeDestruct()
{
    // Unbind button events
    if (sessionModeButton)
    {
        sessionModeButton->OnClicked.RemoveAll(this);
    }
    
    if (backButton)
    {
        backButton->OnClicked.RemoveAll(this);
    }
    
    if (startButton)
    {
        startButton->OnClicked.RemoveAll(this);
    }
    
    // Unbind input events
    if (sessionNameInput)
    {
        sessionNameInput->OnTextChanged.RemoveAll(this);
    }
    
    if (hostNameInput)
    {
        hostNameInput->OnTextChanged.RemoveAll(this);
    }
    
    if (passwordCheckBox)
    {
        passwordCheckBox->OnCheckStateChanged.RemoveAll(this);
    }
    
    if (passwordInput)
    {
        passwordInput->OnTextChanged.RemoveAll(this);
    }
    
    Super::NativeDestruct();
}

void UHUDCreateSession::UpdateTexts()
{
    Super::UpdateTexts();
    
    // Update title
    if (titleText)
    {
        titleText->SetText(GetLocalizedText(sessionTitleTextKey));
    }
    
    // Update session mode label
    if (sessionModeLabel)
    {
        sessionModeLabel->SetText(GetLocalizedText(sessionModeTextKey));
    }
    
    // Update session mode value
    UpdateSessionModeDisplay();
    
    // Update multiplayer field labels
    if (sessionNameLabel)
    {
        sessionNameLabel->SetText(GetLocalizedText(sessionNameTextKey));
    }
    
    if (hostNameLabel)
    {
        hostNameLabel->SetText(GetLocalizedText(hostNameTextKey));
    }
    
    if (passwordLabel)
    {
        passwordLabel->SetText(GetLocalizedText(passwordTextKey));
    }
    
    // Update button texts
    if (backButton)
    {
        UTextBlock* buttonText = Cast<UTextBlock>(backButton->GetChildAt(0));
        if (buttonText)
        {
            buttonText->SetText(GetLocalizedText(backTextKey));
        }
    }
    
    if (startButton)
    {
        UTextBlock* buttonText = Cast<UTextBlock>(startButton->GetChildAt(0));
        if (buttonText)
        {
            buttonText->SetText(GetLocalizedText(startTextKey));
        }
    }
}

bool UHUDCreateSession::ValidateRootWidget()
{
    if (!rootCanvas)
    {
        UMPLogManager::LogError(TEXT("Root canvas is missing!"), TEXT("HUDCreateSession"));
        return false;
    }
    return true;
}

// Button click functions
void UHUDCreateSession::OnSessionModeClicked()
{
    UMPLogManager::LogInfo(TEXT("Session mode clicked"), TEXT("HUDCreateSession"));
    
    // Toggle between single player and multiplayer
    isMultiplayerMode = !isMultiplayerMode;
    
    UpdateSessionModeDisplay();
    UpdateMultiplayerFieldsVisibility();
    UpdatePasswordFieldVisibility();
}

void UHUDCreateSession::OnBackClicked()
{
    UMPLogManager::LogInfo(TEXT("Back clicked"), TEXT("HUDCreateSession"));
    
    if (!owner)
    {
        UMPLogManager::LogError(TEXT("Owner is null, cannot navigate"), TEXT("HUDCreateSession"));
        return;
    }
    
    // Remove current HUD and add Session General HUD
    owner->RemoveHUD(EHUDType::ECreateSession);
    owner->AttachHUD(EHUDType::ESessionGeneral, 0);
    
    UMPLogManager::LogInfo(TEXT("Switched back to Session General HUD"), TEXT("HUDCreateSession"));
}

void UHUDCreateSession::OnStartClicked()
{
    UMPLogManager::LogInfo(TEXT("Start clicked"), TEXT("HUDCreateSession"));
    
    // Validate inputs
    ValidateInputs();
    
    if (isMultiplayerMode)
    {
        CreateMultiplayerSession();
    }
    else
    {
        StartSinglePlayerGame();
    }
}

// Input change functions
void UHUDCreateSession::OnSessionNameChanged(const FText& newText)
{
    sessionName = newText.ToString();
    
    // Limit session name length
    if (sessionName.Len() > maxSessionNameLength)
    {
        sessionName = sessionName.Left(maxSessionNameLength);
        if (sessionNameInput)
        {
            sessionNameInput->SetText(FText::FromString(sessionName));
        }
    }
    
    UMPLogManager::LogDebug(FString::Printf(TEXT("Session name changed to '%s'"), *sessionName), TEXT("HUDCreateSession"));
}

void UHUDCreateSession::OnHostNameChanged(const FText& newText)
{
    hostName = newText.ToString();
    UMPLogManager::LogDebug(FString::Printf(TEXT("Host name changed to '%s'"), *hostName), TEXT("HUDCreateSession"));
}

void UHUDCreateSession::OnPasswordCheckChanged(bool isChecked)
{
    usePassword = isChecked;
    UpdatePasswordFieldVisibility();
    UMPLogManager::LogDebug(FString::Printf(TEXT("Password enabled: %s"), usePassword ? TEXT("Yes") : TEXT("No")), TEXT("HUDCreateSession"));
}

void UHUDCreateSession::OnPasswordChanged(const FText& newText)
{
    password = newText.ToString();
    UMPLogManager::LogDebug(TEXT("Password changed"), TEXT("HUDCreateSession"));
}

// Helper functions
void UHUDCreateSession::UpdateSessionModeDisplay()
{
    if (sessionModeValue)
    {
        FText modeText = isMultiplayerMode ? 
            GetLocalizedText(multiplayerTextKey) : 
            GetLocalizedText(singlePlayerTextKey);
        sessionModeValue->SetText(modeText);
    }
}

void UHUDCreateSession::UpdateMultiplayerFieldsVisibility()
{
    ESlateVisibility visibility = isMultiplayerMode ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
    
    if (sessionNameLabel)
    {
        sessionNameLabel->SetVisibility(visibility);
    }
    
    if (sessionNameInput)
    {
        sessionNameInput->SetVisibility(visibility);
    }
    
    if (hostNameLabel)
    {
        hostNameLabel->SetVisibility(visibility);
    }
    
    if (hostNameInput)
    {
        hostNameInput->SetVisibility(visibility);
    }
    
    if (passwordLabel)
    {
        passwordLabel->SetVisibility(visibility);
    }
    
    if (passwordCheckBox)
    {
        passwordCheckBox->SetVisibility(visibility);
    }
    
    if (passwordInput)
    {
        passwordInput->SetVisibility(visibility);
    }
}

void UHUDCreateSession::UpdatePasswordFieldVisibility()
{
    if (!isMultiplayerMode)
    {
        // Hide password field in single player mode
        if (passwordInput)
        {
            passwordInput->SetVisibility(ESlateVisibility::Hidden);
        }
        return;
    }
    
    // Show/hide password input based on checkbox state
    ESlateVisibility visibility = usePassword ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
    
    if (passwordInput)
    {
        passwordInput->SetVisibility(visibility);
    }
}

void UHUDCreateSession::InitializeHostName()
{
    UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
    if (gameInstance)
    {
        // Try to get Steam username first
        FString steamUsername = gameInstance->GetSteamUsername();
        if (!steamUsername.IsEmpty())
        {
            hostName = steamUsername;
            UMPLogManager::LogDebug(FString::Printf(TEXT("Retrieved Steam username: %s"), *hostName), TEXT("HUDCreateSession"));
        }
        else
        {
            // Fallback to random host name
            hostName = gameInstance->GenerateRandomName();
            UMPLogManager::LogDebug(FString::Printf(TEXT("Generated random host name: %s"), *hostName), TEXT("HUDCreateSession"));
        }
        
        // Set the hint text in the input field
        if (hostNameInput)
        {
            hostNameInput->SetHintText(FText::FromString(hostName));
        }
    }
}

void UHUDCreateSession::ValidateInputs()
{
    // For multiplayer, ensure we have valid session name and host name
    if (isMultiplayerMode)
    {
        // If session name is empty, generate a random one
        if (sessionName.IsEmpty())
        {
            UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
            if (gameInstance)
            {
                sessionName = gameInstance->GenerateRandomName();
            }
        }
        
        // If host name is empty, use the hint text (Steam username or random)
        if (hostName.IsEmpty())
        {
            UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
            if (gameInstance)
            {
                FString steamUsername = gameInstance->GetSteamUsername();
                hostName = !steamUsername.IsEmpty() ? steamUsername : gameInstance->GenerateRandomName();
            }
        }
        
        // If password is enabled but empty, treat as no password
        if (usePassword && password.IsEmpty())
        {
            usePassword = false;
            if (passwordCheckBox)
            {
                passwordCheckBox->SetIsChecked(false);
            }
        }
    }
    
    UMPLogManager::LogInfo(FString::Printf(TEXT("Validated inputs - Mode: %s, Session: '%s', Host: '%s', Password: %s"), 
           isMultiplayerMode ? TEXT("Multiplayer") : TEXT("Single Player"),
           *sessionName, *hostName, usePassword ? TEXT("Yes") : TEXT("No")), TEXT("HUDCreateSession"));
}

// Session creation
void UHUDCreateSession::CreateMultiplayerSession()
{
    UMPLogManager::LogInfo(TEXT("Creating multiplayer session"), TEXT("HUDCreateSession"));
    
    UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
    if (gameInstance)
    {
        gameInstance->CreateSession(sessionName, hostName, usePassword, password);
        
        // The session creation will trigger level loading when completed
        // This is handled by the existing HostSessionCompleted callback in MPGI
    }
    else
    {
        UMPLogManager::LogError(TEXT("Could not get game instance"), TEXT("HUDCreateSession"));
    }
}

void UHUDCreateSession::StartSinglePlayerGame()
{
    UMPLogManager::LogInfo(TEXT("Starting single player game"), TEXT("HUDCreateSession"));
    
    UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
    if (gameInstance)
    {
        gameInstance->StartSinglePlayerGame();
    }
    else
    {
        UMPLogManager::LogError(TEXT("Could not get game instance"), TEXT("HUDCreateSession"));
    }
}