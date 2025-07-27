#pragma once

// [Meow-Phone Project]
//
// This is the base class for all Manager objects in the game. Its primary purpose is to
// provide a common foundation for all managers and to hold a reference to the main
// gameplay Game Mode (`AMPGMGameplay`).
//
// How to utilize in Blueprint:
// - You should not create Blueprints directly from this class. Instead, you should create Blueprints from its child classes (e.g., `UManagerLobby`, `UManagerMatch`).
// - This class provides the `InitializeManager` function, which is intended to be called by the Game Mode on all its managers at the start of a match. This ensures every manager has a valid reference to the Game Mode.
//
// Necessary things to define:
// - Nothing needs to be defined in the editor for this class. Its `gameMode` property is set at runtime.
//
// How it interacts with other classes:
// - UObject: It is a base UObject, meaning it is not an Actor and cannot be placed in a level. It is a data/logic class.
// - AMPGMGameplay: It holds a pointer to the main Game Mode. This allows any child manager class to easily access the Game Mode and, through it, other managers, the game state, etc.
// - Child Managers (UManagerLobby, UManagerMatch, etc.): All other managers inherit from this class to gain the `gameMode` reference and to be part of the common manager framework, which allows the Game Mode to manage them polymorphically.

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ManagerMP.generated.h"

class AMPGMGameplay;

UCLASS()
class UManagerMP : public UObject
{
    GENERATED_BODY()

protected:
    AMPGMGameplay* gameMode;

public:
    virtual void InitializeManager(AMPGMGameplay* inGameMode);
}; 