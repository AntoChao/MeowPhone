#pragma once

// [Meow-Phone Project]
//
// This class defines the structure of the game's save file. It is a container for all
// the data that needs to persist between game sessions, such as player settings.
//
// How to utilize in Blueprint:
// 1. You do not typically interact with this class directly in most Blueprints. The Game Instance (`UMPGI`) is responsible for creating, loading, and saving this object.
// 2. When saving the game (e.g., in a settings menu), the UI would call a "Save Game" function on the Game Instance. The Game Instance would then:
//    a. Create a new instance of `UMPSave`.
//    b. Populate its properties (`langaugeSave`, `playerNameSave`) with the current game settings.
//    c. Use the "Save Game to Slot" node to write it to disk.
// 3. When loading the game (at startup), the Game Instance uses the "Load Game from Slot" node, which returns an instance of this class. The GI then reads the properties from this object to apply the saved settings.
//
// Necessary things to define:
// - A Blueprint should be created from this class (`BP_MPSave`), but it generally requires no additional logic or properties.
// - The properties within this class (`langaugeSave`, `playerNameSave`) are the variables that will be serialized to the save file.
//
// How it interacts with other classes:
// - USaveGame: The standard Unreal class it inherits from, which provides the serialization functionality.
// - UMPGI (Game Instance): The Game Instance is the sole manager of this class. It handles the creation, saving, loading, and application of the data stored within.
// - ELanguage (Enum): Stores the player's preferred language setting.

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "../CommonEnum.h"
#include "MPSave.generated.h"

UCLASS(minimalapi)
class UMPSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	UMPSave();

// save values
public:
	UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
		ELanguage langaugeSave;

	UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
		FString playerNameSave;
	
};
