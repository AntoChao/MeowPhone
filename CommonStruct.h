#pragma once

// [Meow-Phone Project]
//
// This header file is the central repository for most custom data structures (Structs)
// used throughout the project. Structs are used to group related variables into a single,
// convenient package.
//
// How to utilize in Blueprint:
// - Any struct marked with `USTRUCT(BlueprintType)` is accessible in Blueprints. They can be used as variable types, function parameters, and return values. You can also use "Make" and "Break" nodes to construct them or access their members.
//
// How it interacts with other classes:
// - Like `CommonEnum.h`, this file is widely included across the project by any class that needs to use one of these data structures.
// - For example:
//   - `FLogConfig` is used by `UManagerLog` to configure its behavior.
//   - `FSessionInfo` is used by the `UMPGI` and `UHUDSearchSession` widgets to store and display data about found multiplayer games.
//   - `FCatAnimState` and `FHumanAnimState` are critical structs used by the `AMPCharacterCat` and `AMPCharacterHuman` classes, respectively. These structs are replicated and contain all the information their Animation Blueprints need to drive the animation state machines.
//   - `FLocalizedText` is likely the base struct for the rows in the localization DataTable.
//   - `FCreditEntryData` is used by the `UHUDCredit` widget to populate its list of credits.

#include "CoreMinimal.h"
#include "CommonEnum.h"
#include "CommonStruct.generated.h"

class AMissionZone;
class MPCharacter;

// Logging configuration structure
USTRUCT(BlueprintType)
struct FLogConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logging")
	ELogLevel logLevel = ELogLevel::Warning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logging")
	bool enableScreenLogging = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logging")
	bool enableFileLogging = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logging")
	float screenLogDuration = 5.0f;

	FLogConfig()
	{
		logLevel = ELogLevel::Warning;
		enableScreenLogging = false;
		enableFileLogging = false;
		screenLogDuration = 5.0f;
	}
};

USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	int32 sessionIndex;

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	FString sessionName;

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	FString hostName;

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	int32 curPlayersNum;

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	int32 maxPlayersNum;

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	int32 ping;

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	bool usePassword;
};

USTRUCT(BlueprintType)
struct FCatAnimState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    ECatPosture curPosture = ECatPosture::Standing;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    EMoveState curMove = EMoveState::Idle;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    EAirState curAir = EAirState::Grounded;

    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    ECatContext curContext = ECatContext::None;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    ECatInteractionState curInteraction = ECatInteractionState::None;
};

USTRUCT(BlueprintType)
struct FHumanAnimState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    EHumanPosture curPosture = EHumanPosture::Standing;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    EMoveState curMove = EMoveState::Idle;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    EAirState curAir = EAirState::Grounded;

    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    EHumanContext curContext = EHumanContext::None;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    EHumanInteractionState curInteraction = EHumanInteractionState::None;
};

// Localization Text Structure
USTRUCT(BlueprintType)
struct FLocalizedText
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString textKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FText englishText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FText chineseText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FText spanishText;

    // Helper function to get text based on current language
    FText GetLocalizedText(ELanguage language) const
    {
        switch (language)
        {
        case ELanguage::EEnglish:
            return englishText;
        case ELanguage::EChinese:
            return chineseText;
        case ELanguage::ESpanish:
            return spanishText;
        default:
            return englishText;
        }
    }
};

// Resolution Structure for Settings
USTRUCT(BlueprintType)
struct FResolution
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resolution")
    int32 width;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resolution")
    int32 height;

    FResolution() : width(1920), height(1080) {}
    FResolution(int32 w, int32 h) : width(w), height(h) {}

    // Helper function to get display string
    FString GetDisplayString() const
    {
        return FString::Printf(TEXT("%dx%d"), width, height);
    }

    // Comparison operators
    bool operator==(const FResolution& other) const
    {
        return width == other.width && height == other.height;
    }

    bool operator!=(const FResolution& other) const
    {
        return !(*this == other);
    }
};

USTRUCT(BlueprintType)
struct FCreditEntryData
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credit")
    FString AssetName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credit")
    FString Usage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credit")
    FString Author;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credit")
    FString Source;
};
