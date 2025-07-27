#pragma once

// [Meow-Phone Project]
//
// This is the base factory class for creating various actors and objects within the game.
// It provides a generalized framework for spawning, which is then specialized by its child classes.
//
// How to utilize in Blueprint:
// 1. This class is not meant to be used directly in Blueprints for spawning. Instead, you should use one of its child factory classes (e.g., UFactoryCat, UFactoryItem).
// 2. Child factory Blueprints can be created to further extend functionality.
// 3. In your game logic (e.g., in a Game Mode or Manager Blueprint), you would create an instance of a child factory.
// 4. Before using the factory, you MUST call `SetGameWorld` to provide it with the current world context. This is crucial for spawning actors.
//
// Necessary things to define:
// - `gameWorld`: This UPROPERTY must be set via the `SetGameWord` function before any spawning functions are called. Without it, the factory won't know where to spawn actors.
//
// How it interacts with other classes:
// - UWorld: It holds a reference to the UWorld to perform actor spawning operations.
// - AActor: The `SpawnMPActor` function is responsible for creating instances of AActor-derived classes.
// - Child Factories (e.g., UFactoryAbility, UFactoryCat): These classes inherit from UMPFactory and implement the specific spawning logic for different types of actors. They override `SpawnMPActor` to handle different `actorCode` values.
// - Managers (e.g., UManagerMatch): Managers will typically own instances of these factories to delegate the creation of game objects, centralizing the spawning logic.

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "MPFactory.generated.h"

class UWorld;

UCLASS(Blueprintable)
class UMPFactory : public UObject
{
	GENERATED_BODY()
	
public:
	UMPFactory();

// common
protected:
	FActorSpawnParameters spawnParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Property")
		UWorld* gameWorld;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Property")
		AActor* spawnedActor = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Property")
		UObject* spawnedObject = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		void SetGameWorld(UWorld* aWorld);

	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual AActor* SpawnMPActor(int actorCode, FVector actorLocation, FRotator actorRotation);
	
};
