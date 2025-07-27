#pragma once

// [Meow-Phone Project]
//
// This header file is the central repository for almost all enumerations (Enums) used
// throughout the project. Enums are used to create a set of named integral constants,
// which makes the code more readable and less prone to errors than using raw numbers.
//
// How to utilize in Blueprint:
// - Any enum marked with `UENUM(BlueprintType)` is accessible in Blueprints. They can be used as variable types, function parameters, and in `Switch` nodes.
// - This allows Blueprints to work with well-defined states and types, for example, checking `if (playerTeam == ETeam::ECat)` instead of `if (playerTeam == 2)`.
//
// How it interacts with other classes:
// - This file is one of the most widely included headers in the project. Nearly every major class, from characters and items to UI widgets and game modes, includes this file to use these enums.
// - For example:
//   - `EGameLevel` is used by the Game Instance to open the correct map.
//   - `ETeam` is used by Player States, Game Modes, and Characters to define allegiances.
//   - `ECatRace` and `EHumanProfession` are used in character customization and factories.
//   - The animation state enums (`EMoveState`, `ECatPosture`, etc.) are used by the Character classes to drive their Animation Blueprints.
//   - `EHUDType` is used by the Player Controller to manage which UI widget is currently active.
//
// Summary of Enum Categories:
// - **System & Settings**: `ELogLevel`, `EGameLevel`, `EHUDType`, `ELanguage`, `EWindowModeOur`, etc. These define application-level states and options.
// - **Gameplay State**: `EGPStatus`, `ETeam`. These define the high-level state of the match and players.
// - **Gameplay Types**: `ECatRace`, `EHumanProfession`, `EEnvActor`, `EItem`, `EAbility`, `EHat`. These define the specific "types" of various game entities.
// - **Animation States**: `EMoveState`, `EAirState`, `ECatPosture`, `EHumanPosture`, etc. These are used exclusively by the animation system to define a character's current pose and action.
// - **AI States**: `EAICatState`, `EAIHumanState`. These are used in Behavior Trees and Blackboards to control AI decision-making.

#include "CoreMinimal.h"
#include "CommonEnum.generated.h"

// Logging system for proper debug output
UENUM(BlueprintType)
enum class ELogLevel : uint8 {
	None = 0,
	Error = 1,
	Warning = 2,
	Info = 3,
	Debug = 4,
	Verbose = 5
};

UENUM(BlueprintType, Blueprintable)
enum class EGameLevel : uint8 {
	EInit UMETA(DisplayName = "Init Level"),
	EGameplay UMETA(DisplayName = "Gameplay Level")
};

UENUM(BlueprintType, Blueprintable)
enum class EGPStatus : uint8 {
	ELobby,
	ECustomCharacter,
	EPrepare,
	EGameplay
};

UENUM(BlueprintType, Blueprintable)
enum class EHUDType : uint8 {
	EInit,
	EOption,
	ESessionGeneral,
	ECredit,
	ECreateSession,
	ESearchSession,
	ELobby,
	ECustomHuman,
	ECustomCat,
	EGameplayHuman,
	EGameplayCat,
	EMenu,
	EEnd
};

UENUM(BlueprintType, Blueprintable)
enum class ELanguage : uint8 {
	EEnglish,
	EChinese,
	ESpanish,
	Max      UMETA(Hidden)
};

UENUM(BlueprintType, Blueprintable)
enum class EWindowModeOur : uint8 {
	FullScreen UMETA(DisplayName = "Full Screen"),
	FullScreenWindow UMETA(DisplayName = "Full Screen Window"),
	Window UMETA(DisplayName = "Window")
};

UENUM(BlueprintType, Blueprintable)
enum class EGraphicsQuality : uint8 {
	Low UMETA(DisplayName = "Low"),
	Medium UMETA(DisplayName = "Medium"),
	High UMETA(DisplayName = "High"),
	Ultra UMETA(DisplayName = "Ultra")
};

UENUM(BlueprintType, Blueprintable)
enum class ESyncMode : uint8 {
	On UMETA(DisplayName = "On"),
	Off UMETA(DisplayName = "Off")
};

UENUM(BlueprintType, Blueprintable)
enum class ETeam : uint8 {
	ENone,
	EHuman,
	ECat
};

UENUM(BlueprintType, Blueprintable)
enum class ECatRace : uint8 {
	ECatExp,
	ECat1,
	ECat2,
	ECat3,
	EDiedCat,
	ECatMax
};

UENUM(BlueprintType, Blueprintable)
enum class EHumanProfession : uint8 {
	EHumanExp,
	EHuman1,
	EHuman2,
	EHuman3,
	EDiedHuman,
	EHumanMax
};

/*
Pushable means that cat/ human can push the object away
ECustom means that cat/ human can Interact with them. But they can also allows physics (chairs)
*/
UENUM(BlueprintType, Blueprintable)
enum class EEnvActor : uint8 {
	ENone,
	EPushable, // small objects to throw
	ECustom, // window, machine -> all the duration ones
};


/* Declaration
The difference between direct usage vs duration usage lies on the effect
	A direct usage means that it only requires one single input and directly effect
	A duration usage means that the effect is not instant, not a pick up or consumable..
*/
UENUM(BlueprintType, Blueprintable)
enum class EEnvActorType : uint8 {
	EDirectInteract,
	EDurationInteract
};

UENUM(BlueprintType, Blueprintable)
enum class EItem : uint8 {
	ENone
};

UENUM(BlueprintType, Blueprintable)
enum class EItemType : uint8 {
	EDirectUse,
	EDurationUse
};

UENUM(BlueprintType, Blueprintable)
enum class EAbility : uint8 {
	ENone,
	EAbility1,
	EAbility2,
	EAbility3,
	EAbilityMax
};

UENUM(BlueprintType, Blueprintable)
enum class EHat : uint8 {
	ENone,
	ECap,
	ECrown,
	EHat1,
	EHat2,
	EHat3,
	EHatMax
};

UENUM(BlueprintType, Blueprintable)
enum class EAbilityType : uint8 {
	EDirectUse,
	EDurationUse
};

UENUM(BlueprintType, Blueprintable)
enum class EAICatPersonality : uint8 {
	ECat1
};

// ---------- Animation / Pose enums ----------

UENUM(BlueprintType)
enum class EMoveState : uint8 {
    Idle,
    Walk,
    Run
};

// In-air phase
UENUM(BlueprintType)
enum class EAirState : uint8 {
    Grounded,
    Jump,
    Falling,
};

UENUM(BlueprintType)
enum class ECatPosture : uint8 {
    Standing,
    Crouching,
    Sitting,
    Lying
    // Removed BeingHeld and Rubbing - these are now in ECatInteractionState
};

UENUM(BlueprintType)
enum class EHumanPosture : uint8 {
    Standing,
    Crouching
    // Removed Holding and BeingRubbed - these are now in EHumanInteractionState
};

// New interaction state enums
UENUM(BlueprintType)
enum class ECatInteractionState : uint8 {
    None,
    BeingHeld,
    BeingRubbed
};

UENUM(BlueprintType)
enum class EHumanInteractionState : uint8 {
    None,
    HoldingCat,
    BeingRubbed
};

// Context actions are species-specific to avoid name clashes
UENUM(BlueprintType)
enum class ECatContext : uint8 {
    None,
	BeStunned,
	VerticalJump,
    LongFalling,
	StandStretch,
    SitScratch,
	SitPaw,
	SitBiscuit,
    SitDrinkWater,
    LyingSleepSpot
};

UENUM(BlueprintType)
enum class EHumanContext : uint8 {
    None,
	BeStunned,
    HoldCat,
    StopHoldCat,
    BeStruggleOut,
    PutCatInCage,
    DieEffect,
	TurnOnPower,
};

// AI state enums
UENUM(BlueprintType)
enum class EAICatState : uint8
{
    Idle            UMETA(DisplayName="Idle"),
    Wander          UMETA(DisplayName="Wander"),
    InteractHuman   UMETA(DisplayName="InteractHuman"),
    InteractCat     UMETA(DisplayName="InteractCat"),
    PushEnvActor    UMETA(DisplayName="PushEnvActor"),
    Jump            UMETA(DisplayName="Jump"),
    DoubleJump      UMETA(DisplayName="DoubleJump"),
    Falling         UMETA(DisplayName="Falling"),
    HeldByHuman     UMETA(DisplayName="HeldByHuman"),
    Stunned         UMETA(DisplayName="Stunned")
};

UENUM(BlueprintType)
enum class EAIHumanState : uint8
{
    Idle            UMETA(DisplayName="Idle"),
    Wander          UMETA(DisplayName="Wander"),
    Search          UMETA(DisplayName="Search"),
    HoldCat         UMETA(DisplayName="HoldCat"),
    UseItem         UMETA(DisplayName="UseItem"),
    InteractContext UMETA(DisplayName="InteractContext"),
    Stunned         UMETA(DisplayName="Stunned")
};

// enum
UENUM(BlueprintType)
enum class EMPItem : uint8
{
	none,
	catFood,
};