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

// ability
protected:    
    UPROPERTY(BlueprintReadWrite, Category = "Ability Properties")
    UMPAbility activeAbility;
    
    UPROPERTY(BlueprintReadWrite, Category = "Ability Properties")
    TArray<EAbility> initPassiveAbilities;

    UPROPERTY(BlueprintReadWrite, Category = "Ability Properties")
    TArray<UMPAbility> allPassiveAbilities;


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
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
    FLocalizedText humanInteractHintText;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
    FLocalizedText catInteractHintText;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
    FLocalizedText uninteractableHumanHintText;
    UPROPERTY(BlueprintReadWrite, Category = "Interface Properties")
    FLocalizedText uninteractableCatHintText;

public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual bool IsInteractable(AMPCharacter* player) override;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual FLocalizedText GetInteractHintText(AMPCharacter* player) override;

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
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    float holdTotalTime;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    float curHoldTime;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    float curHoldPercentage;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    float holdModifier;

    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    bool isRubbing = false;
    UPROPERTY(BlueprintReadWrite, Category = "Control Properties")
    AMPCharacterHuman* humanRubbing;

protected :
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    virtual bool CheckIfIsAbleToLook() override;
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    virtual bool CheckIfIsAbleToMove() override;
    UFUNCTION(BlueprintCallable, Category = "Control Method")
    virtual bool CheckIfIsAbleToInteract() override;

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
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
    virtual void Interact() override;
}
