#pragma once

// [Meow-Phone Project]
//
// This is a C++ only interface that provides a standardized way for any object to play sounds.
// It defines two distinct methods: one for sounds that should only play for the local player,
// and one for sounds that should be broadcast to all players.
//
// How to utilize in Blueprint:
// - This interface cannot be implemented in Blueprints. C++ classes must inherit from `IMPPlaySoundInterface` to use it.
//
// Necessary things to define:
// - Any C++ class that needs to play sounds using this system must implement both `PlaySoundLocally` and `PlaySoundBroadcast`.
// - `PlaySoundLocally`: The implementation should just play the sound at the actor's location. This is for UI feedback or sounds that are only relevant to the person causing them.
// - `PlaySoundBroadcast`: The implementation should typically be a Server RPC that then calls a NetMulticast RPC to play the sound on the server and all clients. This is for important gameplay sounds that everyone needs to hear (e.g., an item being used, an object breaking).
//
// How it interacts with other classes:
// - UInterface: The base class.
// - AMPCharacter, AMPItem, etc.: These classes implement the interface. For example, when an item is used, its `BeUsed` function might call `PlaySoundBroadcast` on itself to notify all players.
// - USoundCue: The sound assets to be played are passed as `USoundCue` pointers.

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MPPlaySoundInterface.generated.h"

class USoundCue;

UINTERFACE(MinimalAPI, Meta = (CannotImplementInterfaceInBlueprint))
class UMPPlaySoundInterface : public UInterface
{
    GENERATED_BODY()
};

class IMPPlaySoundInterface
{
    GENERATED_BODY()

public:
    virtual void PlaySoundLocally(USoundCue* aSound) = 0;

    virtual void PlaySoundBroadcast(USoundCue* aSound) = 0;
};
