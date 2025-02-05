#pragma once

#include "CoreMinimal.h"
#include "MPGM.h"
#include "TimerManager.h"
#include "MPGMGameplay.generated.h"

class AActor;

class AMPControllerPlayer;
class AMPCharacter;

class UFactoryHuman;
class UFactoryCat;
class UFactoryEnvironment;
class UFactoryItem;
class UFactoryAbility;

enum class EGPStatus : uint8;
enum class EEnvActor : uint8;
enum class EItem : uint8;
enum class EAbility : uint8;

UCLASS(minimalapi)
class MEOWPHONE_API AMPGMGameplay : public AMPGM
{
	GENERATED_BODY()
	
public:
	AMPGMGameplay();

// gameplay common
public:
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual void PostLogin(APlayerController* newPlayer) override;
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual void Logout(APlayerController* exiting) override;
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual void BeginPlay() override;

// gameplay lobby manager
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Lobby Properties")
		TArray<int> playerCatIndeces;
	UPROPERTY(BlueprintReadWrite, Category = "Lobby Properties")
		int numOfCats; // include AI
	UPROPERTY(BlueprintReadWrite, Category = "Lobby Properties")
		int totalCatRacesNum;
	UPROPERTY(BlueprintReadWrite, Category = "Lobby Properties")
		int totalAITypesNum;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby Properties")
		TArray<AMPControllerPlayer*> allPlayersControllers;
	UPROPERTY(BlueprintReadWrite, Category = "Lobby Properties")
		TArray<AMPCharacter*> allPlayerCharacters;
	UPROPERTY(BlueprintReadWrite, Category = "Lobby Properties")
		TArray<AMPCharacter*> allAICharactersCats;

	UFUNCTION(BlueprintCallable, Category = "Lobby Methods")
		void RemoveControlledCharacters(AMPControllerPlayer* aPlayer);
	
// factories
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Factory Properties")
		TSubclassOf<UFactoryAIController> aiControllerFactoryClass;
	UPROPERTY(BlueprintReadWrite, Category = "Factory Properties")
		TSubclassOf<UFactoryHuman> humanFactoryClass;
	UPROPERTY(BlueprintReadWrite, Category = "Factory Properties")
		TSubclassOf<UFactoryCat> catFactoryClass;
	UPROPERTY(BlueprintReadWrite, Category = "Factory Properties")
		TSubclassOf<UFactoryEnvironment> environmentFactoryClass;
	UPROPERTY(BlueprintReadWrite, Category = "Factory Properties")
		TSubclassOf<UFactoryItem> itemFactoryClass;
	UPROPERTY(BlueprintReadWrite, Category = "Factory Properties")
		TSubclassOf<UFactoryAbility> abilityFactoryClass;

	UPROPERTY(BlueprintReadWrite, Category = "Factory Properties")
		UFactoryAIController* aiControllerFactoryInstance;
	UPROPERTY(BlueprintReadWrite, Category = "Factory Properties")
		UFactoryHuman* humanFactoryInstance;
	UPROPERTY(BlueprintReadWrite, Category = "Factory Properties")
		UFactoryCat* catFactoryInstance;
	UPROPERTY(BlueprintReadWrite, Category = "Factory Properties")
		UFactoryEnvironment* environmentFactoryInstance;
	UPROPERTY(BlueprintReadWrite, Category = "Factory Properties")
		UFactoryItem* itemFactoryInstance;
	UPROPERTY(BlueprintReadWrite, Category = "Factory Properties")
		UFactoryAbility* abilityFactoryInstance;

	UFUNCTION(BlueprintCallable, Category = "Factory Methods")
		void SetupFactoryInstances();

public :
	UFUNCTION(BlueprintCallable, Category = "Factory Methods")
		AMPEnvActor* SpawnEnvironment(EEnvActor envTag, FVector spawnLocation, FRotator spawnRotation);
	UFUNCTION(BlueprintCallable, Category = "Factory Methods")
		AMPItem* SpawnItem(EItem itemTag, FVector spawnLocation, FRotator spawnRotation);
	UFUNCTION(BlueprintCallable, Category = "Factory Methods")
		UMPAbility* SpawnAbility(AActor* owner, EAbility abilityTag);
	
// spawn location
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Spawn Properties")
		int itemRemainPercentage = 75%;
	UPROPERTY(BlueprintReadWrite, Category = "Spawn Properties")
		TArray<FVector> allHumanSpawnLocations;
	UPROPERTY(BlueprintReadWrite, Category = "Spawn Properties")
		TArray<FRotator> allHumanSpawnRotations;

	UPROPERTY(BlueprintReadWrite, Category = "Spawn Properties")
		TArray<FVector> allCatSpawnLocations;
	UPROPERTY(BlueprintReadWrite, Category = "Spawn Properties")
		TArray<FRotator> allCatSpawnRotations;

	UPROPERTY(BlueprintReadWrite, Category = "Spawn Properties")
		FVector spectatorSpawnLocation;
	UPROPERTY(BlueprintReadWrite, Category = "Spawn Properties")
		FRotator spectatorSpawnRotation;

// game process
protected:
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		EGPStatus curGameplayStatus;
	
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		bool isReady = false;
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		int readyTotalTime;
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		int curReadyTime;
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		int prepareTotalTime;
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		int curPrepareTime;
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		int gameplayTotalTime;
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		int curGameplayTime;
	
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		FTimerHandle readyTimerHandle;
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		FTimerHandle prepareTimerHandle;
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		FTimerHandle gameplayTimerHandle;

public:
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void StartLobby();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void GotReady(AMPControllerPlayer* aPlayer);
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		bool CheckReadyToStartGame();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		bool CheckBothTeamHasPlayers();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		bool CheckHalfPlayersAreReady();
	
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void CountdownReadyGame();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void EndReadyTime();

	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void StartGame();

protected :
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupGame();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupMap();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupHumans();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupAllCats();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupPlayerCats();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupAICats();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupGameplayHUD();

	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void StartPrepareTime();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void CountdownPrepareGame();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void EndPrepareTime();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void StartGameplayTime();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void CountdownGameplayGame();

public :
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		bool CheckIfGameEnd();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void EndGameplayTime();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void RemoveGameplayHUD();

	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void RegisterPlayerDeath(AMPControllerPlayer* diedPlayer);
	
};
