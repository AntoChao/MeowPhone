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
	EPlaying
};

UENUM(BlueprintType, Blueprintable)
enum class ETeam : uint8 {
	EHuman,
	ECat
};

UENUM(BlueprintType, Blueprintable)
enum class ECharacterHumanAction : uint8 {
	ENone
};

UENUM(BlueprintType, Blueprintable)
enum class ECharacterCatAction : uint8 {
	ENone
};

UENUM(BlueprintType, Blueprintable)
enum class EItem : uint8 {
	ENone
};

UENUM(BlueprintType, Blueprintable)
enum class EAbility : uint8 {
	ENone
};