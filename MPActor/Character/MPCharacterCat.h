#pragma once

#include "CoreMinimal.h"
#include "MPCharacter.h"

#include "MPCharacterCat.generated.h"

enum class EAbility : uint8;
class UMPAbility;
enum class ECatAction : uint8;
class AMPCharacterHuman;

UCLASS(BlueprintType, Blueprintable)
class AMPCharacterCat : public AMPCharacter
{
    GENERATED_BODY()

public :
    AMPCharacterCat();

    virtual void BeginPlay() override;

    virtual void Tick(float deltaTime) override;

// cat move function 
protected:
    // --- Movement Smoothing & Drift Variables ---
    float CurrentMoveInputStrength = 0.0f;    // Smoothed input (0.0 = stopped, 1.0 = full)
    FVector DesiredMoveDirection = FVector::ZeroVector;  // Latest input direction (world space)
    bool bHasMoveInput = false;                 // True if input is currently active

    // Drift settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Movement")
    float DriftStartAngle = 45.0f;              // Minimum angle (degrees) to trigger drift

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Movement")
    float MinSpeedForDrift = 600.0f;            // Minimum speed to enable drift

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Movement")
    float DriftTurnRate = 2.0f;                 // Base turn rate while drifting

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Movement")
    float DriftSlowdownMultiplier = 0.5f;       // Initial multiplier (speed reduced while starting to drift)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Movement")
    float DriftRecoveryTime = 1.5f;             // Time (seconds) to fully recover drift momentum

    bool bIsDrifting = false;                   // True if currently in a drift state
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Movement")
    float DriftElapsedTime = 0.0f;              // How long we've been drifting (for momentum recovery)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Movement")
    float DriftRecoveryAlpha = 1.0f;            // Interpolated drift multiplier (0.5 → 1.0 over time)

    // For tracking current movement state (for drift direction)
    FVector LastVelocityDir = FVector::ZeroVector;  
    float CurrentSpeed = 0.0f;

    // Momentum after drifting
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Momentum")
    float DriftSlowdownMultiplier = 0.5f; // move at 50% speed while drifting
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Momentum")
    float DriftRecoveryTime = 1.5f; // time to recover full speed

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Movement")
    float DriftElapsedTime = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Movement")
    float DriftRecoveryAlpha = 1.0f; // 0 = slow, 1 = full speed


    UFUNCTION(BlueprintCallable, Category = "Ability Method")
    void UpdateDriftStatus();

    virtual void Move(FVector2D direction) override;
    virtual void MoveStop() override;
// double jump
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        bool IsFootNearWall();

    virtual bool CheckIfIsAbleToDoubleJump() override;


// ability
protected:    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Properties")
    UMPAbility* activeAbility;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Properties")
    TArray<EAbility> initPassiveAbilities;

    UPROPERTY(BlueprintReadWrite, Category = "Ability Properties")
    TArray<UMPAbility*> allPassiveAbilities;


    UFUNCTION(BlueprintCallable, Category = "Ability Method")
    void InitializeAllAbility();

    UFUNCTION(BlueprintCallable, Category = "Ability Method")
    void InitializeActiveAbility();
    UFUNCTION(BlueprintCallable, Category = "Ability Method")
    void InitializeAllPassiveAbility();
    
public :
    UFUNCTION(BlueprintCallable, Category = "Ability Method")
    void UseActiveAbility();
    
    UFUNCTION(BlueprintCallable, Category = "Ability Method")
    void UsePassiveAbility(EAbility abilityType);
    
// interactable interface
protected :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FText humanInteractHintText;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FText catInteractHintText;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FText uninteractableHumanHintText;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FText uninteractableCatHintText;

public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual bool IsInteractable(AMPCharacter* player) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual FText GetInteractHintText(AMPCharacter* player) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual void BeInteracted(AMPCharacter* player) override;

// controller direct relation
protected:
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
        ECatAction curCatAction;

    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    bool isBeingHolded = false;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    AMPCharacterHuman* humanHolding;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
    float holdTotalTime;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    float curHoldTime;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    float curHoldPercentage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
    float holdModifier;

    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    bool isRubbing = false;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    AMPCharacterHuman* humanRubbing;

public :
    virtual bool CheckIfIsAbleToLook() override;
    virtual bool CheckIfIsAbleToMove() override;
    virtual bool CheckIfIsAbleToInteract() override;

    virtual void Move(FVector2D direction) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Properties")
    float rotationRate = 400.0f;
    /*
    UFUNCTION(BlueprintCallable, Category = "Tick Method")
    void UpdateRotation(FVector2D Direction);*/

public :
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StartedToBeHold(AMPCharacter* humanPlayer);
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void EndToBeHold();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void Straggle();

    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StartToRub(AMPCharacterHuman* humanToRub);
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StopToRub();

public:
    virtual void Interact() override;

// animation
protected:
    // all animations montages
    
public:
    UFUNCTION(BlueprintCallable, Category = "Animation Method")
    void PlayCatAnimMontage(ECatAnimMontage aMontage);

};
