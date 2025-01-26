#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../MPInteractable.h"

#include "MPCharacter.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMPCharacter : public ACharacter, public IMPInteractable
{
    GENERATED_BODY()

public :
    AMPCHaracter();

// common class methods
public :
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void BeginPlay() override;

    virtual void Tick(float deltaTime) override;

// camera component
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Camera Properties")
        UCameraComponent* characterCamera;
    
// detect 
    UPROPERTY(BlueprintReadWrite, Category = "Detect Properties")
        float detectDistance;
    UPROPERTY(BlueprintReadWrite, Category = "Detect Properties")
        FVector detectDirection;
    UPROPERTY(BlueprintReadWrite, Category = "Detect Properties")
        FHitResult detectHit;
    UPROPERTY(BlueprintReadWrite, Category = "Detect Properties")
        AActor* detectActor;
    UPROPERTY(BlueprintReadWrite, Category = "Detect Properties")
        TScripInterface<IMPInteractable> detectInteractableActor;

    UFUNCTION(BlueprintCallable, Category = "Detect Method")
        void Detect(); // calculate and fire the hit
    UFUNCTION(BlueprintCallable, Category = "Detect Method")
        void DetectReaction(); // determinate the current interaction

// common properties
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Character Common Properties")
        FLocalizedText characterName;
    
// inventory
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Inventory Properties")
        TArray<EMPItem> initItems;
    UPROPERTY(BlueprintReadWrite, Category = "Inventory Properties")
        TArray<AMPItem> inventory;
    UPROPERTY(BlueprintReadWrite, Category = "Inventory Properties")
        int inventoryCapacity;
    
    UPROPERTY(BlueprintReadWrite, Category = "Inventory Properties")
        curHoldingItem* AMPItem;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        void InitializeItems();
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        bool IsInventoryFull();
    
// controller direct relation
protected :
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
        int curSpeed;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
        int moveSpeed;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
        int runSpeed;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
        int extraSpeed;

    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual void PossessedBy(AController* newController) override;

public :
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void Look(FVector direction);
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void Move(FVector direction);

    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void Run();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void RunStop();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void Jump();
    
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual void Interact();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void InteractEnvActor();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void InteractPickUpItem();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual void InteractHuman();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        virtual void InteractCat();

    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void SelectItem(int itemIndex);
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void UseCurItem();
    UFUNCTION(BlueprintCallable, Category = "Control Method")
        void DropCurItem();

}