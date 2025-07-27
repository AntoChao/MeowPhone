#pragma once

// [Meow-Phone Project]
//
// This factory is responsible for spawning various environmental actors. It provides a centralized
// location for the logic that creates dynamic parts of the game world.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint class inheriting from `UFactoryEnvironment`.
// 2. In your game logic (e.g., a level manager or game mode), create an instance of your `UFactoryEnvironment` Blueprint.
// 3. Before use, ensure the `gameWorld` is set by calling the `SetGameWorld` function (inherited from `UMPFactory`).
// 4. Call `SpawnMPActor` with a specific `actorCode`, `location`, and `rotation` to spawn the desired environmental actor.
//
// Necessary things to define:
// - The C++ implementation of `SpawnMPActor` in `FactoryEnvironment.cpp` must be populated with a switch statement or similar logic. This logic will map integer `actorCode` values to specific `TSubclassOf<AActor>` classes for the environment objects you want to spawn (e.g., breakable vases, interactive doors, etc.).
//
// How it interacts with other classes:
// - UMPFactory: Inherits the base actor spawning framework.
// - Environmental Actors (e.g., `AMPEnvActorHolder`): This factory's main job is to create instances of various environmental actors. The specific classes are determined by the logic within its `SpawnMPActor` function.
// - Manager Classes (e.g., `UManagerMatch` or a potential `UManagerEnvironment`): These higher-level managers would own and use this factory to populate the world with dynamic objects at the start of a match or in response to game events.

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "FactoryEnvironment.generated.h"

UCLASS(Blueprintable)
class UFactoryEnvironment : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UFactoryEnvironment();

public :
	virtual AActor* SpawnMPActor(int actorCode, FVector actorLoEnvironmention, FRotator actorRotation) override;
	
};
