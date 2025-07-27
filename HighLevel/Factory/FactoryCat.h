#pragma once

// [Meow-Phone Project]
//
// This factory class is dedicated to spawning Cat characters (`AMPCharacterCat`). It is used to
// create different variations of cats, such as a standard cat or a "died" version.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint that inherits from `UFactoryCat`.
// 2. In this Blueprint, you MUST assign the `Cat EXP Class` and `Cat Died Class` properties.
//    These should be your `AMPCharacterCat` Blueprints (e.g., `BP_Cat_Normal`, `BP_Cat_DiedEffect`).
// 3. In your game logic (e.g., Game Mode), create an instance of your `UFactoryCat` Blueprint.
// 4. Before spawning, call `SetGameWorld` (inherited from `UMPFactory`).
// 5. Call `SpawnMPActor` with the relevant `actorCode` to spawn the desired cat type at a specific location and rotation.
//
// Necessary things to define:
// - `catEXPClass`: This must be set in the Blueprint to the character class that represents a standard, playable cat.
// - `catDiedClass`: This must be set in the Blueprint to a class that represents a cat after it has "died" - this might be a ragdoll or a version with a special effect.
// - The `SpawnMPActor` implementation in `FactoryCat.cpp` needs to correctly map the `actorCode` to these defined classes.
//
// How it interacts with other classes:
// - UMPFactory: Inherits the core spawning capabilities.
// - AMPCharacterCat: This factory's primary purpose is to create instances of this class and its children.
// - Game Mode (e.g., `AMPGMGameplay`): The game mode will use this factory to spawn cats at the start of a match or during gameplay.
// - Player Controller / AI Controller: Once a cat is spawned, it will be possessed by either a player or an AI controller.

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "FactoryCat.generated.h"

class AMPCharacterCat;

UCLASS(Blueprintable)
class UFactoryCat : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UFactoryCat();

public :
	virtual AActor* SpawnMPActor(int actorCode, FVector actorLocation, FRotator actorRotation) override;
	
// all tsubclassof
protected :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Factory Stats")
		TSubclassOf<AMPCharacterCat> catEXPClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Factory Stats")
		TSubclassOf<AMPCharacterCat> catDiedClass;
};
