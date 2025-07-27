#pragma once

// [Meow-Phone Project]
//
// This is the specialized character class for the Cat. It inherits all the base functionality
// from `AMPCharacter` and adds a significant amount of cat-specific mechanics, including:
// - **Advanced Movement**: A double-jump/wall-jump mechanic.
// - **Human Interaction**: Systems for being held by a human, a "struggle" minigame to escape, and being "rubbed".
// - **Abilities**: A system to initialize and use active and passive abilities.
// - **Complex Animation State**: A much more detailed animation state machine (`FCatAnimState`) to handle cat-specific postures and actions (e.g., sitting, stretching, sleeping).
//
// How to utilize in Blueprint:
// 1. Create a Blueprint class inheriting from `AMPCharacterCat` (e.g., `BP_Cat`). This will be your main playable cat.
// 2. In the Blueprint's defaults, you MUST configure:
//    - **Skeletal Mesh**: The visual model for the cat.
//    - **Anim Class**: The Animation Blueprint that drives the cat's animations. This AnimBP must be specifically designed to work with the `FCatAnimState` struct.
//    - **Animation Montages**: Populate the large list of montage properties (e.g., `verticalJump_Montage`, `longFalling_Montage`) with the corresponding animation assets.
//    - **Hint Texts**: Set the various hint text properties for different interaction scenarios.
// 3. The `initPassiveAbilities` array can be populated with `EAbility` enums to grant the cat specific passive abilities at spawn.
//
// Necessary things to define:
// - All the `UAnimMontage` properties must be assigned in the Blueprint for the character's contextual animations to work.
// - The cat's Animation Blueprint must get the `AMPCharacterCat` owner and read the `animState` variable to drive its state machine.
//
// How it interacts with other classes:
// - AMPCharacter: Inherits all its base functionality.
// - AMPCharacterHuman: Has special interactions with the human character, such as being held (`StartedToBeHold`) or rubbed (`StartToBeRubbed`). The human character is the one who initiates these interactions.
// - AMPAbility: The character initializes and uses active and passive abilities.
// - FCatAnimState (Struct): This struct is the core of the cat's animation system. This class sets the values in the struct, and the Animation Blueprint reads them to play the correct animations.
// - Replication: `FCatAnimState` and the `struggleBar` are replicated, ensuring that all clients see the cat's animations and struggle progress correctly.

#include "CoreMinimal.h"
#include "MPCharacter.h"
#include "../../CommonStruct.h"
#include "MPCharacterCat.generated.h"

class AMPAbility;
class AMPCharacterHuman;
enum class EAbility : uint8;
enum class ECatAction : uint8;
struct FCatAnimState;
enum class ECatPosture : uint8;
enum class EMoveState : uint8;
enum class ECatContext : uint8;
enum class ECatInteractState : uint8;
enum class ECatContext : uint8;

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
    float GetStruggleBarPercentage();

    // State getters
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    bool IsBeingHeld();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    bool IsRubbing();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    AMPCharacterHuman* GetHumanHolding();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    AMPCharacterHuman* GetHumanRubbing();

// 5.6 ability related
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Properties")
    TArray<EAbility> initPassiveAbilities;
    UPROPERTY(BlueprintReadWrite, Category = "Ability Properties")
    TArray<AMPAbility*> allPassiveAbilities;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Properties")
    AMPAbility* activeAbility;

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
    virtual void SetMove(EMoveState newMove) override;
    virtual void SetAir(EAirState newAir) override;
    void SetContext(ECatContext newContext, bool bMandatory = false);
    void SetInteraction(ECatInteractionState newInteraction);

    void IdlePoseTimeout();

    UFUNCTION()
    void OnRep_AnimState();

    UFUNCTION(BlueprintPure, Category="AnimState")
    const FCatAnimState& GetAnimState() const { return animState; }
    
public:
    void BeginIdlePoseTimer();

// 6.2 animation context/ montage
// public
    void PlayCatContextAnimMontage(ECatContext aMontage);
    virtual void OnMontageEndedContextClear(UAnimMontage* montage, bool bInterrupted) override;

    // Cat-specific animation montages
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float beStunnedMontagePlayRate = 1.0f;
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