#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../MPInteractable.h"
#include "UObject/ScriptInterface.h"

#include "MPCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

enum class EMovementMode : uint8;
enum class EMovementLocomotion : uint8;
enum class EMovementAirStatus : uint8;

enum class EMPItem : uint8;
class AMPItem;
enum class ETeam : uint8;

UCLASS(BlueprintType, Blueprintable)
class AMPCharacter : public ACharacter, public IMPInteractable
{
    GENERATED_BODY()

public :
    AMPCharacter();

// common class methods
public :
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void BeginPlay() override;

    virtual void Tick(float deltaTime) override;

// interactable interface
public :
    virtual bool IsInteractable(AMPCharacter* player) override;

    virtual FText GetInteractHintText(AMPCharacter* player) override;

    virtual void BeInteracted(AMPCharacter* player) override;

// camera component
protected :
	/* camera 
    * is different between human and cat
	* human -> first person, cat -> third person
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Properties")
        USpringArmComponent* cameraSpringArm;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Properties")
        UCameraComponent* characterCamera;

// detect 
    FComponentQueryParams DefaultComponentQueryParams;
	FCollisionResponseParams DefaultResponseParam;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detect Properties")
        bool isAbleToFireTraceLine = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detect Properties")
        float detectDistance;
    
    UPROPERTY(BlueprintReadWrite, Category = "Detect Properties")
        FVector detectStart;
    UPROPERTY(BlueprintReadWrite, Category = "Detect Properties")
        FVector detectDirection;
    UPROPERTY(BlueprintReadWrite, Category = "Detect Properties")
        FVector detectEnd;
    UPROPERTY(BlueprintReadWrite, Category = "Detect Properties")
        FHitResult detectHit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detect Properties")
        AActor* detectedActor; // should never be able to interact, only used for item/ ability
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detect Properties")
        TScriptInterface<IMPInteractable> detectInteractableActor;

    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        bool ableToInteractCurrently; // hide or not the interact UI
    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        FText curDetectHintText;
    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        FText invalidDetectHintText;

    UFUNCTION(BlueprintCallable, Category = "Detect Method")
        void Detect(); // calculate and fire the hit
    UFUNCTION(BlueprintCallable, Category = "Detect Method")
        void DetectReaction(); // determinate the current interaction

// common properties
protected :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Common Properties")
    FText characterName;
    
// inventory
protected :
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Properties")
        TArray<EItem> initItems;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Properties")
        TArray<AMPItem*> inventory;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Properties")
        int inventoryCapacity;
    
    UPROPERTY(BlueprintReadWrite, Category = "Inventory Properties")
        int curHoldingItemIndex = -1;
    UPROPERTY(BlueprintReadWrite, Category = "Inventory Properties")
        AMPItem* curHoldingItem;

    UFUNCTION(BlueprintCallable, Category = "Inventory Method")
        void InitializeItems();
    UFUNCTION(BlueprintCallable, Category = "Inventory Method")
        void AddAnItem(EItem aItemTag);
    UFUNCTION(BlueprintCallable, Category = "Inventory Method")
        bool IsAbleToAddItem();
    UFUNCTION(BlueprintCallable, Category = "Inventory Method")
        bool IsInventoryFull();
    
public :
    UFUNCTION(BlueprintCallable, Category = "Inventory Method")
        void PickupAnItem(AMPItem* aItem);

    UFUNCTION(BlueprintCallable, Category = "Inventory Method")
        void DeleteItem(AMPItem* itemToDelete);

// controller/ input reaction
protected :
    // possession
    virtual void PossessedBy(AController* newController) override;

    // movement/ animation control
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    virtual void UpdateMovingControls();

    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
        EMovementMode curMovementMode = EMovementMode::EStanding;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
        EMovementLocomotion curLocomotionState = EMovementLocomotion::EIdle;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
        EMovementAirStatus curAirState = EMovementAirStatus::EGround;


    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
        int curSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
        int moveSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
        int runSpeed;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
        int crouchSpeed;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
        int extraSpeed;

    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void UpdateSpeed();


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
        float wallDetectionHeightPercentage = 0.8f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
        float wallDetectionDownward = -10.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
        float wallDetectionRadius = 15.0f;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control Properties")
        float wallDetectionAngleTolerance = 0.3f;

    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual bool CheckIfIsAbleToLook();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual bool CheckIfIsAbleToMove();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual bool CheckIfIsAbleToRun();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual bool CheckIfIsAbleToCrouch();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual bool CheckIfIsAbleToJump();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual bool CheckIfIsAbleToDoubleJump();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual bool CheckIfIsAbleToClimb();

    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual bool CheckIfIsAbleToInteract();

public :
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void Look(FVector2D direction);
    
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual void Move(FVector2D direction);
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual void MoveStop();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void Run();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void RunStop();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void CrouchStart();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void CrouchEnd();
        
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual oid JumpStart();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual void JumpEnd();


    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual void Interact();

    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void SelectItem(int itemIndex);
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void UnselectCurItem();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void UseCurItem();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        bool IsAbleToUseCurItem();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void DropCurItem();

// general setter and getter
public :
    UFUNCTION(BlueprintCallable, Category = "Getter Method")
        ETeam GetCharacterTeam();
    
    UFUNCTION(BlueprintCallable, Category = "Setter Method")
        void SetMovementMode(EMovementMode NewMode);
    UFUNCTION(BlueprintCallable, Category = "Setter Method")
        void SetLocomotionState(EMovementLocomotion NewLocomotion);
    UFUNCTION(BlueprintCallable, Category = "Setter Method")
        void SetAirState(EMovementAirStatus NewAirState);
        
};