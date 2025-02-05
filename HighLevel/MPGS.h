#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AMPGS.generated.h"

class AMPCharacterHuman;
class AMPCharacterCat;
class AMPItem;
class AMPEnvActor;

UCLASS(minimalapi)
class MEOWPHONE_API AMPGS : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AMPGS();

// common GS properties
public:
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Common Properties")
		TArray<AMPCharacterHuman*> allHumans;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Common Properties")
		TArray<AMPCharacterCat*> allCats;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Common Properties")
		TArray<AMPItem*> allItems;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Common Properties")
		TArray<AMPEnvActor*> allEnvActors;

	UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
		float totalMPProgression;
	UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
		float curMPProgression;
	UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
		float curMPProgressionPercentage;

	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		void UpdateMPProgression(int modifier);
};
