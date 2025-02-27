#pragma once

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
