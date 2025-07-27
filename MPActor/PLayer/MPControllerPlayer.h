#pragma once

// [Meow-Phone Project]
//
// This is the one and only Player Controller class for a human player. It is a critical,
// central hub that handles three major areas of responsibility:
// 1. **Input Handling**: It binds player input (from keyboard, mouse, or gamepad) via the Enhanced Input system to specific gameplay actions (Move, Jump, Interact, etc.).
// 2. **UI Management**: It is responsible for creating, showing, hiding, and holding references to ALL of the game's UI widgets (HUDs).
// 3. **Client-to-Server Communication**: It is the primary vehicle for a client to communicate their intentions to the server. When a player presses a button to perform a replicated action (like using an item or setting their ready status), the input function here calls a `Server_` RPC, sending the request to the server for validation and execution.
//
// How to utilize in Blueprint:
// 1. A Blueprint must be created from this class (`BP_ControllerPlayer`).
// 2. This Blueprint must be set as the `PlayerController Class` in your Game Modes (`AMPGMInit` and `AMPGMGameplay`).
// 3. In the `BP_ControllerPlayer` Blueprint's defaults, you MUST configure:
//    - **HUD Classes**: All of the `...HUDClass` properties must be assigned with their corresponding UI Widget Blueprints. This controller will not be able to create any UIs if these are not set.
//    - **Input Actions**: All of the `UInputAction` properties must be assigned with the correct Input Action assets from your project.
//    - **Input Mapping Context**: The `gpGamppingContext` must be assigned with your primary IMC.
//
// Necessary things to define:
// - All `TSubclassOf<U...HUD>` properties MUST be set in the Blueprint.
// - All `UInputAction` properties and the `UInputMappingContext` MUST be set in the Blueprint.
//
// How it interacts with other classes:
// - APlayerController: The base class.
// - Enhanced Input System (`UInputMappingContext`, `UInputAction`): Binds hardware input to gameplay functions.
// - HUD Widgets (`UHUDInit`, `UHUDLobby`, etc.): It creates and manages the lifecycle of all UI screens. The UI, in turn, often calls functions on this Player Controller to send requests to the server.
// - Server RPCs: It is filled with `UFUNCTION(Server, Reliable)` functions. These are the bridge from the client to the server. For example, `InteractFunc` is called by local input, which then calls `Server_RequestInteract` to ask the server to perform the action.
// - AMPCharacter: When in a match, it possesses a character pawn. The input functions here (e.g., `MoveFunc`) directly call the corresponding functions on the possessed pawn (e.g., `controlledBody->Move()`).
// - Game Mode / Player State: It communicates with server-side objects like the Game Mode to execute requests (e.g., `ServerSetReadyState` is received by the controller's server instance, which then calls a function on the `UManagerLobby`).

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MPControllerPlayer.generated.h"

enum class EHUDType : uint8;

class UHUDInit;
class UHUDOption;
class UHUDSessionGeneral;
class UHUDCredit;
class UHUDCreateSession;
class UHUDSearchSession;
class UHUDLobby;
class UHUDManagerLobby;
class UHUDHuman;
class UHUDCharacterCat;
class UHUDCharacterHuman;
class UHUDMenu;
class UHUDEnd;

class AMPCharacter;
class UInputMappingContext;
class UInputAction;

struct FInputActionValue;

// Forward declarations of gameplay enums used in RPCs
enum class EMPItem : uint8;
enum class EEnvActor : uint8;
enum class EAbility : uint8;

UCLASS(BlueprintType, Blueprintable)
class MEOWPHONE_API AMPControllerPlayer : public APlayerController
{
    GENERATED_BODY()

public :
    AMPControllerPlayer();

// common player controller properties
protected :
    UFUNCTION(BlueprintCallable, Category = "Player State Method")
        virtual void BeginPlay() override;

public :
    UFUNCTION(BlueprintCallable, Category = "Player State Method")
        void InitializePS(int aPlayerIndex);

    	// Team assignment methods
	UFUNCTION(BlueprintCallable, Category = "Team Methods")
	void RequestTeamSwitch(ETeam newTeam);
    UFUNCTION(BlueprintCallable, Category = "Team Methods")
        ETeam GetCurrentTeam() const;
    UFUNCTION(BlueprintCallable, Category = "Team Methods")
        bool IsTeamAssigned() const;

    // Lobby methods
    UFUNCTION(BlueprintCallable, Category = "Lobby Methods")
        void SetReadyState(bool inIsReady);
    UFUNCTION(BlueprintCallable, Category = "Lobby Methods")
        bool GetReadyState() const;
    UFUNCTION(BlueprintCallable, Category = "Lobby Methods")
        void RequestAddBot(ETeam team);
    UFUNCTION(BlueprintCallable, Category = "Lobby Methods")
        void RequestRemoveBot(int32 playerIndex);

    	// Server RPCs for team assignment
	UFUNCTION(Server, Reliable)
		void ServerRequestTeamSwitch(ETeam newTeam);

    // Server RPCs for lobby functionality
    UFUNCTION(Server, Reliable)
        void ServerSetReadyState(bool inIsReady);
    UFUNCTION(Server, Reliable)
        void ServerRequestAddBot(ETeam team);
    UFUNCTION(Server, Reliable)
        void ServerRequestRemoveBot(int32 playerIndex);

    	// Client RPCs for team assignment results
	UFUNCTION(Client, Reliable)
		void ClientTeamSwitchResult(ETeam newTeam, bool success);

    // Client RPCs for lobby functionality
    UFUNCTION(Client, Reliable)
        void ClientReadyStateResult(bool inIsReady, bool success);
    UFUNCTION(Client, Reliable)
        void ClientAddBotResult(ETeam team, bool success);
    UFUNCTION(Client, Reliable)
        void ClientRemoveBotResult(int32 playerIndex, bool success);

// hud manager

protected :
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDInit* initHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDOption* optionHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDSessionGeneral* sessionGeneralHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDCredit* creditHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDCreateSession* createSessionHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDSearchSession* searchSessionHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDManagerLobby* ManagerLobbyHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDCharacterHuman* humanHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDCharacterCat* catHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDMenu* menuHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDEnd* endHUD;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UHUDInit> initHUDClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UHUDOption> optionHUDClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UHUDSessionGeneral> sessionGeneralHUDClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UHUDCreateSession> createSessionHUDClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UHUDSearchSession> searchSessionHUDClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UHUDManagerLobby> ManagerLobbyHUDClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UHUDCharacterHuman> humanHUDClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UHUDCharacterCat> catHUDClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UHUDMenu> menuHUDClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UHUDEnd> endHUDClass;
    
    UFUNCTION(BlueprintCallable, Category = "HUD Method")
        void TurnUIInputOn();
    UFUNCTION(BlueprintCallable, Category = "HUD Method")
        void TurnGameplayInputOn();

public :
    UFUNCTION(BlueprintCallable, Category = "HUD Method")
        void AttachHUD(EHUDType hudType, int zOrder);
    UFUNCTION(BlueprintCallable, Category = "HUD Method")
        void RemoveHUD(EHUDType hudType);

	UFUNCTION(BlueprintCallable, Category = "HUD Method")
	UHUDManagerLobby* GetManagerLobbyHUD() const { return ManagerLobbyHUD; }
    UFUNCTION(BlueprintCallable, Category = "HUD Method")
    UHUDEnd* GetEndHUD() const { return endHUD; }
    UFUNCTION(BlueprintCallable, Category = "HUD Method")
	UHUDSearchSession* GetSearchSessionHUD() const { return searchSessionHUD; }
    UFUNCTION(BlueprintCallable, Category = "HUD Method")
	UHUDOption* GetOptionHUD() const { return optionHUD; }

// game progress update
public :
    UFUNCTION(BlueprintCallable, Category = "GameProgress Method")
        void LobbyStartUpdate();
    UFUNCTION(BlueprintCallable, Category = "GameProgress Method")
        void CharacterCustomStartUpdate();
    UFUNCTION(BlueprintCallable, Category = "GameProgress Method")
        void PrepareStartUpdate();
    UFUNCTION(BlueprintCallable, Category = "GameProgress Method")
        void GameplayStartUpdate();
       
    UFUNCTION(BlueprintCallable, Category = "GameProgress Method")
        void UpdateLobbyHUDCountdownText(int secondRemaining);
// character relation
protected :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Properties")
        AMPCharacter* controlledBody;
    
public :
    UFUNCTION(BlueprintCallable, Category = "Character Method")
        virtual void OnPossess(APawn* inPawn) override;
    
    UFUNCTION(BlueprintCallable, Category = "Control")
        void UnPossessEffect();
    
    UFUNCTION(BlueprintCallable, Category = "Character Method")
        void ControlledBodyDied();

// mapping input actions
protected :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        UInputMappingContext* gpGamppingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        UInputAction* openMenuAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        UInputAction* lookAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        float lookXSensitive;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        float lookYSensitive;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        UInputAction* moveAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        UInputAction* runAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        UInputAction* jumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        UInputAction* crouchAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        UInputAction* interactAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        UInputAction* selectItemOneAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        UInputAction* selectItemTwoAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        UInputAction* selectItemThreeAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        UInputAction* useCurItemAction;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        UInputAction* dropCurItemAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input Properties")
        UInputAction* useAbilityAction;
    
public :
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void SetupMappingContext();
    virtual void SetupInputComponent() override;
    
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void OpenMenuFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void LookFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void MoveFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void MoveEndFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void RunFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void RunEndFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void JumpFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void JumpEndFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void CrouchFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void CrouchEndFunc(const FInputActionValue& value);
        
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void InteractFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void SelectItemOneFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void SelectItemTwoFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void SelectItemThreeFunc(const FInputActionValue& value);

    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void UseCurItemFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void DropCurItemFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void UseAbilityFunc(const FInputActionValue& value);

    /* Gameplay action RPCs */
	UFUNCTION(Server, Reliable, Category = "Action")
	void Server_RequestInteract();

	UFUNCTION(Server, Reliable, Category = "Action")
	void Server_RequestSelectItem(int ItemIndex);

	UFUNCTION(Server, Reliable, Category = "Action")
	void Server_RequestUseCurItem();

	UFUNCTION(Server, Reliable, Category = "Action")
	void Server_RequestDropCurItem();
    
    /*
     * RPC request helpers – let the owning client ask the server (GameMode) to spawn
     * gameplay objects.  These run only on the server and forward to AMPGMGameplay’s
     * factory helpers.  Blueprints can call them directly.
     */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Spawn")
	void Server_RequestSpawnItem(EMPItem ItemTag, const FVector& Location, const FRotator& Rotation);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Spawn")
	void Server_RequestSpawnEnvironment(EEnvActor EnvTag, const FVector& Location, const FRotator& Rotation);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Spawn")
	void Server_RequestSpawnAbility(EAbility AbilityTag, AActor* AbilityOwner);
    
	UFUNCTION(Server, Reliable, Category = "Action")
	void Server_RequestUseAbility();
    
public:
    // Preview system
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Preview")
    int32 PreviewSlotIndex = -1;
    UFUNCTION(BlueprintCallable, Category = "Preview")
    void FocusPreviewCamera();
    UFUNCTION(BlueprintCallable, Category = "Preview")
    void FocusGameplayCamera();
};