#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MPGS.generated.h"

class AMPCharacterHuman;
class AMPCharacterCat;
class AMPItem;
class AMPEnvActorComp;

enum class EGPStatus : uint8;

/* Some thoughts
The reason why GameState should hold all attributes related with the game
Its because it allows easy access for players
As GameState is exisitng in each player instance. There is no need for player to use server rpc
*/
UCLASS(minimalapi)
class AMPGS : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AMPGS();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

// common GS properties
public:
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Common Properties")
		TArray<AMPCharacterHuman*> allHumans;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Common Properties")
		TArray<AMPCharacterCat*> allCats;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Common Properties")
		TArray<AMPItem*> allItems;
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Common Properties")
		TArray<AMPEnvActorComp*> allEnvActors;

	// Gameplay progression
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		EGPStatus curGameplayStatus;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GameProgress Properties")
		bool isMostPlayerReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameProgress Properties")
		int readyTotalTime;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "GameProgress Properties")
		int curReadyTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameProgress Properties")
		int prepareTotalTime;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "GameProgress Properties")
		int curPrepareTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameProgress Properties")
		int gameplayTotalTime;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "GameProgress Properties")
		int curGameplayTime;
	
	// MP Progression
	UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
		float totalMPProgression;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Common Properties")
		float curMPProgression;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Common Properties")
		float curMPProgressionPercentage;

	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		void UpdateMPProgression(int modifier);
};
