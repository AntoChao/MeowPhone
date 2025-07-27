#pragma once

// [Meow-Phone Project]
//
// This is the Game State class. The Game State is a server-authoritative actor that is
// replicated to every connected client. Its primary purpose is to hold and synchronize game-wide
// state variables that all players need to be aware of. This is the central repository for
// "facts" about the current match.
//
// How to utilize in Blueprint:
// 1. From any Blueprint, you can get a reference to the Game State using the "Get Game State" node and casting it to `BP_MPGS`. This is a safe and common operation on clients.
// 2. UI elements (like the main game HUD) are the primary consumers of this class. They bind to the replicated variables to display up-to-date information. For example:
//    - A timer on the HUD would display the value of `curGameplayTime`.
//    - A progress bar for the Cat team would use `curMPProgressionPercentage`.
//    - A progress bar for the Human team would use `caughtCatsPercentage`.
// 3. Since this data is replicated, clients do not need to ask the server for it via RPCs. They can simply read the properties from their local copy of the Game State.
// 4. Writing to these variables should only be done on the server, which is typically handled by the Game Mode (`AMPGMGameplay`).
//
// Necessary things to define:
// - All properties in this class are set at runtime by the server (via the Game Mode). Nothing needs to be configured in its Blueprint editor, but a Blueprint should be created (`BP_MPGS`) and set as the `GameState Class` in the `BP_MPGMGameplay` Game Mode.
//
// How it interacts with other classes:
// - AGameStateBase: The standard Unreal class it inherits from.
// - AMPGMGameplay (Game Mode): The Game Mode is the "owner" and "writer" of the Game State. On the server, the Game Mode calculates game progress and timers and updates the properties on the Game State. The engine's networking system then automatically replicates these changes to all clients.
// - OnRep_... functions: These are RepNotify functions. When a client receives an update for a variable marked with `ReplicatedUsing`, the corresponding `OnRep_` function is automatically called. This is extremely useful for triggering UI updates or sound effects on the client precisely when the data changes. For example, `OnRep_CurMPProgression` could trigger a sound effect indicating progress was made.
// - HUDs: The UI reads data from the Game State to display the status of the match to the player.

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
