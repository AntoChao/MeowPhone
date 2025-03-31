#pragma once

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
class UHUDCustomHuman;
class UHUDCustomCat;
class UHUDHuman;
class UHUDCat;
class UHUDMenu;
class UHUDEnd;

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
        void InitializePS(int aPlayerIndex);

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
        UHUDLobby* lobbyHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDCustomHuman* customHumanHUD;
        UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDCustomCat* customCatHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDHuman* humanHUD;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UHUDCat* catHUD;
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
        TSubclassOf<UHUDLobby> lobbyHUDClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UHUDCustomHuman> customHumanHUDClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UHUDCustomCat> customCatHUDClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UHUDHuman> humanHUDClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UHUDCat> catHUDClass;
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
    
};