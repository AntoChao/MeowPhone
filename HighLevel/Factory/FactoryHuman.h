#pragma once

// [Meow-Phone Project]
//
// This factory is specialized in spawning Human characters (`AMPCharacterHuman`). It centralizes
// the creation logic for different types of human characters in the game.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint class that inherits from `UFactoryHuman`.
// 2. In this Blueprint, you can add properties to define the specific `AMPCharacterHuman` subclasses to be spawned (e.g., different human player models). These would be added as `TSubclassOf<AMPCharacterHuman>` UPROPERTIES.
// 3. In your Game Mode or other manager classes, create an instance of your `UFactoryHuman` Blueprint.
// 4. Before spawning, call the `SetGameWorld` function (inherited from `UMPFactory`).
// 5. Use the `SpawnMPActor` function with an `actorCode` to create a specific human character at the desired location and rotation.
//
// Necessary things to define:
// - The C++ implementation of `SpawnMPActor` in `FactoryHuman.cpp` must be filled out with logic (like a switch statement) that maps `actorCode` values to the `TSubclassOf<AMPCharacterHuman>` properties you define in the Blueprint.
// - In the `UFactoryHuman` Blueprint, you must assign the actual character Blueprints to the `TSubclassOf` properties you've exposed.
//
// How it interacts with other classes:
// - UMPFactory: It derives from the base factory to get common spawning functionality.
// - AMPCharacterHuman: The primary purpose of this factory is to create instances of `AMPCharacterHuman` and its child classes.
// - Game Mode (e.g., `AMPGMGameplay`): The game mode will own and operate this factory to spawn human players or AI at the beginning of a match.
// - Player Controller / AI Controller: Once a human character is spawned, it will be possessed by an appropriate controller.

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "FactoryHuman.generated.h"

UCLASS(Blueprintable)
class UFactoryHuman : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UFactoryHuman();

public :
	virtual AActor* SpawnMPActor(int actorCode, FVector actorLocation, FRotator actorRotation) override;
	
};
