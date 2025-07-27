#pragma once

// [Meow-Phone Project]
//
// This is a core gameplay interface that defines the "contract" for any object that can be
// interacted with by a character. By having classes implement this interface, the character's
// interaction system can work with any type of object without needing to know its specific class.
//
// How to utilize in Blueprint:
// - This interface is marked as `CannotImplementInterfaceInBlueprint`. This means all the logic must be implemented in C++.
// - C++ classes like `AMPCharacter`, `AMPItem`, and `AMPEnvActorComp` inherit from `IMPInteractable` and provide their own implementations for the functions.
// - The character's interaction trace checks if a detected actor implements this interface. If it does, the character can then call these functions on it.
//
// Necessary things to define:
// - Any C++ class that should be interactable MUST publicly inherit from `IMPInteractable` and provide concrete implementations for all three virtual functions.
//
// How it interacts with other classes:
// - UInterface: The base class for Unreal Engine interfaces.
// - AMPCharacter: The character class is the "client" of this interface. It detects actors, checks if they implement `IMPInteractable`, and then calls `IsInteractable`, `GetInteractHintText`, and `BeInteracted` to drive the interaction.
// - AMPItem, AMPEnvActorComp, etc.: These are "servers" or "providers" of the interface. They implement the functions to define how they specifically respond to being interacted with. For example, an item's `BeInteracted` will cause it to be picked up, while a switch's `BeInteracted` might toggle a light.

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MPInteractable.generated.h"

struct FLocalizedText;
class AMPCharacter;

UINTERFACE(MinimalAPI, Meta = (CannotImplementInterfaceInBlueprint))
class UMPInteractable : public UInterface
{
    GENERATED_BODY()
};

class IMPInteractable
{
    GENERATED_BODY()

public:
    virtual bool IsInteractable(AMPCharacter* player) = 0;

    virtual FText GetInteractHintText(AMPCharacter* player) = 0;

    virtual void BeInteracted(AMPCharacter* player) = 0;
};