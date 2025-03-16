#pragma once

#include "CoreMinimal.h"
#include "CommonEnum.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class ELanguage : uint8 {
	EEnglish,
	EChinese,
	ESpanish
};

UENUM(BlueprintType, Blueprintable)
enum class EGPStatus : uint8 {
	ELobby,
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
	EGameplayHuman,
	EGameplayCat,
	EMenu,
	EEnd
};

UENUM(BlueprintType, Blueprintable)
enum class ETeam : uint8 {
	EHuman,
	ECat
};

UENUM(BlueprintType, Blueprintable)
enum class ECatRace : uint8 {
	ECatExp,
	ECat1,
	ECat2,
	EDiedCat
};

UENUM(BlueprintType, Blueprintable)
enum class EHumanProfession : uint8 {
	EHumanExp,
	EHuman1,
	EHuman2,
	EDiedHuman
};
/**/
UENUM(BlueprintType, Blueprintable)
enum class EHumanAction : uint8 {
	ENone
};

UENUM(BlueprintType, Blueprintable)
enum class ECatAction : uint8 {
	EStraggle,
	EInteractHuman,
	EInteractCat,
	EInteracEnvActor,
	EPickupItem,
	ENone
};

UENUM(BlueprintType, Blueprintable)
enum class ECatAnimMontage : uint8 {
	EPush,
	EUseMeowPhone,
	ENone
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
	ENone
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