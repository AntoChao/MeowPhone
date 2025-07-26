#pragma once

#include "CoreMinimal.h"
#include "MPCharacter.h"

#include "MPCharacterHuman.generated.h"

enum class EHumanAction : uint8;
class AMPCharacterCat;

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
    void TakeHealthDamage(int32 damageAmount);
    UFUNCTION(BlueprintCallable, Category = "Health Methods")
    void HealHealth(int32 healAmount);
    UFUNCTION(BlueprintCallable, Category = "Health Methods")
    void Die();
    UFUNCTION(BlueprintCallable, Category = "Health Methods")
    bool IsDead() const { return isDead; }
    
    UFUNCTION(BlueprintCallable, Category = "Health Methods")
    int32 GetCurrentHealth() const { return currentHealth; }
    UFUNCTION(BlueprintCallable, Category = "Health Methods")
    int32 GetMaxHealth() const { return maxHealth; }
    UFUNCTION(BlueprintCallable, Category = "Health Methods")
    float GetHealthPercentage() const { return maxHealth > 0 ? (float)currentHealth / (float)maxHealth : 0.0f; }

    UFUNCTION()
        void OnRep_Health();
    UFUNCTION()
        void OnRep_IsDead();

// 5. controller/ input reaction
protected:
    virtual void Move(FVector2D direction) override;
    virtual void Run() override;
    virtual void RunStop() override;
    virtual void CrouchStart() override;
    virtual void CrouchEnd() override;
    virtual void JumpStart() override;
    virtual void JumpEnd() override;
    virtual void Interact() override;

// 5.1 controller enable/disable
protected:
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
    bool IsHoldingCat() const { return animState.curInteraction == EHumanInteractionState::HoldingCat && IsValid(catHolding); }
    
    // force release (struggle out)
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void ForceReleaseCat();

    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StartToBeRubbed(AMPCharacterCat* rubbedCat);
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    void StopToBeRubbed();

    // State getters
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    AMPCharacterCat* GetHeldCat() const { return IsValid(catHolding) ? catHolding : nullptr; }
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    bool IsBeingRubbed() const { return animState.curInteraction == EHumanInteractionState::BeingRubbed; }
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    AMPCharacterCat* GetCatRubbing() const { return IsValid(catRubbing) ? catRubbing : nullptr; }
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    float GetHeldCatStrugglePercentage() const { return IsValid(catHolding) ? catHolding->GetStruggleBarPercentage() : 0.0f; }

// 6. animation system
// 6.1 animation state
protected:
    // Replicated animation state struct
    UPROPERTY(ReplicatedUsing = OnRep_AnimState, BlueprintReadOnly, Category = "AnimState")
    FHumanAnimState animState;
    // Setters for animation state
    void SetPosture(EHumanPosture newPosture);
    void SetMove(EMoveState newMove);
    void SetAir(EAirState newAir);
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
    UFUNCTION(BlueprintCallable, Category = "Animation Methods")
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
    // Add play rate for hold animation montage
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
    float holdAnimMontagePlayRate = 1.0f;

    // BlueprintCallable function to trigger snapping
    UFUNCTION(BlueprintCallable)
    void holdAnimSnapping();
    // Helper to update hand targets
    UFUNCTION(BlueprintCallable)
    void updateHoldAnimHandTargets();
    // Function to setup motion warping for hold
    void setupHoldAnimMotionWarping(class AMPCharacterCat* catActor);
    UFUNCTION(BlueprintCallable)
    void stopHoldAnimCleanup();

// 7. special condition
public:
    virtual void BeStunned(int32 stunDuration) override;
    virtual void StopStunned() override;
};