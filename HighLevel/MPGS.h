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
	
		// lobby
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "GameProgress Properties")
		bool isMostPlayerReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameProgress Properties")
		int readyTotalTime;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "GameProgress Properties")
		int curReadyTime;
	
		// custom character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameProgress Properties")
		int customCharacterTotalTime;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "GameProgress Properties")
		int curCustomCharacterTime;

		// gameplay -> MP Progression
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameProgress Properties")
		int prepareTotalTime;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "GameProgress Properties")
		int curPrepareTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameProgress Properties")
		int gameplayTotalTime;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "GameProgress Properties")
		int curGameplayTime;

		UPROPERTY(Replicated, BlueprintReadWrite, Category = "Common Properties")
		float totalMPProgression;
	UPROPERTY(ReplicatedUsing = OnRep_CurMPProgression, BlueprintReadWrite, Category = "Common Properties")
		float curMPProgression;
	UPROPERTY(ReplicatedUsing = OnRep_CurMPProgressionPercentage, BlueprintReadWrite, Category = "Common Properties")
		float curMPProgressionPercentage;

	// Cat team objective: percentage of total progression needed to win (default 80%)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
		float catWinProgressionPercentage = 0.8f; // 80% default

	// Human team progression (cat catching)
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Common Properties")
		int32 totalCatPlayers = 0;
	UPROPERTY(ReplicatedUsing = OnRep_CaughtCats, BlueprintReadWrite, Category = "Common Properties")
		int32 caughtCats = 0;
	UPROPERTY(ReplicatedUsing = OnRep_CaughtCatsPercentage, BlueprintReadWrite, Category = "Common Properties")
		float caughtCatsPercentage = 0.0f;

	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		void UpdateMPProgression(int modifier);
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		void UpdateHumanProgression(int modifier);

	UFUNCTION()
		void OnRep_CurMPProgression();
	UFUNCTION()
		void OnRep_CurMPProgressionPercentage();
	UFUNCTION()
		void OnRep_CaughtCats();
	UFUNCTION()
		void OnRep_CaughtCatsPercentage();
};
