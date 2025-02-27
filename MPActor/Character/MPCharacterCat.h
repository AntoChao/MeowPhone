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
};
