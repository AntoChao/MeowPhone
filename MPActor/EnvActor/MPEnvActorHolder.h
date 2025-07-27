#pragma once

// [Meow-Phone Project]
//
// This class serves as a placeholder or spawn point marker for dynamic environmental actors.
// It is intended to be placed in the level editor to designate a location and rotation where
// a more complex environmental actor (like a pushable vase or a cage) will be spawned
// at the beginning of the match.
//
// How to utilize in Blueprint:
// 1. You can create a Blueprint from this class if you want to add visual markers (e.g., a billboard component) that are only visible in the editor, making it easier to see where these spawn points are.
// 2. Place instances of this Actor (or its Blueprint) in the level to define spawn locations for environmental objects.
// 3. A manager class, likely `UManagerMatch` or a similar level-setup manager, will find all actors of this type at the start of the game (`GetAllActorsOfClass`). It will then iterate through them, get their transform (location and rotation), and use the `UFactoryEnvironment` to spawn a random or specific `AMPEnvActorComp` at that transform. After spawning the real actor, it will destroy this placeholder.
//
// Necessary things to define:
// - A `TSubclassOf<AMPEnvActorComp>` property could be added to a Blueprint of this class, allowing level designers to specify exactly what kind of object should spawn at this location.
//
// How it interacts with other classes:
// - AActor: The base class.
// - UManagerMatch / UFactoryEnvironment: This actor doesn't interact directly with anything. Instead, a manager finds it, reads its transform, and then uses a factory to spawn a different actor in its place. It is a passive data marker for other systems to use during level initialization.

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "MPEnvActorHolder.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMPEnvActorHolder : public AActor
{
    GENERATED_BODY()

public:
    AMPEnvActorHolder();
};