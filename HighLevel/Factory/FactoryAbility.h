#pragma once

// [Meow-Phone Project]
//
// This class is a specialized factory for creating ability actors in the game. It extends
// the base `UMPFactory` to handle the specific logic required for spawning abilities.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint class that inherits from `UFactoryAbility`.
// 2. In your game logic (e.g., within a character or ability manager Blueprint), create an instance of your `UFactoryAbility` Blueprint.
// 3. Before spawning, ensure the `gameWorld` is set by calling the `SetGameWorld` function, which is inherited from `UMPFactory`.
// 4. Call the `SpawnMPActor` function with the appropriate `actorCode` for the ability you wish to create. This function will return the spawned ability actor.
//
// Necessary things to define:
// - The implementation of `SpawnMPActor` in the corresponding C++ source file (`FactoryAbility.cpp`) must contain the logic to handle different `actorCode` values. Each code should map to a specific ability class to be spawned.
//
// How it interacts with other classes:
// - UMPFactory: It inherits from `UMPFactory` to get the basic actor spawning framework.
// - AMPAbility: This factory is responsible for creating instances of `AMPAbility` or its child classes. The `SpawnMPActor` function will be configured to spawn different types of abilities based on the provided code.
// - Character Classes (e.g., AMPCharacterCat): Characters will likely use this factory to spawn their abilities during gameplay. For instance, when a player presses an ability key, the character would use the ability factory to create the corresponding ability actor in the world.

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "FactoryAbility.generated.h"

UCLASS(Blueprintable)
class UFactoryAbility : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UFactoryAbility();

public :
	virtual AActor* SpawnMPActor(int actorCode, FVector actorLocation, FRotator actorRotation) override;
	
};
