#pragma once

// [Meow-Phone Project]
//
// This manager is the central authority for all AI Controllers in the game. It is responsible
// for creating, tracking, and assigning high-level tasks to AI-controlled characters for both
// the Cat and Human teams.
//
// How to utilize in Blueprint:
// 1. This manager is typically created and owned by the Game Mode (`AMPGMGameplay`). It should not be placed in the level directly.
// 2. From a lobby or settings menu, you would get the instance of this manager from the Game Mode and call `SetNumAICats` and `SetNumAIHumans` to configure the number of bots for the upcoming match.
// 3. The Game Mode is responsible for calling `SetupAIManager` when the match is being initialized.
// 4. When the match starts, the Game Mode calls `SpawnLobbyAIs` to create the actual pawns and controllers for the configured number of bots.
// 5. During gameplay, other systems can add global tasks. For example, if a loud noise happens, that system can call `AddGlobalSearchTask` on this manager to have an AI investigate the location.
//
// Necessary things to define:
// - This manager's properties are configured at runtime, primarily through the `SetNumAI...` functions. It does not require pre-set assets in the Blueprint editor.
// - It depends on an `AMPAISystemManager` actor being present in the level. The `SetupAIManager` function is responsible for finding and linking to this actor.
//
// How it interacts with other classes:
// - UManagerMP: Inherits from the base manager class, likely for inclusion in a centralized manager system.
// - AMPGMGameplay: The Game Mode owns, configures, and drives this manager's major functions (`Setup`, `SpawnLobbyAIs`).
// - AMPAIController: This manager directly holds references to all active AI controllers, separating them by team.
// - AMPAISystemManager: It holds a reference to this actor to interface with lower-level, persistent AI systems (like EQS queries, perception data, etc.).
// - UFactoryAIController (Inferred from functionality): This manager almost certainly uses an AI Controller Factory in its C++ implementation to spawn the controller instances when `AddBot` or `SpawnLobbyAIs` is called.
// - Lobby UI: The UI calls functions on this manager to update the number of bots required for a match.

#include "CoreMinimal.h"
#include "ManagerMP.h"
#include "../../CommonEnum.h"
#include "ManagerAIController.generated.h"

class AMPAIController;
class AMPGMGameplay;
class AMPAISystemManager;

UCLASS(Blueprintable)
class UManagerAIController : public UManagerMP
{
    GENERATED_BODY()

protected:
    UPROPERTY()
    TArray<TObjectPtr<AMPAIController>> AllAIHumans;

    UPROPERTY()
    TArray<TObjectPtr<AMPAIController>> AllAICats;

    struct FGlobalAITask
    {
        FVector Location;
        bool bAssigned = false;
    };

    TArray<FGlobalAITask> PendingTasks;

    UPROPERTY()
	int aiCatNumber = 0;

    UPROPERTY()
    int aiHumanNumber = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Manager")
    AMPAISystemManager* AISystemManager;

public:
    // Initialize the AI System Manager
    UFUNCTION(BlueprintCallable, Category="AI Manager")
    void SetupAIManager();

    // Add or remove a bot (either human or cat). Returns true if successful.
    UFUNCTION(BlueprintCallable, Category="AI Manager")
    bool AddBot(ETeam team);

    UFUNCTION(BlueprintCallable, Category="AI Manager")
    bool RemoveBot(int32 playerIndex);

    // Spawn pawns and possess based on current controllers - called when match starts
    void SpawnLobbyAIs();

    // --- Global task handling ---
    UFUNCTION(BlueprintCallable, Category="AI Manager")
    void AddGlobalSearchTask(const FVector& NoiseLocation);

    // Iterate over pending tasks and assign to available AI
    void AssignGlobalTasks();

    // Accessors
    const TArray<AMPAIController*>& GetAllAIHumans() const { return AllAIHumans; }
    const TArray<AMPAIController*>& GetAllAICats() const { return AllAICats; }

    UFUNCTION(BlueprintCallable, Category = "Setter && Getter")
    AMPAISystemManager* GetAISystemManager() const { return AISystemManager; }

    UFUNCTION(BlueprintCallable, Category = "Setter && Getter")
    void SetNumAICats(int aiCatNum);
    UFUNCTION(BlueprintCallable, Category = "Setter && Getter")
    void SetNumAIHumans(int aiHumanNum);
}; 