#pragma once

#include "CoreMinimal.h"
#include "PlayerController.h"
#include "MPControllerPlayer.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMPControllerPlayer : public APlayerController
{
    GENERATED_BODY()

public :
    AMPControllerPlayer();

// common player controller properties
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Player State Properties")
        AMPPlayerState* playerState;

    UFUNCTION(BlueprintCallable, Category = "Player State Method")
        virtual void BeginPlay() override;
    
// hud manager
protected :
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UMPHUD* curPlayerHUD;
    
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UMPHUDHuman> humanHUDClass;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UMPHUDCat> catHUDClass;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        TSubclassOf<UMPHUDEndGP> endGPHUDClass;
    
public :
    UFUNCTION(BlueprintCallable, Category = "HUD Method")
        void SetupInitWidget();
    UFUNCTION(BlueprintCallable, Category = "HUD Method")
        void SetupEndGPWidget();

// character relation
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Character Properties")
        AMPCharacter* controlledBody;
    
public :
    UFUNCTION(BlueprintCallable, Category = "Character Method")
        virtual void OnPossess(APawn* inPawn) override;
    
    UFUNCTION(BlueprintCallable, Category = "Character Method")
        void ControlledBodyDied();
    UFUNCTION(BlueprintCallable, Category = "Character Method")
        virtual void UnPossess() override;
    
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
        UInputAction* useAbilityAction;
    
public :
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void SetupMappingContext();
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void SetupInputComponent(bool inGameplay);
    
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void OpenMenuFunc();
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void LookFunc();
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void MoveFunc();
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void RunFunc();
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void RunEndFunc();
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void JumpFunc();
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void JumpEndFunc();
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void InteractFunc();
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void SelectItemOneFunc();
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void SelectItemTwoFunc();
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void SelectItemThreeFunc();
    UFUNCTION(BlueprintCallable, Category = "Input Method")
        void UseAbilityFunc();
    
}