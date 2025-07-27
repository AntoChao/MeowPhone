#pragma once

// [Meow-Phone Project]
//
// This factory is responsible for creating AI Controllers for different teams (e.g., Cat, Human).
// It simplifies the process of spawning the correct AI Controller based on a team enum.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint class that inherits from `UFactoryAIController`.
// 2. In this Blueprint, you MUST assign the `Cat AI Controller Class` and `Human AI Controller Class` properties. These should be set to your specific AI Controller Blueprints (e.g., `BP_CatAIController`, `BP_HumanAIController`).
// 3. In your game logic (e.g., in a Game Mode or a Character Spawner), create an instance of your `UFactoryAIController` Blueprint.
// 4. Ensure the `gameWorld` is set by calling `SetGameWorld` (inherited from UMPFactory).
// 5. Call the `SpawnAIController` function, passing in the desired `ETeam`. This will spawn and return the correct AI Controller for that team.
//
// Necessary things to define:
// - `catAIControllerClass`: In the Blueprint editor, you must set this to the AI Controller class that should be used for the Cat team.
// - `humanAIControllerClass`: In the Blueprint editor, you must set this to the AI Controller class that should be used for the Human team.
//
// How it interacts with other classes:
// - UMPFactory: It inherits the base spawning functionality. The `SpawnMPActor` override is less likely to be used directly in favor of the more specific `SpawnAIController`.
// - AMPAIController: This factory spawns instances of `AMPAIController` child classes.
// - ETeam (Enum): The `SpawnAIController` function uses this enum to decide which AI Controller class to spawn.
// - Game Mode / Spawning Logic: The Game Mode (like `AMPGMGameplay`) will likely use this factory when a new character is spawned into the world to assign it an appropriate AI Controller.

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "FactoryAIController.generated.h"

class AMPAIController;

UCLASS(Blueprintable)
class UFactoryAIController : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UFactoryAIController();

public :
	virtual AActor* SpawnMPActor(int actorCode, FVector actorLoItemion, FRotator actorRotation) override;

	// Spawn AI controller by team
	UFUNCTION(BlueprintCallable, Category = "AI Controller Factory")
	AMPAIController* SpawnAIController(ETeam team);

// all tsubclassof
protected :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Factory Stats")
		TSubclassOf<AMPAIController> catAIControllerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Factory Stats")
		TSubclassOf<AMPAIController> humanAIControllerClass;
};
