#pragma once

// [Meow-Phone Project]
//
// This factory class is responsible for spawning all in-game items (`AMPItem`). It serves as a
// single point for creating items, which helps in managing item generation logic.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint class that inherits from `UFactoryItem`.
// 2. In this Blueprint, you should add `TSubclassOf<AMPItem>` UPROPERTIES for each type of item you want this factory to be able to spawn. For example, `UPROPERTY() TSubclassOf<AMPItem> HealthPotionClass;`.
// 3. In your Game Mode, a manager, or even from an item spawner actor, create an instance of your `UFactoryItem` Blueprint.
// 4. In the Blueprint editor for `UFactoryItem`, assign your specific item Blueprints (e.g., `BP_HealthPotion`) to the properties you created.
// 5. Before spawning, call `SetGameWorld` (inherited from `UMPFactory`).
// 6. Call `SpawnMPActor` with the appropriate `actorCode` to create an item at a specific location and rotation.
//
// Necessary things to define:
// - You must define `TSubclassOf<AMPItem>` properties in the header for the different items.
// - You must assign these properties in the `UFactoryItem` Blueprint.
// - The `SpawnMPActor` function in `FactoryItem.cpp` must contain the logic (e.g., a switch statement) to map an `actorCode` to the correct item class property for spawning.
//
// How it interacts with other classes:
// - UMPFactory: Inherits base spawning functionality.
// - AMPItem: This factory's sole purpose is to create instances of `AMPItem` child classes.
// - Managers/Spawners: Higher-level classes will use this factory to spawn items in the world, for example, at designated item spawn points or when an enemy drops loot.

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "FactoryItem.generated.h"

UCLASS(Blueprintable)
class UFactoryItem : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UFactoryItem();

public :
	virtual AActor* SpawnMPActor(int actorCode, FVector actorLoItemion, FRotator actorRotation) override;
	
};
