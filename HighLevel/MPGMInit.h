#pragma once

// [Meow-Phone Project]
//
// This is a specialized Game Mode intended for use in the initialization or main menu level.
// Its purpose is to be a lightweight Game Mode that does not load all the complex managers
// and factories that `AMPGMGameplay` does. This keeps the main menu level fast to load and
// efficient.
//
// How to utilize in Blueprint:
// 1. Create a Blueprint from this class (e.g., `BP_MPGMInit`).
// 2. Set this Blueprint as the "Default Game Mode" for your main menu or initialization level in the World Settings.
// 3. This Game Mode generally does not require any additional logic in its Blueprint, as it's primarily used to ensure the correct default pawn, player controller, and HUD are used for the menu.
//
// Necessary things to define:
// - In the `BP_MPGMInit` Blueprint, you should set the default classes, such as:
//   - `Default Pawn Class`: Should be `nullptr` or a simple spectator pawn, as there is no playable character in the menu.
//   - `HUD Class`: Should be set to your main menu HUD Blueprint (e.g., `BP_HUDInit`).
//   - `PlayerController Class`: Should be set to a player controller suitable for UI interaction (e.g., `AMPControllerPlayer`).
//
// How it interacts with other classes:
// - AMPGM: Inherits the base class, giving it the cached reference to the Game Instance.
// - HUDInit: This Game Mode is configured to create and display the initial UI for the main menu.
// - UMPGI (Game Instance): It interacts with the Game Instance to initiate actions like hosting or joining a session, which will then trigger a level transition to the gameplay map (which uses `AMPGMGameplay`).

#include "CoreMinimal.h"
#include "MPGM.h"
#include "MPGMInit.generated.h"

UCLASS(minimalapi)
class AMPGMInit : public AMPGM
{
	GENERATED_BODY()
	
public:
	AMPGMInit();
};
