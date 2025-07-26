#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../MPInteractable.h"
#include "../MPPlaySoundInterface.h"
#include "UObject/ScriptInterface.h"

#include "MPCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class SoundCue;

class AMPItem;
enum class EMPItem : uint8;
enum class ETeam : uint8;

UCLASS(BlueprintType, Blueprintable)
class AMPCharacter : public ACharacter, public IMPInteractable, public IMPPlaySoundInterface
{
    GENERATED_BODY()
    
// 1. all actor class methods
public :
    AMPCharacter();
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;

// 2. interface
// 2.1 interactable
public :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FString interactHintTextKey;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interface Properties")
    FString uninteractableHintTextKey;

    virtual bool IsInteractable(AMPCharacter* player) override;
    virtual FText GetInteractHintText(AMPCharacter* player) override;
    virtual void BeInteracted(AMPCharacter* player) override;

// 2.2 play sound
public:
    virtual void PlaySoundLocally(USoundCue* aSound) override;
    virtual void PlaySoundBroadcast(USoundCue* aSound) override;

protected:
    UFUNCTION(Server, Reliable)
        void PlaySoundServer(USoundCue* aSound);
            
    UFUNCTION(NetMulticast, Reliable)
        void PlaySoundMulticast(USoundCue* aSound);

// 3. components
// 3.1 camera component
	/* camera 
    * is different between human and cat
	* human -> first person, cat -> third person
    */
protected :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Properties")
        USpringArmComponent* cameraSpringArm;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Properties")
        UCameraComponent* characterCamera;

    // Preview camera for lobby preview
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Properties")
        UCameraComponent* PreviewCamera;

    // detect 
    FComponentQueryParams DefaultComponentQueryParams;
	FCollisionResponseParams DefaultResponseParam;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detect Properties")
        bool isAbleToFireTraceLine = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detect Properties")
        float detectDistance = 300.0f; // Added default value
    
    UPROPERTY(BlueprintReadWrite, Category = "Detect Properties")
        FVector detectStart;
    UPROPERTY(BlueprintReadWrite, Category = "Detect Properties")
        FVector detectDirection;
    UPROPERTY(BlueprintReadWrite, Category = "Detect Properties")
        FVector detectEnd;
    UPROPERTY(BlueprintReadWrite, Category = "Detect Properties")
        FHitResult detectHit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detect Properties")
        AActor* detectedActor = nullptr; // Added null initialization
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detect Properties")
        TScriptInterface<IMPInteractable> detectInteractableActor;

    UPROPERTY(BlueprintReadOnly, Category = "Common Properties")
        FText detectInvalidHintText = FText::FromString(TEXT("")); // detect hint text for object that are not even mpactors
    UPROPERTY(BlueprintReadOnly, Category = "Common Properties")
        FText curDetectHintText; // detect hint text (for objects that are not interactable but are mpactors)

    UFUNCTION(BlueprintCallable, Category = "Detect Method")
        void Detect(); // calculate and fire the hit
    UFUNCTION(BlueprintCallable, Category = "Detect Method")
        void DetectReaction(); // determinate the current interaction

    UFUNCTION()
        void OnRep_AbleToInteract();

// 3.2 motion warping component
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
        UMotionWarpingComponent* motionWarpingComponent;
    
// 4. common properties
protected :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Common Properties")
        FText characterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Common Properties")
        ETeam characterTeam = ETeam::EHuman;

    UFUNCTION(BlueprintCallable, Category = "Getter Method")
        ETeam GetCharacterTeam() const { return characterTeam; }

// 5. controller/ input reaction
protected :
    // possession
    virtual void PossessedBy(AController* newController) override;

    // input reaction
    void Look(FVector2D direction);
    virtual void Move(FVector2D direction);
    virtual void MoveStop();
    virtual void Run();
    virtual void RunStop();
    virtual void CrouchStart();
    virtual void CrouchEnd();
    virtual void JumpStart();
    virtual void JumpEnd();
    virtual void Interact();

    void SelectItem(int32 itemIndex);
    void UnselectCurItem();
    void UseCurItem();
    bool IsAbleToUseCurItem();
    void DropCurItem();

// 5.1 controller enable/disable
    virtual bool CheckIfIsAbleToLook();
    virtual bool CheckIfIsAbleToMove();
    virtual bool CheckIfIsAbleToRun();
    virtual bool CheckIfIsAbleToCrouch();
    virtual bool CheckIfIsAbleToJump();
    virtual bool CheckIfIsAbleToDoubleJump();
    virtual bool CheckIfIsAbleToClimb();
    virtual bool CheckIfIsAbleToInteract();
    virtual bool CheckIfIsAbleToUseItems();

// 5.2 movement related
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    virtual void UpdateMovingControlsPerTick(float deltaTime);

    UPROPERTY(ReplicatedUsing = OnRep_CurSpeed, BlueprintReadWrite, Category = "Control Properties")
        int32 curSpeed = 0; // Added default value
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
        int32 moveSpeed = 600; // Added default value
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
        int32 runSpeed = 1200; // Added default value
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
        int32 crouchSpeed = 300; // Added default value
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
        int32 extraSpeed = 0; // Added default value

    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void UpdateSpeed();
    
    UFUNCTION()
        void OnRep_CurSpeed();
        
// 5.3 inventory related
protected :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Properties")
        TArray<EMPItem> initItems;
    UPROPERTY(Replicated, BlueprintReadWrite, Category = "Inventory Properties")
        TArray<AMPItem*> inventory;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Properties")
        int32 inventoryCapacity = 10; // Added default value
    
    UPROPERTY(ReplicatedUsing = OnRep_CurHoldingItemIndex, BlueprintReadWrite, Category = "Inventory Properties")
        int32 curHoldingItemIndex = -1;
    UPROPERTY(ReplicatedUsing = OnRep_CurHoldingItem, BlueprintReadWrite, Category = "Inventory Properties")
        AMPItem* curHoldingItem = nullptr; // Added null initialization

    UFUNCTION(BlueprintCallable, Category = "Inventory Method")
        void InitializeItems();
    UFUNCTION(BlueprintCallable, Category = "Inventory Method")
        void AddAnItem(EItem aItemTag);
    UFUNCTION(BlueprintCallable, Category = "Inventory Method")
        bool IsAbleToAddItem();
    UFUNCTION(BlueprintCallable, Category = "Inventory Method")
        bool IsInventoryFull();
    
    UFUNCTION()
    void OnRep_CurHoldingItemIndex();
    
    UFUNCTION()
    void OnRep_CurHoldingItem();
    
public :
    UFUNCTION(BlueprintCallable, Category = "Inventory Method")
        void PickupAnItem(AMPItem* aItem);
    UFUNCTION(BlueprintCallable, Category = "Inventory Method")
        void DeleteItem(AMPItem* itemToDelete);
        
// 6.Animation system
// 6.1 animation state
protected:
    UPROPERTY(ReplicatedUsing = OnRep_IsDoingAnimation, BlueprintReadWrite, Category = "Animation Properties")
        bool isDoingAnAnimation = false;
    UFUNCTION()
        void OnRep_IsDoingAnimation();

// 6.2 animation context/ montage
public:
    UFUNCTION(BlueprintCallable, Category = "Setter Method")
        void SetMovementMode(EMovementMode newMode);
    UFUNCTION(BlueprintCallable, Category = "Setter Method")
        void SetAirState(EAirState newAirState);
        
    UFUNCTION(BlueprintCallable, Category = "Animation Methods")
    virtual void PlayContextAnimationMontage() = 0; // Pure virtual - must be implemented by children
    UFUNCTION(BlueprintCallable, Category = "Animation Methods")
    virtual void PlaySelectedMontage(UAnimMontage* chosenMontage, float playRate = 1.0f);
    UFUNCTION(BlueprintCallable, Category = "Animation Methods")
    virtual void OnMontageEnded(UAnimMontage* montage, bool bInterrupted);
    UFUNCTION(BlueprintCallable, Category = "Animation Methods")
    virtual void OnMontageEndedContextClear(UAnimMontage* montage, bool bInterrupted);

    UFUNCTION(BlueprintCallable, Category = "Animation Methods")
    bool IsPlayerInAnimation() const { return isDoingAnAnimation; }    
    UFUNCTION(BlueprintCallable, Category = "Animation Methods")
    void SetIsDoingAnAnimation(bool bDoingAnimation) { isDoingAnAnimation = bDoingAnimation; }

// 7. special condition
public:
    // Stun system
    UPROPERTY(ReplicatedUsing = OnRep_IsStunned, BlueprintReadWrite, Category = "Stun Properties")
    bool bIsStunned = false;
    
    FTimerHandle stunTimerHandle; // Timer for clearing stunned state

    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual void BeStunned(int32 stunDuration);
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual void StopStunned();

    UFUNCTION()
    void OnRep_IsStunned();

};