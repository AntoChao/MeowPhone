#pragma once

#include "CoreMinimal.h"
#include "MPCharacter.h"

#include "MPCharacterCat.generated.h"

class UMPAbility;
class AMPCharacterHuman;
enum class EAbility : uint8;
enum class ECatAction : uint8;
struct FCatAnimState;

UCLASS(BlueprintType, Blueprintable)
class AMPCharacterCat : public AMPCharacter
{
    GENERATED_BODY()

// 1. all actor class methods
public:
    AMPCharacterCat();
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
    
// 2. interface
// 2.1 interactable
public:
    virtual bool IsInteractable(AMPCharacter* player) override;
    virtual void BeInteracted(AMPCharacter* player) override;

protected :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FText humanInteractHintText;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FText catInteractHintText;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FText uninteractableHumanHintText;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FText uninteractableCatHintText;

// 3. components
// 3.1 camera component
// 3.2 detect
protected:
    // Returns true if near wall, and outputs the wall normal
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    bool IsFootNearWall(FVector& OutWallNormal);
    
    // Tracks the last wall normal for double jump direction
    FVector LastWallNormal;

// 4. common properties

// 5. Controller/input reaction
public:
    virtual void Move(FVector2D direction) override;
    virtual void MoveStop() override;
    virtual void Run() override;
    virtual void RunStop() override;
    virtual void CrouchStart();
    virtual void CrouchEnd();
    virtual void JumpStart() override;
    virtual void JumpEnd() override;
    void PerformDoubleJump();
    virtual void Interact() override;
    
// 5.1 controller enable/disable
    virtual bool CheckIfIsAbleToLook() override;
    virtual bool CheckIfIsAbleToMove() override;
    virtual bool CheckIfIsAbleToInteract() override;
    virtual bool CheckIfIsAbleToUseItems() override;
    virtual bool CheckIfIsAbleToRun() override;
    virtual bool CheckIfIsAbleToCrouch() override;
    virtual bool CheckIfIsAbleToJump() override;
    virtual bool CheckIfIsAbleToDoubleJump() override;
    
// 5.2 movement related
    // Double jump tracking
    int doubleJumpCount = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
    float jumpStrength = 600.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
    float blendAlpha = 0.5f;

// 5.3 jump related
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
    float wallDetectionHeightPercentage = 0.8f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
    float wallDetectDownward = -10.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
    float wallDetectRadius = 15.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
    float wallDetectionAngleTolerance = 0.3f;

// 5.4 air related
    bool bWasGroundedLastTick = true;
    float airTime = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimState")
    float longFallingHeightThreshold = 400.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimState")
    float fallTraceDistance = 10000.0f;
    float fallStartZ = 0.0f;
    bool bIsFalling = false;
    
    // Air state management
    virtual void UpdateMovingControlsPerTick(float deltaTime) override;
    
// 5.5 interaction related
    // Struggle bar for being held
    UPROPERTY(ReplicatedUsing = OnRep_StruggleBar, EditAnywhere, BlueprintReadWrite, Category = "ControlState")
    float struggleBar = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ControlState")
    float struggleBarMax = 100.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ControlState")
    float struggleBarPerInput = 10.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Properties")
    float rotationRate = 400.0f;

    UFUNCTION()
    void OnRep_StruggleBar();

    void StartedToBeHold(AMPCharacter* humanPlayer);
    void Straggle();
    void EndToBeHold();
    void StartToBeRubbed(AMPCharacterHuman* humanToRub);
    void StopToBeRubbed();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hold Animation")
    FName holdAnimLeftHandSocketName = "holdAnimLeftHandSocket";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hold Animation")
    FName holdAnimRightHandSocketName = "holdAnimRightHandSocket";

    UFUNCTION(BlueprintCallable, Category = "Hold Animation")
    FVector GetHoldAnimLeftHandPosition() const;
    UFUNCTION(BlueprintCallable, Category = "Hold Animation")
    FVector GetHoldAnimRightHandPosition() const;

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
    AMPCharacterHuman* humanRubbing;

    // Struggle bar getter for UI
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    float GetStruggleBarPercentage() const { return struggleBarMax > 0.0f ? struggleBar / struggleBarMax : 0.0f; }

    // State getters
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    bool IsBeingHeld() const { return animState.curInteraction == ECatInteractionState::BeingHeld; }
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    bool IsRubbing() const { return animState.curInteraction == ECatInteractionState::BeingRubbed; }
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    AMPCharacterHuman* GetHumanHolding() const { return IsValid(humanHolding) ? humanHolding : nullptr; }
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    AMPCharacterHuman* GetHumanRubbing() const { return IsValid(humanRubbing) ? humanRubbing : nullptr; }

// 5.6 ability related
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Properties")
    TArray<EAbility> initPassiveAbilities;
    UPROPERTY(BlueprintReadWrite, Category = "Ability Properties")
    TArray<UMPAbility*> allPassiveAbilities;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Properties")
    UMPAbility* activeAbility;

    void InitializeAllAbility();
    void InitializeActiveAbility();
    void InitializeAllPassiveAbility();
    void UseActiveAbility();
    void UsePassiveAbility(EAbility abilityType);

// 6. Animation system
// 6.1 animation state
protected:
    UPROPERTY(ReplicatedUsing = OnRep_AnimState, BlueprintReadOnly, Category = "AnimState")
    FCatAnimState animState;
    
    FTimerHandle idlePoseTimerHandle;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimState")
    float idlePoseMinTime = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimState")
    float idlePoseMaxTime = 3.0f;

    void SetPosture(ECatPosture newPosture);
    void SetMove(EMoveState newMove);
    void SetAir(EAirState newAir);
    void SetContext(ECatContext newContext, bool bMandatory = false);
    void SetInteraction(ECatInteractionState newInteraction);

    void BeginIdlePoseTimer();
    void IdlePoseTimeout();

    UFUNCTION()
    void OnRep_AnimState();

    UFUNCTION(BlueprintPure, Category="AnimState")
    const FCatAnimState& GetAnimState() const { return animState; }
    
// 6.2 animation context/ montage
// public
    UFUNCTION(BlueprintCallable, Category = "Animation Method")
    void PlayCatContextAnimMontage(ECatContext aMontage);
    UFUNCTION(BlueprintCallable, Category = "Animation Methods")
    virtual void OnMontageEndedContextClear(UAnimMontage* montage, bool bInterrupted) override;

    // Cat-specific animation montages
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* beStunned_Montage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float verticalJump_MontagePlayRate = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* verticalJump_Montage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float longFalling_MontagePlayRate = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* longFalling_Montage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float standStretch_MontagePlayRate = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* standStretch_Montage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float sitScratch_MontagePlayRate = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* sitScratch_Montage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float sitPaw_MontagePlayRate = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* sitPaw_Montage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float sitBiscuit_MontagePlayRate = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* sitBiscuit_Montage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float sitDrinkWater_MontagePlayRate = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* sitDrinkWater_Montage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float lyingSleepSpot_MontagePlayRate = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* lyingSleepSpot_Montage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float beStunned_MontagePlayRate = 1.0f;

// 7. special condition
public:
    virtual void BeStunned(int32 stunDuration) override;
    virtual void StopStunned() override;
};