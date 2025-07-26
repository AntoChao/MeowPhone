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

class ULobbyManager;
class UPreviewManager;
class UMatchManager;
class UAIControllerManager;

// Old deprecated enum names removed. Use correct ones below.
// Correct forward declarations for enums used in this header
enum class EEnvActor : uint8;
enum class EItem : uint8;
enum class EAbility : uint8;
enum class ETeam : uint8;

UCLASS()
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

	UPROPERTY(BlueprintReadWrite, Category = "GameState Properties")
		TArray<AMPControllerPlayer*> allPlayersControllers;
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		TArray<AMPCharacter*> allPlayerCharacters;
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
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		int catAIIndex = 1;
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		int humanAIIndex = 2;
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		int aiManagerIndex = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		AMPAISystemManager* theHumanAIManager = nullptr;


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
	UFUNCTION(BlueprintCallable, Category = "Lobby Methods")
		void RemoveControlledCharacters(AMPControllerPlayer* aPlayer);
	
// gameplay custom character 
protected:

// gameplay
	// gameplay Setup
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		TArray<AMPAIController*> allAICats;
	UPROPERTY(BlueprintReadWrite, Category = "Setup Properties")
		TArray<AMPAIController*> allAIHumans;
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
	FTimerHandle restartLobbyTimerHandle;

protected:
	// Debug mode settings - can be changed in Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Settings")
		bool singlePlayerDebugMode = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Settings")
		bool multiplayerDebugMode = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Settings")
		ETeam debugPlayer = ETeam::EHuman;

public:
	// All lobby, team, and bot methods are now in their respective managers
	
	// Client RPC for countdown updates
	UFUNCTION(Client, Reliable, Category = "Lobby Methods")
		void ClientUpdateReadyCountdown(int32 secondsRemaining);
	
	UFUNCTION(Client, Reliable, Category = "Lobby Methods")
		void ClientUpdateCustomizationCountdown(int32 secondsRemaining);
	
	// Player list access
	UFUNCTION(BlueprintCallable, Category = "Player Methods")
		TArray<AMPControllerPlayer*> GetAllPlayerControllers() const { return allPlayersControllers; }
	
	// Team-specific player lists
	UFUNCTION(BlueprintCallable, Category = "Player Methods")
		TArray<AMPControllerPlayer*> GetHumanPlayers() const;
	UFUNCTION(BlueprintCallable, Category = "Player Methods")
		TArray<AMPControllerPlayer*> GetCatPlayers() const;
	
	// AI list access
	UFUNCTION(BlueprintCallable, Category = "AI Methods")
		TArray<AMPAIController*> GetAllHumanAIs() const { return allAIHumans; }
	UFUNCTION(BlueprintCallable, Category = "AI Methods")
		TArray<AMPAIController*> GetAllCatAIs() const { return allAICats; }
	
	// Team-specific AI lists
	UFUNCTION(BlueprintCallable, Category = "AI Methods")
		TArray<AMPAIController*> GetHumanAIs() const { return allAIHumans; }
	UFUNCTION(BlueprintCallable, Category = "AI Methods")
		TArray<AMPAIController*> GetCatAIs() const { return allAICats; }

	// Game restart methods
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void RestartLobby();
	
		// start game
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void StartGame();

protected :
	// All game setup and progress methods are now in MatchManager
	
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
	
	UFUNCTION(BlueprintCallable, Category = "GameProgress Methods")
		void DisplayProgressionStatus();
	
// setters and getters
public :
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Managers")
    ULobbyManager* LobbyManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Managers")
    UAIControllerManager* AIControllerManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Managers")
    UPreviewManager* PreviewManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Managers")
	UMatchManager* MatchManager;
	
	UFUNCTION(BlueprintCallable, Category = "Setter && Getter")
		void SetNumAICats(int aiCatNum);
	UFUNCTION(BlueprintCallable, Category = "Setter && Getter")
		void SetNumAIHumans(int aiHumanNum);

	// Debug mode helpers
	UFUNCTION(BlueprintCallable, Category = "Debug Methods")
		bool IsSinglePlayerDebugMode() const { return singlePlayerDebugMode; }
	UFUNCTION(BlueprintCallable, Category = "Debug Methods")
		bool IsMultiplayerDebugMode() const { return multiplayerDebugMode; }
	UFUNCTION(BlueprintCallable, Category = "Debug Methods")
		FString GetDebugModeStatus() const;
	UFUNCTION(BlueprintCallable, Category = "Debug Methods")
		void SetSinglePlayerDebugMode(bool enabled) { singlePlayerDebugMode = enabled; }
	UFUNCTION(BlueprintCallable, Category = "Debug Methods")
		void SetMultiplayerDebugMode(bool enabled) { multiplayerDebugMode = enabled; }

public:
    // Preview system API now handled by PreviewManager
    UFUNCTION(BlueprintCallable, Category = "Preview")
    int32 GetPlayerPreviewSlot(AMPControllerPlayer* Player) const;
    UFUNCTION(BlueprintCallable, Category = "Preview")
    void RequestPreviewCharacterUpdate(AMPControllerPlayer* Player, ETeam Team, int CatRace, int HumanProfession, int Hat);

protected:
    // Preview system for lobby customization
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview")
    TArray<FVector> characterPreviewLocations; // Set in Blueprint, size 8
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preview")
    TArray<FRotator> characterPreviewRotations; // Set in Blueprint, size 8

    // Preview management is now in PreviewManager
};
