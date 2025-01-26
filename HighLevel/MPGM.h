#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MPGM.generated.h"

class UMPGI;

UCLASS(minimalapi)
class MEOWPHONE_API AMPGM : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMPGM();

// common gamemode instance
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
		UMPGI* mpGameInstance;

public:
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual void BeginPlay() override;
};
