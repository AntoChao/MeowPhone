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
class AMPAIController;

class UManagerLobby;
class UManagerPreview;
class UManagerMatch;
class UManagerAIController;

// Old deprecated enum names removed. Use correct ones below.
// Correct forward declarations for enums used in this header
enum class EEnvActor : uint8;
enum class EMPItem : uint8;
enum class EAbility : uint8;
enum class ETeam : uint8;

UCLASS()
class AMPGMGameplay : public AMPGM
{
	GENERATED_BODY()

// gameplay common
public:
	AMPGMGameplay();
	~AMPGMGameplay();

public:
	virtual void PostLogin(APlayerController* newPlayer) override;
	virtual void Logout(AController* exiting) override;
	virtual void BeginPlay() override;

// game state common
protected:
	UPROPERTY(BlueprintReadWrite, Category = "GameState Properties")
		AMPGS* theGameState;
	UFUNCTION(BlueprintCallable, Category = "GameState Methods")
		void InitializeGameState();

public:
	UFUNCTION(BlueprintCallable, Category = "GameState Methods")
	AMPGS* GetGameState() const { return theGameState; }
	
// factories common
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

	UFUNCTION(BlueprintCallable, Category = "Factory Methods")
	void InitializeFactoryInstances();

public:
	// Factory instances now public to allow access from manager classes
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

// debug support
protected:
	// Debug mode settings - can be changed in Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Settings")
		bool singlePlayerDebugMode = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Settings")
		bool multiplayerDebugMode = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Settings")
		ETeam debugPlayer;

public:
	UFUNCTION(BlueprintCallable, Category = "Debug Methods")
	bool GetSinglePlayerDebugMode() const { return singlePlayerDebugMode; }
	UFUNCTION(BlueprintCallable, Category = "Debug Methods")
	bool GetMultiplayerDebugMode() const { return multiplayerDebugMode; }

	UFUNCTION(BlueprintCallable, Category = "Debug Methods")
		FString GetDebugModeStatus() const;
	UFUNCTION(BlueprintCallable, Category = "Debug Methods")
		void SetSinglePlayerDebugMode(bool enabled) { singlePlayerDebugMode = enabled; }
	UFUNCTION(BlueprintCallable, Category = "Debug Methods")
		void SetMultiplayerDebugMode(bool enabled) { multiplayerDebugMode = enabled; }

// managers common
protected:
	void InitializeAllManagers();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Managers")
	UManagerLobby* ManagerLobby;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Managers")
	UManagerAIController* ManagerAIController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Managers")
	UManagerPreview* ManagerPreview;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Managers")
	UManagerMatch* ManagerMatch;

// manager lobby
protected:
	UPROPERTY(BlueprintReadWrite, Category = "GameState Properties")
		TArray<AMPControllerPlayer*> allPlayersControllers;
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		TArray<AMPCharacter*> allPlayerCharacters;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Properties")
		int numOfPlayers;

public:
	UFUNCTION(BlueprintCallable, Category = "Player Methods")
		TArray<AMPControllerPlayer*> GetAllPlayerControllers() const { return allPlayersControllers; }
	
	// Team-specific player lists
	UFUNCTION(BlueprintCallable, Category = "Player Methods")
		TArray<AMPControllerPlayer*> GetHumanPlayers() const;
	UFUNCTION(BlueprintCallable, Category = "Player Methods")
		TArray<AMPControllerPlayer*> GetCatPlayers() const;

	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void RestartLobby();

public:
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		bool AddBot(ETeam team);
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		bool RemoveBot(int32 playerIndex);

public:
	FTimerHandle readyTimerHandle;
	FTimerHandle restartLobbyTimerHandle;

	UFUNCTION(BlueprintCallable, Category = "Lobby Methods")
	void RemoveControlledCharacters(AMPControllerPlayer* aPlayer);

	
// manager preview
protected:
    // Preview system for lobby customization
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview")
    TArray<FVector> characterPreviewLocations; // Set in Blueprint, size 8
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview")
    TArray<FRotator> characterPreviewRotations; // Set in Blueprint, size 8

public:
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
	void StartCustomizeCharacter();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
	void CountdownCustomizeCharacter();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
	void EndCustomizeCharacter();

	// Preview system API now handled by ManagerPreview
    UFUNCTION(BlueprintCallable, Category = "Preview")
    int32 GetPlayerPreviewSlot(AMPControllerPlayer* Player) const;
    UFUNCTION(BlueprintCallable, Category = "Preview")
    void RequestPreviewCharacterUpdate(AMPControllerPlayer* Player, ETeam Team, int CatRace, int HumanProfession, int Hat);

public:
	// Client RPC for countdown updates
	UFUNCTION(Client, Reliable, Category = "Lobby Methods")
	void ClientUpdateCustomizationCountdown(int32 secondsRemaining);

// manager ai controller
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		AMPAISystemManager* theHumanAIManager = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
	int catAIIndex = 1;
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
	int humanAIIndex = 2;
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
	int aiManagerIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		TArray<AMPAIController*> allAICats;
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		TArray<AMPAIController*> allAIHumans;

public:
		// AI list access
	UFUNCTION(BlueprintCallable, Category = "AI Methods")
		TArray<AMPAIController*> GetAllHumanAIs() const { return allAIHumans; }
	UFUNCTION(BlueprintCallable, Category = "AI Methods")
		TArray<AMPAIController*> GetAllCatAIs() const { return allAICats; }

	// manager match
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
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
	
public :
	UFUNCTION(BlueprintCallable, Category = "Factory Methods")
		AMPEnvActorComp* SpawnEnvironment(EEnvActor envTag, FVector spawnLocation, FRotator spawnRotation);
	UFUNCTION(BlueprintCallable, Category = "Factory Methods")
		AMPItem* SpawnItem(EMPItem itemTag, FVector spawnLocation, FRotator spawnRotation);
	UFUNCTION(BlueprintCallable, Category = "Factory Methods")
		AMPAbility* SpawnAbility(AActor* abilityOwner, EAbility abilityTag);

	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void StartGame();

	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		bool CheckIfGameEnd();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void EndGameplayTime();
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void RemoveGameplayHUD();

	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void RegisterPlayerDeath(AMPControllerPlayer* diedPlayer,
			FVector diedPlayerLocation, FRotator diedPlayerRotation);
	
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void DisplayProgressionStatus();
};
