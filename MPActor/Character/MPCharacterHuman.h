#pragma once

// [Meow-Phone Project]
//
// This is the specialized character class for the Human. It inherits from `AMPCharacter` and
// implements all human-specific gameplay mechanics, which primarily revolve around interacting
// with and catching cats. Key features include:
// - **Health System**: A simple health/lives system where the human can take damage and "die".
// - **Cat Interaction**: The logic for catching a cat (`StartHoldingCat`), carrying it, and placing it in a cage (`PutCatInCage`). This includes handling the cat's struggle attempts (`ForceReleaseCat`).
// - **Animation State**: Its own animation state machine (`FHumanAnimState`) to drive human-specific animations.
// - **Motion Warping**: A detailed system for using Motion Warping to ensure the "hold cat" animation aligns correctly with the captured cat actor.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint class inheriting from `AMPCharacterHuman` (e.g., `BP_Human`).
// 2. In the Blueprint's defaults, you MUST configure:
//    - **Skeletal Mesh** and **Anim Class**: The visual model and Animation Blueprint for the human. The AnimBP must be designed to work with the `FHumanAnimState` struct.
//    - **Animation Montages**: Assign all the necessary montage assets (`holdAnimMontage`, `putCatInCage_Montage`, etc.).
// 3. The `maxHealth` property can be configured in the Blueprint.
//
// Necessary things to define:
// - All `UAnimMontage` properties must be assigned for animations to work.
// - The Human's Animation Blueprint must read the `animState` variable to drive its logic.
// - Sockets (like `holdAnimChestSocket`) must exist on the human's skeleton for motion warping and attachments to work correctly.
//
// How it interacts with other classes:
// - AMPCharacter: Inherits all base functionality.
// - AMPCharacterCat: The primary target of the human's special interactions. The human can call functions on the cat, like `StartedToBeHold`, and the cat can call functions back, like `Straggle`, which results in the human calling `ForceReleaseCat`.
// - UMotionWarpingComponent: Used extensively in `holdAnimSnapping` to align the human's animation to the cat being held, making the interaction look natural regardless of the exact positions.
// - FHumanAnimState (Struct): Contains all the replicated state variables needed to drive the human's animation blueprint.
// - Replication: `currentHealth`, `isDead`, and `animState` are all replicated to ensure clients have an accurate view of the human's status and animations.

#include "CoreMinimal.h"
#include "MPCharacter.h"

#include "MPCharacterHuman.generated.h"

enum class EHumanAction : uint8;
class AMPCharacterCat;
enum class EHumanPosture : uint8;
enum class EMoveState : uint8;
enum class EHumanInteractionState : uint8;

UCLASS(BlueprintType, Blueprintable)
class AMPCharacterHuman : public AMPCharacter
{
    GENERATED_BODY()

// 1. all actor class methods
public:
    AMPCharacterHuman();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;

// 2. interface
// 2.1 interactable
public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual bool IsInteractable(AMPCharacter* player) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual void BeInteracted(AMPCharacter* player) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FText catInteractHintText;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FText uninteractableCatHintText;

// 2.2 sound 
// 3. components
// 3.1 camera component
// 4. common properties
// 4.1 health system
protected:
    UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadWrite, Category = "Health Properties")
    int32 currentHealth = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Properties")
    int32 maxHealth = 3;
    UPROPERTY(ReplicatedUsing = OnRep_IsDead, BlueprintReadWrite, Category = "Health Properties")
    bool isDead = false;

   
    UFUNCTION(BlueprintCallable, Category = "Health Methods")
    void HealHealth(int32 healAmount);
    UFUNCTION(BlueprintCallable, Category = "Health Methods")
    void Die();

    UFUNCTION()
    void OnRep_Health();
    UFUNCTION()
    void OnRep_IsDead();

public:
    UFUNCTION(BlueprintCallable, Category = "Health Methods")
    bool IsDead() const { return isDead; }
    
    UFUNCTION(BlueprintCallable, Category = "Health Methods")
    void TakeHealthDamage(int32 damageAmount);

    UFUNCTION(BlueprintCallable, Category = "Health Methods")
    int32 GetCurrentHealth() const { return currentHealth; }
    UFUNCTION(BlueprintCallable, Category = "Health Methods")
    int32 GetMaxHealth() const { return maxHealth; }
    UFUNCTION(BlueprintCallable, Category = "Health Methods")
    float GetHealthPercentage() const { return maxHealth > 0 ? (float)currentHealth / (float)maxHealth : 0.0f; }

// 5. controller/ input reaction
public:
    virtual void Move(FVector2D direction) override;
    virtual void Run() override;
    virtual void RunStop() override;
    virtual void CrouchStart() override;
    virtual void CrouchEnd() override;
    virtual void JumpStart() override;
    virtual void JumpEnd() override;
    virtual void Interact() override;

// 5.1 controller enable/disable
public:
    virtual bool CheckIfIsAbleToLook() override;
    virtual bool CheckIfIsAbleToMove() override;
    virtual bool CheckIfIsAbleToInteract() override;
    virtual bool CheckIfIsAbleToUseItems() override;
    virtual bool CheckIfIsAbleToRun() override;
    virtual bool CheckIfIsAbleToCrouch() override;
    virtual bool CheckIfIsAbleToJump() override;

// 5.2 movement related
// 5.3 jump related
// 5.4 air related
protected:
    // Air state management
    virtual void UpdateMovingControlsPerTick(float deltaTime) override;

// 5.5 interaction related
protected:
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    AMPCharacterCat* catHolding;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    AMPCharacterCat* catRubbing;

    public :
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StartHoldingCat(AMPCharacterCat* catToHold);
    // voluntary release
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StopHoldingCat();

    UFUNCTION(BlueprintCallable, Category = "Control Method")
    bool IsHoldingCat();

    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void PutCatInCage();

    // force release (struggle out)
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void ForceReleaseCat();

    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StartToBeRubbed(AMPCharacterCat* rubbedCat);
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StopToBeRubbed();

    // State getters
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    AMPCharacterCat* GetHeldCat() const;
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    bool IsBeingRubbed() const;
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    AMPCharacterCat* GetCatRubbing() const;
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    float GetHeldCatStrugglePercentage() const;

// 6. animation system
// 6.1 animation state
protected:
    // Replicated animation state struct
    UPROPERTY(ReplicatedUsing = OnRep_AnimState, BlueprintReadOnly, Category = "AnimState")
    FHumanAnimState animState;

    // Setters for animation state
    void SetPosture(EHumanPosture newPosture);
    virtual void SetMove(EMoveState newMove) override;
    virtual void SetAir(EAirState newAir) override;
    void SetContext(EHumanContext newContext, bool bMandatory = false);
    void SetInteraction(EHumanInteractionState newInteraction);
    
    UFUNCTION()
    void OnRep_AnimState();

    UFUNCTION(BlueprintPure, Category="AnimState")
    const FHumanAnimState& GetAnimState() const { return animState; }

// 6.2 animation context/ montage
protected:
    UFUNCTION(BlueprintCallable, Category = "Animation Methods")   
    void PlayHumanContextAnimMontage(EHumanContext context);
    virtual void OnMontageEndedContextClear(UAnimMontage* montage, bool bInterrupted) override;

// all montages
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* beStunned_Montage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float beStunnedMontagePlayRate = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    UAnimMontage* holdAnimMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    float holdAnimMontagePlayRate = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* stopHoldAnimMontage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float stopHoldAnimMontagePlayRate = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* beStruggleOut_Montage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float beStruggleOutMontagePlayRate = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* putCatInCage_Montage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float putCatInCageMontagePlayRate = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* dieEffect_Montage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float dieEffectMontagePlayRate = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    UAnimMontage* turnOnPower_Montage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Montages")
    float turnOnPowerMontagePlayRate = 1.0f;

// 6.3 warp hold animation
    // Hold Animation Warp Target Name
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    FName holdAnimWarpTargetName = "HoldAnimWarpTarget";
    // Hold Animation Chest Socket Name
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    FName holdAnimChestSocketName = "holdAnimChestSocket";
    // Hold Animation Chest Socket Offset
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    FVector holdAnimChestSocketOffset = FVector::ZeroVector;
    // BlueprintViewable hand target positions
    UPROPERTY(BlueprintReadOnly, Category = Animation)
    FVector holdAnimLeftHandTarget;
    UPROPERTY(BlueprintReadOnly, Category = Animation)
    FVector holdAnimRightHandTarget;
    // Exposed value for distance customization
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    float holdAnimWarpDistanceOffset = 0.f;

    // BlueprintCallable function to trigger snapping
    UFUNCTION(BlueprintCallable)
    void holdAnimSnapping();
    // Helper to update hand targets
    UFUNCTION(BlueprintCallable)
    void updateHoldAnimHandTargets();
    // Function to setup motion warping for hold
    void setupHoldAnimMotionWarping(AMPCharacterCat* catActor);
    UFUNCTION(BlueprintCallable)
    void stopHoldAnimCleanup();

// 7. special condition
public:
    virtual void BeStunned(int32 stunDuration) override;
    virtual void StopStunned() override;
};