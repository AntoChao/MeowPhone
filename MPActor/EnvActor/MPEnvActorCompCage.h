#pragma once

// [Meow-Phone Project]
//
// This class represents a cage, a specialized environmental actor used to capture cats.
// It inherits from the base `AMPEnvActorComp` and adds logic to check if it's occupied
// and to handle the "catching" of a cat.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint class inheriting from this one (e.g., `BP_CatCage`).
// 2. In the Blueprint, assign a static mesh for the cage's appearance. You might also want to have two meshes (or materials): one for the empty state and one for the occupied state.
// 3. Place instances of this Blueprint in your level where cats can be captured.
// 4. A Human character holding a cat will interact with this cage. The `IsInteractable` override checks if the interactor is a human holding a cat and if the cage is not already occupied.
// 5. When a human interacts with it, `ApplyInteractEffectDirect` is called, which in turn calls `CatchCat`, marking the cage as occupied and dealing with the captured cat.
//
// Necessary things to define:
// - A `StaticMesh` for the cage must be assigned in the `BP_CatCage` Blueprint.
// - The `OnRep_IsOccupied` function should be implemented in the Blueprint to handle the visual change when the cage becomes occupied (e.g., swapping a material, closing a door animation).
//
// How it interacts with other classes:
// - AMPEnvActorComp: Inherits all the base interaction and replication functionality.
// - AMPCharacterHuman: The human character is the primary interactor. This class checks if the human is holding a cat (`CanAcceptCat`).
// - AMPCharacterCat: The cat character is passed into the `CatchCat` function to be "captured". This likely involves destroying or disabling the cat actor and updating the game state.
// - Replication: The `isOccupiedReplicated` boolean is replicated. When its value changes on the server, the `OnRep_IsOccupied` function is automatically called on all clients, allowing for synchronized visual updates.

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