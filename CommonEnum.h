#pragma once

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
	ESpanish
};

UENUM(BlueprintType, Blueprintable)
enum class EWindowMode : uint8 {
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
