#pragma once

#include "CoreMinimal.h"
#include "CommonStruct.generated.h"

USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	int32 sessionIndex;

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	FString sessionName;

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	int32 curPlayersNum;

	UPROPERTY(BlueprintReadOnly, Category = "Session Info")
	int32 maxPlayersNum;
};

USTRUCT(BlueprintType)
struct FLocalizedText
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString englishStr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString chineseStr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString spanishStr;

    FString GetTextForCurrentLanguage(ELanguage curLanguage) const
    {
        switch (curLanguage)
        {
        case ELanguage::EEnglish:
            return englishStr;
        case ELanguage::EChinese:
            return chineseStr;
        case ELanguage::ESpanish:
            return spanishStr;
        default:
            return englishStr;
        }
    }
};