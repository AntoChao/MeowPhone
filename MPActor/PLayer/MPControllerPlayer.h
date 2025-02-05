#pragma once

#include "CoreMinimal.h"
#include "PlayerController.h"
#include "MPControllerPlayer.generated.h"

enum class EHUDType : uint8;

class UMPHUDInit;
class UMPHUDOption;
class UMPHUDSessionGeneral;
class UMPHUDCredit;
class UMPHUDCreateSession;
class UMPHUDSearchSession;
class UMPHUDLobby;
class UMPHUDHuman;
class UMPHUDCat;
class UMPHUDMenu;
class UMPHUDEnd;

class AMPCharacter;
class UInputMappingContext;
class UInputAction;

struct FInputActionValue;

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
        void InitializePS(int playerIndex);

// hud manager

protected :
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UMPHUDInit* initHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UMPHUDOption* optionHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UMPHUDSessionGeneral* sessionGeneralHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UMPHUDCredit* creditHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UMPHUDCreateSession* createSessionHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UMPHUDSearchSession* searchSessionHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UMPHUDLobby* lobbyHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UMPHUDHuman* humanHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UMPHUDCat* catHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UMPHUDMenu* menuHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UMPHUDEnd* endHUD;

    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UMPHUDInit> initHUDClass;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UMPHUDOption> optionHUDClass;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UMPHUDSessionGeneral> sessionGeneralHUDClass;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UMPHUDCreateSession> createSessionHUDClass;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UMPHUDSearchSession> searchSessionHUDClass;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UMPHUDLobby> lobbyHUDClass;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UMPHUDHuman> humanHUDClass;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UMPHUDCat> catHUDClass;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UMPHUDMenu> menuHUDClass;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UMPHUDEnd> endHUDClass;
    
    UFUNCTION(BlueprintCallable, Category = "HUD Method")
        void TurnUIInputOn();
    UFUNCTION(BlueprintCallable, Category = "HUD Method")
        void TurnGameplayInputOn();

public :
    UFUNCTION(BlueprintCallable, Category = "HUD Method")
        void AttachHUD(EHUDType hudType, int zOrder);
    UFUNCTION(BlueprintCallable, Category = "HUD Method")
        void RemoveHUD(EHUDType hudType);

// game progress update
public :
    UFUNCTION(BlueprintCallable, Category = "GameProgress Method")
        void LobbyStartUpdate();
    UFUNCTION(BlueprintCallable, Category = "GameProgress Method")
        void PrepareStartUpdate();
    UFUNCTION(BlueprintCallable, Category = "GameProgress Method")
        void GameplayStartUpdate();
    
// character relation
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Character Properties")
        AMPCharacter* controlledBody;
    
public :
    UFUNCTION(BlueprintCallable, Category = "Character Method")
        virtual void OnPossess(APawn* inPawn) override;
    
    UFUNCTION(BlueprintCallable, Category = "Character Method")
        virtual void UnPossess() override;
    UFUNCTION(BlueprintCallable, Category = "Character Method")
        void ControlledBodyDied();

// mapping input actions
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        UInputMappingContext* gpGamppingContext;

    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        UInputAction* openMenuAction;
    
    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        UInputAction* lookAction;
    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        int lookXSensitive;
    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        int lookYSensitive;
    
    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        UInputAction* moveAction;
    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        int moveXSensitive;
    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        int moveYSensitive;

    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        UInputAction* runAction;
    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        UInputAction* jumpAction;
    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        UInputAction* interactAction;
    
    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        UInputAction* selectItemOneAction;
    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        UInputAction* selectItemTwoAction;
    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        UInputAction* selectItemThreeAction;
    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        UInputAction* useCurItemAction;
    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        UInputAction* dropCurItemAction;

    UPROPERTY(BlueprintReadWrite, Category = "Input Properties")
        UInputAction* useAbilityAction;
    
public :
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void SetupMappingContext();
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void SetupInputComponent();
    
    UFUNCTION(Bluepr  intCallable, Category = "Input Method")
        void OpenMenuFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void LookFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void MoveFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void RunFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void RunEndFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void JumpFunc(const FInputActionValue& value);
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void JumpEndFunc(const FInputActionValue& value);
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
    
}