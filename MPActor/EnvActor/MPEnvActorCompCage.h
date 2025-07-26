#pragma once

#include "CoreMinimal.h"
#include "MPEnvActorComp.h"

#include "MPEnvActorCompCage.generated.h"

class AMPCharacterCat;
class AMPCharacterHuman;

UCLASS(BlueprintType, Blueprintable)
class AMPEnvActorCompCage : public AMPEnvActorComp
{
    GENERATED_BODY()

public:
    AMPEnvActorCompCage();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Common methods
protected:
    virtual void BeginPlay() override;

    // interactable interface
public:
    virtual bool IsInteractable(AMPCharacter* targetActor) override;
    virtual FText GetInteractHintText(AMPCharacter* targetActor) override;

    // interact specific
protected:
    virtual void ApplyInteractEffectDirect(AMPCharacter* targetActor);

    // Cage specific properties
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cage Properties")
    FText cageName = FText::FromString(TEXT("Cat Cage"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cage Properties")
    bool isOccupied = false;

    UPROPERTY(ReplicatedUsing = OnRep_IsOccupied, BlueprintReadWrite, Category = "Cage Properties")
    bool isOccupiedReplicated = false;

    UFUNCTION()
    void OnRep_IsOccupied();

public:
    UFUNCTION(BlueprintCallable, Category = "Cage Methods")
    bool CanAcceptCat(AMPCharacter* humanActor);

    UFUNCTION(BlueprintCallable, Category = "Cage Methods")
    void CatchCat(AMPCharacter* humanActor, AMPCharacterCat* catToCatch);

    UFUNCTION(BlueprintCallable, Category = "Cage Methods")
    bool IsCageOccupied() const { return isOccupied; }
}; 