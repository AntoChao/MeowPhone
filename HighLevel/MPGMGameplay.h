#pragma once

#include "CoreMinimal.h"
#include "MPGM.h"
#include "TimerManager.h"
#include "MPGMGameplay.generated.h"

class AMPGS;

class UFactoryAIController;
class UFactoryHuman;
class UFactoryCat;
class UFactoryEnvironment;
class UFactoryItem;
class UFactoryAbility;

class AMPControllerPlayer;
class AMPCharacter;
class AMPEnvActorComp;

class AMPAISystemManager;

enum class EMPEnvActor : uint8;
enum class EMPItem : uint8;
enum class EAbility : uint8;

UCLASS(minimalapi)
class AMPGMGameplay : public AMPGM
{
	GENERATED_BODY()
	
public:
	AMPGMGameplay();

// gameplay common
public:
	virtual void PostLogin(APlayerController* newPlayer) override;
	
	virtual void Logout(AController* exiting) override;
	
	virtual void BeginPlay() override;

// game state initialize
protected:
	UPROPERTY(BlueprintReadWrite, Category = "GameState Properties")
		AMPGS* theGameState;
	UFUNCTION(BlueprintCallable, Category = "GameState Methods")
		void InitializeGameState();

// gameplay lobby manager
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Properties")
		int numOfPlayers;
	UPROPERTY(BlueprintReadWrite, Category = "Lobby Properties")
		TArray<AMPControllerPlayer*> allPlayersControllers;

	UFUNCTION(BlueprintCallable, Category = "Lobby Methods")
		void RemoveControlledCharacters(AMPControllerPlayer* aPlayer);
	
// gameplay custom character 
protected:

// gameplay
	// gameplay Setup
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		TArray<AMPCharacter*> allPlayerCharacters;
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		TArray<AMPCharacter*> allAICats;
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		TArray<AMPCharacter*> allAIHumans;
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		AMPAISystemManager* theAIManager = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		int numberAICats = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		int numberAIHumans = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		int aiManagerIndex = 0; 
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		int catAIIndex = 1; 
		// it would be good for passing as string isntead of integers
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		int humanAIIndex = 2;

	// spawn location
	protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties")
		int itemRemainPercentage = 75;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties")
		int envActorRandomnessPercentage = 80;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties")
		TArray<FVector> allHumanSpawnLocations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties")
		TArray<FRotator> allHumanSpawnRotations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties")
		TArray<FVector> allCatSpawnLocations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties")
		TArray<FRotator> allCatSpawnRotations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties")
		FVector spectatorSpawnLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties")
		FRotator spectatorSpawnRotation;
		
// factories
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory Properties")
		TSubclassOf<UFactoryAIController> aiControllerFactoryClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory Properties")
		TSubclassOf<UFactoryHuman> humanFactoryClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory Properties")
		TSubclassOf<UFactoryCat> catFactoryClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory Properties")
		TSubclassOf<UFactoryEnvironment> environmentFactoryClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory Properties")
		TSubclassOf<UFactoryItem> itemFactoryClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Factory Properties")
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
		AMPEnvActorComp* SpawnEnvironment(EEnvActor envTag, FVector spawnLocation, FRotator spawnRotation);
	UFUNCTION(BlueprintCallable, Category = "Factory Methods")
		AMPItem* SpawnItem(EItem itemTag, FVector spawnLocation, FRotator spawnRotation);
	UFUNCTION(BlueprintCallable, Category = "Factory Methods")
		UMPAbility* SpawnAbility(AActor* abilityOwner, EAbility abilityTag);

// game process
protected:
	FTimerHandle readyTimerHandle;
	FTimerHandle customCharacterTimerHandle;
	FTimerHandle prepareTimerHandle;
	FTimerHandle gameplayTimerHandle;

public:
	// lobby
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

	// character custom
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void StartCustomizeCharacter();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void CountdownCustomizeCharacter();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void EndCustomizeCharacter();
	
		// start game
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void StartGame();

protected :
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupGame();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupMap();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupMapItems();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupMapEnvActors();

	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupPlayers();

	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupAIs();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupAICats();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupAIHumans();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void SetupAIManager();

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
		void RegisterPlayerDeath(AMPControllerPlayer* diedPlayer,
			FVector diedPlayerLocation, FRotator diedPlayerRotation);
	
// setters and getters
public :
	UFUNCTION(BlueprintCallable, Category = "Setter && Getter")
		void SetNumAICats(int aiCatNum);
	UFUNCTION(BlueprintCallable, Category = "Setter && Getter")
		void SetNumAIHumans(int aiHumanNum);

// testing
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing Properties")
		bool testingLobby = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Testing Properties")
		bool onlyTestStartGame = true;
};
