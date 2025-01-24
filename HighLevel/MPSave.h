#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MPSave.generated.h"

UCLASS(minimalapi)
class MEOWPHONE_API UMPSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	UMPSave();

};
