#pragma once

// [Meow-Phone Project]
//
// This class is a highly versatile environmental actor that delegates its core interaction
// logic to Blueprint. It acts as a template for creating custom interactive objects where the
// specific effect of the interaction is defined entirely in the Blueprint graph.
//
// How to utilize in Blueprint:
// 1. Create a new Blueprint class inheriting from this one (e.g., `BP_CustomButton`, `BP_DistractionDevice`).
// 2. In this Blueprint, you implement one or both of the "Custom Interact Function" events:
//    - **Event Custom Interact Effect Direct**: If your object has an instant effect (like opening a door or toggling a light), implement the logic for that effect here.
//    - **Event Custom Interact Effect Duration**: If your object has an effect that happens over time (like channeling a spell or repairing an engine), implement the logic for each "tick" of the interaction here.
// 3. Configure the base interaction properties in the Blueprint's defaults (e.g., `isSingleUse`, `totalInteractDuration`, `totalCooldown`) just like with a regular `AMPEnvActorComp`.
// 4. When a character interacts with this actor, the C++ code calls `ApplyInteractEffectDirect` or starts the duration timer. These functions, in turn, fire the corresponding `BlueprintImplementableEvent` in your graph.
//
// Necessary things to define:
// - You MUST implement at least one of the `CustomInteractEffect...` events in the child Blueprint for the object to have any function.
//
// How it interacts with other classes:
// - AMPEnvActorComp: Inherits all the standard interaction machinery (timers, cooldowns, replication).
// - BlueprintImplementableEvent: This is the core mechanism. The C++ class defines the function signatures, but the implementation is left entirely to the Blueprint. This provides a clean bridge from the generic C++ interaction system to specific, custom Blueprint logic.

#include "CoreMinimal.h"
#include "MPEnvActorComp.h"

#include "MPEnvActorCompCustom.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMPEnvActorCompCustom : public AMPEnvActorComp
{
    GENERATED_BODY()

public:
    AMPEnvActorCompCustom();

    // interactable interface
public:
    virtual bool IsInteractable(AMPCharacter* targetActor) override;

    virtual FText GetInteractHintText(AMPCharacter* targetActor) override;

    // interact specific
protected:
    virtual void ApplyInteractEffectDirect(AMPCharacter* targetActor) override;

    virtual void ApplyInteractEffectDurationEffect() override;

    // implement in blueprint cuz the variety
    UFUNCTION(BlueprintImplementableEvent, Category = "Custom Interact Function")
    void CustomInteractEffectDirect();

    UFUNCTION(BlueprintImplementableEvent, Category = "Custom Interact Function")
    void CustomInteractEffectDuration();

};