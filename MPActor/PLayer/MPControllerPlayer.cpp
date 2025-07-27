#include "MPControllerPlayer.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "../../HighLevel/Managers/ManagerLog.h"
#include "../Player/MPPlayerState.h"
#include "../Character/MPCharacter.h"
#include "../Character/MPCharacterHuman.h"
#include "../Character/MPCharacterCat.h"
#include "../Item/MPItem.h"
#include "../EnvActor/MPEnvActorHolder.h"
#include "../Ability/MPAbility.h"
#include "../../HighLevel/MPGMGameplay.h"
#include "../../HighLevel/MPGS.h"
#include "Camera/CameraComponent.h"

// HUD includes
#include "../Player/Widget/HUDInit.h"
#include "../Player/Widget/HUDOption.h"
#include "../Player/Widget/HUDSessionGeneral.h"
#include "../Player/Widget/HUDCreateSession.h"
#include "../Player/Widget/HUDSearchSession.h"
#include "../Player/Widget/HUDLobby.h"
#include "../Player/Widget/HUDLobbyManager.h"
#include "../Player/Widget/HUDCharacterHuman.h"
#include "../Player/Widget/HUDCharacterCat.h"
#include "../Player/Widget/HUDMenu.h"
#include "../Player/Widget/HUDEnd.h"

AMPControllerPlayer::AMPControllerPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Set default values
	lookXSensitive = 1.0f;
	lookYSensitive = 1.0f;
}

void AMPControllerPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void AMPControllerPlayer::InitializePS(int aPlayerIndex)
{
	AMPPlayerState* thePlayerState = Cast<AMPPlayerState>(PlayerState);
	if (thePlayerState)
	{
		thePlayerState->playerIndex = aPlayerIndex;
	}
}

// Team assignment methods
void AMPControllerPlayer::RequestTeamSwitch(ETeam newTeam)
{
	ServerRequestTeamSwitch(newTeam);
}

ETeam AMPControllerPlayer::GetCurrentTeam() const
{
	AMPPlayerState* thePlayerState = Cast<AMPPlayerState>(PlayerState);
	if (thePlayerState)
	{
		return thePlayerState->playerTeam;
	}
	return ETeam::ENone;
}

bool AMPControllerPlayer::IsTeamAssigned() const
{
	return GetCurrentTeam() != ETeam::ENone;
}

// Server RPCs for team assignment
void AMPControllerPlayer::ServerRequestTeamSwitch_Implementation(ETeam newTeam)
{
	// Validate authority
	if (!HasAuthority())
	{
		UManagerLog::LogWarning(TEXT("ServerRequestTeamSwitch called without authority"), TEXT("MPControllerPlayer"));
		return;
	}
	
	// Call GameMode to handle team switch
	AMPGMGameplay* gameMode = Cast<AMPGMGameplay>(GetWorld()->GetAuthGameMode());
	if (gameMode)
	{
		bool success = gameMode->GetManagerLobby()->SwitchPlayerTeam(this, newTeam);
		ClientTeamSwitchResult(newTeam, success);
		// Team change will trigger OnRep_PlayerTeam callback to update HUD
	}
	else
	{
		UManagerLog::LogError(TEXT("Could not get GameMode for team switch"), TEXT("MPControllerPlayer"));
		ClientTeamSwitchResult(newTeam, false);
	}
}

// Client RPCs for team assignment results
void AMPControllerPlayer::ClientTeamSwitchResult_Implementation(ETeam newTeam, bool success)
{
	if (success)
	{
		UManagerLog::LogInfo(FString::Printf(TEXT("Successfully switched to %s team"), 
			newTeam == ETeam::EHuman ? TEXT("Human") : TEXT("Cat")), TEXT("MPControllerPlayer"));
		
		// Update HUD team button visibility
		if (ManagerLobbyHUD && ManagerLobbyHUD->lobbyHUD)
		{
			ManagerLobbyHUD->lobbyHUD->UpdateTeamButtonVisibility();
		}
	}
	else
	{
		UManagerLog::LogWarning(FString::Printf(TEXT("Failed to switch to %s team"), 
			newTeam == ETeam::EHuman ? TEXT("Human") : TEXT("Cat")), TEXT("MPControllerPlayer"));
	}
}

// hud manager
void AMPControllerPlayer::TurnUIInputOn()
{
	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
}
void AMPControllerPlayer::TurnGameplayInputOn()
{
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void AMPControllerPlayer::AttachHUD(EHUDType hudType, int zOrder)
{
    switch(hudType)
    {
        case EHUDType::EInit :
        {
            if (!initHUD && initHUDClass)
            {
                initHUD = CreateWidget<UHUDInit>(this, initHUDClass);
                if (initHUD != nullptr)
                {
                    TurnUIInputOn();
                    initHUD->AddToViewport(zOrder);
                    initHUD->SetOwner(this);
                }
            }
            break;
        }
        case EHUDType::EOption :
        {
            if (!optionHUD && optionHUDClass)
            {
                optionHUD = CreateWidget<UHUDOption>(this, optionHUDClass);
                if (optionHUD != nullptr)
                {
                    TurnUIInputOn();
                    optionHUD->AddToViewport(zOrder);
                    optionHUD->SetOwner(this);
                }
            }
            break;
        }
        case EHUDType::ESessionGeneral :
        {
            if (!sessionGeneralHUD && sessionGeneralHUDClass)
            {
                sessionGeneralHUD = CreateWidget<UHUDSessionGeneral>(this, sessionGeneralHUDClass);
                if (sessionGeneralHUD != nullptr)
                {
                    TurnUIInputOn();
                    sessionGeneralHUD->AddToViewport(zOrder);
                    sessionGeneralHUD->SetOwner(this);
                }
            }
            break;
        }
        case EHUDType::ECreateSession :
        {
            if (!createSessionHUD && createSessionHUDClass)
            {
                createSessionHUD = CreateWidget<UHUDCreateSession>(this, createSessionHUDClass);
                if (createSessionHUD != nullptr)
                {
                    TurnUIInputOn();
                    createSessionHUD->AddToViewport(zOrder);
                    createSessionHUD->SetOwner(this);
                }
            }
            break;
        }
        case EHUDType::ESearchSession :
        {
            if (!searchSessionHUD && searchSessionHUDClass)
            {
                searchSessionHUD = CreateWidget<UHUDSearchSession>(this, searchSessionHUDClass);
                if (searchSessionHUD != nullptr)
                {
                    TurnUIInputOn();
                    searchSessionHUD->AddToViewport(zOrder);
                    searchSessionHUD->SetOwner(this);
                }
            }
            break;
        }
        case EHUDType::ELobby :
        {
            if (!ManagerLobbyHUD && ManagerLobbyHUDClass)
            {
                ManagerLobbyHUD = CreateWidget<UHUDManagerLobby>(this, ManagerLobbyHUDClass);
                if (ManagerLobbyHUD != nullptr)
                {
                    TurnUIInputOn();
                    ManagerLobbyHUD->AddToViewport(zOrder);
                    ManagerLobbyHUD->SetOwner(this);
                }
            }
            break;
        }
        case EHUDType::ECustomHuman :
        {
            // Customization is now handled by the lobby manager
            // This case is kept for backward compatibility but should not be used
            UManagerLog::LogWarning(TEXT("ECustomHuman HUD type is deprecated. Use ELobby instead."), TEXT("MPControllerPlayer"));
            break;
        }
        case EHUDType::ECustomCat :
        {
            // Customization is now handled by the lobby manager
            // This case is kept for backward compatibility but should not be used
            UManagerLog::LogWarning(TEXT("ECustomCat HUD type is deprecated. Use ELobby instead."), TEXT("MPControllerPlayer"));
            break;
        }
        case EHUDType::EGameplayHuman :
        {
            if (!humanHUD && humanHUDClass)
            {
                humanHUD = CreateWidget<UHUDCharacterHuman>(this, humanHUDClass);
                if (humanHUD != nullptr)
                {
                    TurnGameplayInputOn();
                    humanHUD->AddToViewport(zOrder);
                    humanHUD->SetOwner(this);
                }
            }
            break;
        }
        case EHUDType::EGameplayCat :
        {
            if (!catHUD && catHUDClass)
            {
                catHUD = CreateWidget<UHUDCharacterCat>(this, catHUDClass);
                if (catHUD != nullptr)
                {
                    TurnGameplayInputOn();
                    catHUD->AddToViewport(zOrder);
                    catHUD->SetOwner(this);
                }
            }
            break;
        }
        case EHUDType::EMenu :
        {
            if (!menuHUD && menuHUDClass)
            {
                menuHUD = CreateWidget<UHUDMenu>(this, menuHUDClass);
                if (menuHUD != nullptr)
                {
                    TurnUIInputOn();
                    menuHUD->AddToViewport(zOrder);
                    menuHUD->SetOwner(this);
                }
            }
            break;
        }
        case EHUDType::EEnd :
        {
            if (!endHUD && endHUDClass)
            {
                endHUD = CreateWidget<UHUDEnd>(this, endHUDClass);
                if (endHUD != nullptr)
                {
                    TurnUIInputOn();
                    endHUD->AddToViewport(zOrder);
                    endHUD->SetOwner(this);
                }
            }
            break;
        }
    }
}
void AMPControllerPlayer::RemoveHUD(EHUDType hudType)
{
    switch(hudType)
    {
        case EHUDType::EInit :
        {
            if (initHUD)
            {
                TurnUIInputOn();
                initHUD->RemoveFromParent();
                initHUD = nullptr;
            }
            break;
        }
        case EHUDType::EOption :
        {
            if (optionHUD)
            {
                TurnUIInputOn();
                optionHUD->RemoveFromParent();
                optionHUD = nullptr;
            }
            break;
        }
        case EHUDType::ESessionGeneral :
        {
            if (sessionGeneralHUD)
            {
                TurnUIInputOn();
                sessionGeneralHUD->RemoveFromParent();
                sessionGeneralHUD = nullptr;
            }
            break;
        }
        case EHUDType::ECreateSession :
        {
            if (createSessionHUD)
            {
                TurnUIInputOn();
                createSessionHUD->RemoveFromParent();
                createSessionHUD = nullptr;
            }
            break;
        }
        case EHUDType::ESearchSession :
        {
            if (searchSessionHUD)
            {
                TurnUIInputOn();
                searchSessionHUD->RemoveFromParent();
                searchSessionHUD = nullptr;
            }
            break;
        }
        case EHUDType::ELobby :
        {
            if (ManagerLobbyHUD)
            {
                TurnUIInputOn();
                ManagerLobbyHUD->RemoveFromParent();
                ManagerLobbyHUD = nullptr;
            }
            break;
        }
        case EHUDType::ECustomHuman :
        {
            // Customization is now handled by the lobby manager
            UManagerLog::LogWarning(TEXT("ECustomHuman HUD removal is deprecated. Use ELobby instead."), TEXT("MPControllerPlayer"));
            break;
        }
        case EHUDType::ECustomCat :
        {
            // Customization is now handled by the lobby manager
            UManagerLog::LogWarning(TEXT("ECustomCat HUD removal is deprecated. Use ELobby instead."), TEXT("MPControllerPlayer"));
            break;
        }
        case EHUDType::EGameplayHuman :
        {
            if (humanHUD)
            {
                TurnGameplayInputOn();
                humanHUD->RemoveFromParent();
                humanHUD = nullptr;
            }
            break;
        }
        case EHUDType::EGameplayCat :
        {
            if (catHUD)
            {
                TurnGameplayInputOn();
                catHUD->RemoveFromParent();
                catHUD = nullptr;
            }
            break;
        }
        case EHUDType::EMenu :
        {
            if (menuHUD)
            {
                TurnGameplayInputOn();
                menuHUD->RemoveFromParent();
                menuHUD = nullptr;
            }
            break;
        }
        case EHUDType::EEnd :
        {
            if (endHUD)
            {
                TurnUIInputOn();
                endHUD->RemoveFromParent();
                endHUD = nullptr;
            }
            break;
        }
    }
}

// game progress update
void AMPControllerPlayer::LobbyStartUpdate()
{
    AMPPlayerState* thePlayerState = Cast<AMPPlayerState>(PlayerState);
    if (thePlayerState)
    {
        thePlayerState->isPlayerReady = false;
        thePlayerState->isPlayerDied = false;
    }
}
void AMPControllerPlayer::CharacterCustomStartUpdate()
{
    AMPPlayerState* thePlayerState = Cast<AMPPlayerState>(PlayerState);
    if (thePlayerState)
    {
        thePlayerState->isPlayerReady = false;
        thePlayerState->isPlayerDied = false;
    }
}
void AMPControllerPlayer::PrepareStartUpdate()
{
    AMPPlayerState* thePlayerState = Cast<AMPPlayerState>(PlayerState);
    if (thePlayerState)
    {
        thePlayerState->isPlayerReady = false;
        thePlayerState->isPlayerDied = false;
    }
}
void AMPControllerPlayer::GameplayStartUpdate()
{
    AMPPlayerState* thePlayerState = Cast<AMPPlayerState>(PlayerState);
    if (thePlayerState)
    {
        thePlayerState->isPlayerReady = false;
        thePlayerState->isPlayerDied = false;
    }
}

void AMPControllerPlayer::UpdateLobbyHUDCountdownText(int secondRemaining)
{
    if (ManagerLobbyHUD)
    {
        ManagerLobbyHUD->UpdateCountdownText(secondRemaining);
	}
}

// character relation
void AMPControllerPlayer::OnPossess(APawn* inPawn)
{
    Super::OnPossess(inPawn);

    controlledBody = Cast<AMPCharacter>(inPawn);
    SetupMappingContext();
    SetupInputComponent();
}
void AMPControllerPlayer::UnPossessEffect()
{
    return;
}

/* ControlledBodyDied()
* ControlledBodyDied should be called when character died 
* It call gamemode to let it spawn a spectator pawn to possess
*/
void AMPControllerPlayer::ControlledBodyDied()
{
    AMPPlayerState* thePlayerState = Cast<AMPPlayerState>(PlayerState);
    if (thePlayerState)
    {
        thePlayerState->isPlayerDied = true;
    }

    AGameModeBase* curGameMode = UGameplayStatics::GetGameMode(GetWorld());
    AMPGMGameplay* mpGameMode = Cast<AMPGMGameplay>(curGameMode);

    if (mpGameMode)
    {
        FVector diedLocation = FVector::ZeroVector;
        FRotator diedRotation = FRotator::ZeroRotator;

        if (controlledBody)
        {
            diedLocation = controlledBody->GetActorLocation();
            diedRotation = controlledBody->GetActorRotation();
        }
        mpGameMode->RegisterPlayerDeath(this, diedLocation, diedRotation);
    }
}

// mapping input actions
void AMPControllerPlayer::SetupMappingContext()
{
    if (IsLocalPlayerController())
	{
		if (GetLocalPlayer() != nullptr)
		{
			UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

			if (subsystem)
			{
				subsystem->AddMappingContext(gpGamppingContext, 0);
			}
		}
	}
}
void AMPControllerPlayer::SetupInputComponent()
{
    Super::SetupInputComponent();

	if (UEnhancedInputComponent* enhancedInput = Cast<UEnhancedInputComponent>(InputComponent)) {
		
		enhancedInput->BindAction(openMenuAction, ETriggerEvent::Started, this, &AMPControllerPlayer::OpenMenuFunc);

        enhancedInput->BindAction(lookAction, ETriggerEvent::Triggered, this, &AMPControllerPlayer::LookFunc);

        enhancedInput->BindAction(moveAction, ETriggerEvent::Triggered, this, &AMPControllerPlayer::MoveFunc);
        enhancedInput->BindAction(moveAction, ETriggerEvent::Completed, this, &AMPControllerPlayer::MoveEndFunc);

        enhancedInput->BindAction(runAction, ETriggerEvent::Triggered, this, &AMPControllerPlayer::RunFunc);
        enhancedInput->BindAction(runAction, ETriggerEvent::Completed, this, &AMPControllerPlayer::RunEndFunc);

        enhancedInput->BindAction(jumpAction, ETriggerEvent::Triggered, this, &AMPControllerPlayer::JumpFunc);
        enhancedInput->BindAction(jumpAction, ETriggerEvent::Completed, this, &AMPControllerPlayer::JumpEndFunc);

        enhancedInput->BindAction(crouchAction, ETriggerEvent::Triggered, this, &AMPControllerPlayer::CrouchFunc);
        enhancedInput->BindAction(crouchAction, ETriggerEvent::Completed, this, &AMPControllerPlayer::CrouchEndFunc);

        enhancedInput->BindAction(interactAction, ETriggerEvent::Started, this, &AMPControllerPlayer::InteractFunc);
        
        enhancedInput->BindAction(selectItemOneAction, ETriggerEvent::Started, this, &AMPControllerPlayer::SelectItemOneFunc);
        enhancedInput->BindAction(selectItemTwoAction, ETriggerEvent::Started, this, &AMPControllerPlayer::SelectItemTwoFunc);
        enhancedInput->BindAction(selectItemThreeAction, ETriggerEvent::Started, this, &AMPControllerPlayer::SelectItemThreeFunc);
        
        enhancedInput->BindAction(useCurItemAction, ETriggerEvent::Started, this, &AMPControllerPlayer::UseCurItemFunc);
        enhancedInput->BindAction(useCurItemAction, ETriggerEvent::Ongoing, this, &AMPControllerPlayer::UseCurItemFunc);
        enhancedInput->BindAction(dropCurItemAction, ETriggerEvent::Started, this, &AMPControllerPlayer::DropCurItemFunc);
        
        enhancedInput->BindAction(useAbilityAction, ETriggerEvent::Started, this, &AMPControllerPlayer::UseAbilityFunc);
    }
}

void AMPControllerPlayer::OpenMenuFunc(const FInputActionValue& value)
{
    // Toggle menu: if open, close it; if closed, open it
    if (menuHUD)
    {
        RemoveHUD(EHUDType::EMenu);
        TurnGameplayInputOn();
    }
    else if (controlledBody)
    {
        AttachHUD(EHUDType::EMenu, 0);
        TurnUIInputOn();
    }
}
void AMPControllerPlayer::LookFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		FVector2D lookVector = value.Get<FVector2D>();

        lookVector.X *= lookXSensitive;
        lookVector.Y *= lookYSensitive;

		controlledBody->Look(lookVector);
	}
}
void AMPControllerPlayer::MoveFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
        FVector2D moveVector = value.Get<FVector2D>();
		controlledBody->Move(moveVector);
	}
}
void AMPControllerPlayer::MoveEndFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
        controlledBody->MoveStop();
    }
}
void AMPControllerPlayer::RunFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		if (value.Get<bool>())
		{
            controlledBody->Run();
		}
	}
}
void AMPControllerPlayer::RunEndFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
        controlledBody->RunStop();
	}
}
void AMPControllerPlayer::JumpFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		if (value.Get<bool>())
		{
            controlledBody->JumpStart();
		}
	}
}
void AMPControllerPlayer:: JumpEndFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		if (value.Get<bool>())
		{
            controlledBody->JumpEnd();
		}
	}
}
void AMPControllerPlayer::CrouchFunc(const FInputActionValue& value)
{
    if (controlledBody)
    {
        if (value.Get<bool>())
		{
            controlledBody->CrouchStart();
		}
    }   
}

void AMPControllerPlayer::CrouchEndFunc(const FInputActionValue& value)
{
    if (controlledBody)
    {
        if (value.Get<bool>())
		{
            controlledBody->CrouchEnd();
		}
    }
    
}

void AMPControllerPlayer::InteractFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		if (value.Get<bool>())
		{
			if (HasAuthority())
			{
				controlledBody->Interact();
			}
			else
			{
				Server_RequestInteract();
			}
		}
	}
}
void AMPControllerPlayer::SelectItemOneFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		if (value.Get<bool>())
		{
			if (HasAuthority()) { controlledBody->SelectItem(1); }
			else { Server_RequestSelectItem(1); }
		}	
	}
}
void AMPControllerPlayer::SelectItemTwoFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		if (value.Get<bool>())
		{
			if (HasAuthority()) { controlledBody->SelectItem(2); }
			else { Server_RequestSelectItem(2); }
		}
	}
}
void AMPControllerPlayer::SelectItemThreeFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		if (value.Get<bool>())
		{
			if (HasAuthority()) { controlledBody->SelectItem(3); }
			else { Server_RequestSelectItem(3); }
		}
	}
}
void AMPControllerPlayer::UseCurItemFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		if (value.Get<bool>())
		{
			if (HasAuthority()) { controlledBody->UseCurItem(); }
			else { Server_RequestUseCurItem(); }
		}
	}
}
void AMPControllerPlayer::DropCurItemFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		if (value.Get<bool>())
		{
			if (HasAuthority()) { controlledBody->DropCurItem(); }
			else { Server_RequestDropCurItem(); }
		}
	}
}
void AMPControllerPlayer::UseAbilityFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
        AMPCharacterCat* catBody = Cast<AMPCharacterCat>(controlledBody);
        if (catBody)
        {
			if (value.Get<bool>())
			{
				if (HasAuthority())
				{
					catBody->UseActiveAbility();
				}
				else
				{
					Server_RequestUseAbility();
				}
			}
        }
    }
}

void AMPControllerPlayer::Server_RequestSpawnItem_Implementation(EMPItem ItemTag, const FVector& Location, const FRotator& Rotation)
{
    AMPGMGameplay* GM = Cast<AMPGMGameplay>(GetWorld()->GetAuthGameMode());
    if (GM && HasAuthority())
    {
        GM->SpawnItem(ItemTag, Location, Rotation);
    }
}

void AMPControllerPlayer::Server_RequestSpawnEnvironment_Implementation(EEnvActor EnvTag, const FVector& Location, const FRotator& Rotation)
{
    AMPGMGameplay* GM = Cast<AMPGMGameplay>(GetWorld()->GetAuthGameMode());
    if (GM && HasAuthority())
    {
        GM->SpawnEnvironment(EnvTag, Location, Rotation);
    }
}

void AMPControllerPlayer::Server_RequestSpawnAbility_Implementation(EAbility AbilityTag, AActor* AbilityOwner)
{
    AMPGMGameplay* GM = Cast<AMPGMGameplay>(GetWorld()->GetAuthGameMode());
    if (GM && HasAuthority())
    {
        GM->SpawnAbility(AbilityOwner, AbilityTag);
    }
}

// =====================
// Gameplay action RPCs
// =====================

void AMPControllerPlayer::Server_RequestInteract_Implementation()
{
    if (controlledBody)
    {
        controlledBody->Interact();
    }
}

void AMPControllerPlayer::Server_RequestSelectItem_Implementation(int ItemIndex)
{
    if (controlledBody)
    {
        controlledBody->SelectItem(ItemIndex);
    }
}

void AMPControllerPlayer::Server_RequestUseCurItem_Implementation()
{
    if (controlledBody)
    {
        controlledBody->UseCurItem();
    }
}

void AMPControllerPlayer::Server_RequestDropCurItem_Implementation()
{
    if (controlledBody)
    {
        controlledBody->DropCurItem();
    }
}

void AMPControllerPlayer::Server_RequestUseAbility_Implementation()
{
    if (controlledBody)
    {
        AMPCharacterCat* catBody = Cast<AMPCharacterCat>(controlledBody);
        if (catBody)
        {
            catBody->UseActiveAbility();
        }
    }
}

// =====================
// Lobby Methods
// =====================

void AMPControllerPlayer::SetReadyState(bool inIsReady)
{
	ServerSetReadyState(inIsReady);
}

bool AMPControllerPlayer::GetReadyState() const
{
	AMPPlayerState* thePlayerState = Cast<AMPPlayerState>(PlayerState);
	if (thePlayerState)
	{
		return thePlayerState->isPlayerReady;
	}
	return false;
}

void AMPControllerPlayer::RequestAddBot(ETeam team)
{
	ServerRequestAddBot(team);
}

void AMPControllerPlayer::RequestRemoveBot(int32 playerIndex)
{
	ServerRequestRemoveBot(playerIndex);
}

// Server RPCs for lobby functionality
void AMPControllerPlayer::ServerSetReadyState_Implementation(bool inIsReady)
{
	// Validate authority
	if (!HasAuthority())
	{
		UManagerLog::LogWarning(TEXT("ServerSetReadyState called without authority"), TEXT("MPControllerPlayer"));
		return;
	}
	
	// Call GameMode to handle ready state
	AMPGMGameplay* gameMode = Cast<AMPGMGameplay>(GetWorld()->GetAuthGameMode());
	if (gameMode && gameMode->GetManagerLobby())
	{
		bool success = gameMode->GetManagerLobby()->SetPlayerReady(this, inIsReady);
		ClientReadyStateResult(inIsReady, success);
	}
	else
	{
		UManagerLog::LogError(TEXT("Could not get GameMode for ready state update"), TEXT("MPControllerPlayer"));
		ClientReadyStateResult(inIsReady, false);
	}
}

void AMPControllerPlayer::ServerRequestAddBot_Implementation(ETeam team)
{
	// Validate authority
	if (!HasAuthority())
	{
		UManagerLog::LogWarning(TEXT("ServerRequestAddBot called without authority"), TEXT("MPControllerPlayer"));
		return;
	}
	
	// Check if this player is the host
	UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
	if (!gameInstance || !gameInstance->IsHost())
	{
		UManagerLog::LogWarning(TEXT("Non-host player attempted to add bot"), TEXT("MPControllerPlayer"));
		ClientAddBotResult(team, false);
		return;
	}
	
	// Call GameMode to handle bot addition
	AMPGMGameplay* gameMode = Cast<AMPGMGameplay>(GetWorld()->GetAuthGameMode());
	if (gameMode)
	{
		bool success = gameMode->AddBot(team);
		ClientAddBotResult(team, success);
	}
	else
	{
		UManagerLog::LogError(TEXT("Could not get GameMode for bot addition"), TEXT("MPControllerPlayer"));
		ClientAddBotResult(team, false);
	}
}

void AMPControllerPlayer::ServerRequestRemoveBot_Implementation(int32 playerIndex)
{
	// Validate authority
	if (!HasAuthority())
	{
		UManagerLog::LogWarning(TEXT("ServerRequestRemoveBot called without authority"), TEXT("MPControllerPlayer"));
		return;
	}
	
	// Check if this player is the host
	UMPGI* gameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
	if (!gameInstance || !gameInstance->IsHost())
	{
		UManagerLog::LogWarning(TEXT("Non-host player attempted to remove bot"), TEXT("MPControllerPlayer"));
		ClientRemoveBotResult(playerIndex, false);
		return;
	}
	
	// Call GameMode to handle bot removal
	AMPGMGameplay* gameMode = Cast<AMPGMGameplay>(GetWorld()->GetAuthGameMode());
	if (gameMode)
	{
		bool success = gameMode->RemoveBot(playerIndex);
		ClientRemoveBotResult(playerIndex, success);
	}
	else
	{
		UManagerLog::LogError(TEXT("Could not get GameMode for bot removal"), TEXT("MPControllerPlayer"));
		ClientRemoveBotResult(playerIndex, false);
	}
}

// Client RPCs for lobby functionality
void AMPControllerPlayer::ClientReadyStateResult_Implementation(bool inIsReady, bool success)
{
	if (success)
	{
		UManagerLog::LogInfo(FString::Printf(TEXT("Ready state updated successfully: %s"), 
			inIsReady ? TEXT("Ready") : TEXT("Not Ready")), TEXT("MPControllerPlayer"));
		
		// Update HUD if available
		if (ManagerLobbyHUD)
		{
			ManagerLobbyHUD->UpdateReadyState(inIsReady);
		}
	}
	else
	{
		UManagerLog::LogWarning(FString::Printf(TEXT("Failed to update ready state: %s"), 
			inIsReady ? TEXT("Ready") : TEXT("Not Ready")), TEXT("MPControllerPlayer"));
	}
}

void AMPControllerPlayer::ClientAddBotResult_Implementation(ETeam team, bool success)
{
	if (success)
	{
		UManagerLog::LogInfo(FString::Printf(TEXT("Successfully added bot to team: %d"), (int32)team), TEXT("MPControllerPlayer"));
		// HUD will be updated via GameMode's ClientUpdateLobbyHUDs RPC
	}
	else
	{
		UManagerLog::LogWarning(FString::Printf(TEXT("Failed to add bot to team: %d"), (int32)team), TEXT("MPControllerPlayer"));
	}
}

void AMPControllerPlayer::ClientRemoveBotResult_Implementation(int32 playerIndex, bool success)
{
	if (success)
	{
		UManagerLog::LogInfo(FString::Printf(TEXT("Successfully removed bot at index: %d"), playerIndex), TEXT("MPControllerPlayer"));
		// HUD will be updated via GameMode's ClientUpdateLobbyHUDs RPC
	}
	else
	{
		UManagerLog::LogWarning(FString::Printf(TEXT("Failed to remove bot at index: %d"), playerIndex), TEXT("MPControllerPlayer"));
	}
}

void AMPControllerPlayer::FocusPreviewCamera()
{
    if (PreviewSlotIndex < 0) return;
    AMPGMGameplay* GM = Cast<AMPGMGameplay>(GetWorld()->GetAuthGameMode());
    if (!GM) return;
    if (!GM->GetManagerPreview()->GetPreviewCharacters().IsValidIndex(PreviewSlotIndex)) return;
    AActor* PreviewChar = GM->GetManagerPreview()->GetPreviewCharacters()[PreviewSlotIndex];
    if (!PreviewChar) return;
    UCameraComponent* PreviewCam = PreviewChar->FindComponentByClass<UCameraComponent>();
    if (PreviewCam)
    {
        SetViewTargetWithBlend(PreviewChar);
    }
}

void AMPControllerPlayer::FocusGameplayCamera()
{
    if (controlledBody)
    {
        UCameraComponent* GameplayCam = controlledBody->FindComponentByClass<UCameraComponent>();
        if (GameplayCam)
        {
            SetViewTargetWithBlend(controlledBody);
        }
    }
}