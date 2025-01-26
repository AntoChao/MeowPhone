#pragma once

#include "CoreMinimal.h"
#include "MPGM.h"
#include "TimerManager.h"
#include "MPGMGameplay.generated.h"

class AMPControllerPlayer;
class AMPCharacter;

class UFactoryHuman;
class UFactoryCat;
class UFactoryEnvironment;
class UFactoryItem;
class UFactoryAbility;

enum EGPStatus;

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
		int numOfMaxHuman;
	UPROPERTY(BlueprintReadWrite, Category = "Lobby Properties")
		int numOfMaxCat;
	UPROPERTY(BlueprintReadWrite, Category = "Lobby Properties")
		int numOfHuman;
	UPROPERTY(BlueprintReadWrite, Category = "Lobby Properties")
		int numOfCat;
	UPROPERTY(BlueprintReadWrite, Category = "Lobby Properties")
		int numOfAICat;

	UPROPERTY(BlueprintReadWrite, Category = "Lobby Properties")
		TArray<AMPControllerPlayer*> allPlayersControllers;
	UPROPERTY(BlueprintReadWrite, Category = "Lobby Properties")
		TArray<AMPCharacter*> allCharacters;
	
// factories
protected:
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

// spawn location
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Spawn Properties")
		TArray<FVector> allCatSpawnLocations;
	UPROPERTY(BlueprintReadWrite, Category = "Spawn Properties")
		TArray<FRotator> allCatSpawnRotations;
	UPROPERTY(BlueprintReadWrite, Category = "Spawn Properties")
		TArray<FVector> allHumanSpawnLocations;
	UPROPERTY(BlueprintReadWrite, Category = "Spawn Properties")
		TArray<FRotator> allHumanSpawnRotations;

// game process
protected:
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		EGPStatus curGameplayStatus;
	
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		int prepareTime;
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		int gameplayTime;
	
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		int curCountDown;
	UPROPERTY(BlueprintReadWrite, Category = "GameProgress Properties")
		FTimerHandle countDownTimer;
	
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void CountDown();
public:
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		bool IsEnableToStartGame();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void StartGame();

	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void CheckIfGameEnd();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void EndGame();	

protected:
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupGame();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupMap();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupHumans();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupCats();

	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void StartPrepareTime();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void EndPrepareTime();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void StartGameplayTime();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void EndGameplayTime();
};
