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