#pragma once

#include "CoreMinimal.h"
#include "ManagerMP.h"
#include "../../CommonEnum.h"
#include "ManagerAIController.generated.h"

class AMPAIController;
class AMPGMGameplay;

UCLASS(Blueprintable)
class UManagerAIController : public UManagerMP
{
    GENERATED_BODY()

public:
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
    void SetNumAICats(int aiCatNum);
    UFUNCTION(BlueprintCallable, Category = "Setter && Getter")
    void SetNumAIHumans(int aiHumanNum);

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
}; 