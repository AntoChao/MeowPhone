#include "MPControllerPlayer.h"

#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

// high level
#include "../../HighLevel/MPGMGameplay.h"

// input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// related
#include "MPPlayerState.h"
#include "../Character/MPCharacter.h"
#include "../Character/MPCharacterCat.h"

// hud
#include "Widget/HUDCat.h"
#include "Widget/HUDCreateSession.h"
#include "Widget/HUDCredit.h"
#include "Widget/HUDEnd.h"
#include "Widget/HUDHuman.h"
#include "Widget/HUDInit.h"
#include "Widget/HUDLobby.h"
#include "Widget/HUDMenu.h"
#include "Widget/HUDOption.h"
#include "Widget/HUDSearchSession.h"
#include "Widget/HUDSessionGeneral.h"

AMPControllerPlayer::AMPControllerPlayer()
{

}

// common player controller properties
void AMPControllerPlayer::BeginPlay() 
{
    Super::BeginPlay();
}

void AMPControllerPlayer::InitializePS(int aPlayerIndex)
{
    AMPPlayerState* curPlayerState = Cast<AMPPlayerState>(PlayerState);
    if (curPlayerState)
    {
        curPlayerState->playerIndex = aPlayerIndex;
        curPlayerState->playerTeam = ETeam::ECat;
        curPlayerState->catRace = ECatRace::ECatExp;
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
            if (!lobbyHUD && lobbyHUDClass)
            {
                lobbyHUD = CreateWidget<UHUDLobby>(this, lobbyHUDClass);
                if (lobbyHUD != nullptr)
                {
                    TurnUIInputOn();
                    lobbyHUD->AddToViewport(zOrder);
                    lobbyHUD->SetOwner(this);
                }
            }
            break;
        }
        case EHUDType::EGameplayHuman :
        {
            if (!humanHUD && humanHUDClass)
            {
                humanHUD = CreateWidget<UHUDHuman>(this, humanHUDClass);
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
                catHUD = CreateWidget<UHUDCat>(this, catHUDClass);
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
            if (lobbyHUD)
            {
                TurnGameplayInputOn();
                lobbyHUD->RemoveFromParent();
                lobbyHUD = nullptr;
            }
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
    if (value.Get<bool>()) 
    {
        AttachHUD(EHUDType::EMenu, 1);
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
			controlledBody->Interact();
		}
	}
}
void AMPControllerPlayer::SelectItemOneFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		if (value.Get<bool>())
		{
			controlledBody->SelectItem(1);
		}
	}
}
void AMPControllerPlayer::SelectItemTwoFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		if (value.Get<bool>())
		{
			controlledBody->SelectItem(2);
		}
	}
}
void AMPControllerPlayer::SelectItemThreeFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		if (value.Get<bool>())
		{
			controlledBody->SelectItem(3);
		}
	}
}
void AMPControllerPlayer::UseCurItemFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		if (value.Get<bool>())
		{
			controlledBody->UseCurItem();
		}
	}
}
void AMPControllerPlayer::DropCurItemFunc(const FInputActionValue& value)
{
    if (controlledBody) 
    {
		if (value.Get<bool>())
		{
			controlledBody->DropCurItem();
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
            catBody->UseActiveAbility();
        }
    }
}