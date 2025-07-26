#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MatchManager.generated.h"

class AMPGMGameplay;
class AMPControllerPlayer;

UCLASS()
class UMatchManager : public UObject
{
    GENERATED_BODY()

public:
    void Initialize(AMPGMGameplay* InGameMode);

    // Character Customization
    void StartCustomizeCharacter();
    void CountdownCustomizeCharacter();
    void EndCustomizeCharacter();

    // Game Flow
    void StartGame();
    void SetupGame();
    void StartPrepareTime();
    void CountdownPrepareGame();
    void EndPrepareTime();
    void StartGameplayTime();
    void CountdownGameplayGame();
    bool CheckIfGameEnd();
    void EndGameplayTime();

    void RegisterPlayerDeath(AMPControllerPlayer* diedPlayer, FVector diedPlayerLocation, FRotator diedPlayerRotation);
    void DisplayProgressionStatus();

protected:
    UPROPERTY()
    AMPGMGameplay* GameMode;

    FTimerHandle customCharacterTimerHandle;
    FTimerHandle prepareTimerHandle;
    FTimerHandle gameplayTimerHandle;

    void SetupMap();
    void SetupMapItems();
    void SetupMapEnvActors();
    void SetupPlayers();
    void SetupAIs();
    void SetupAICats();
    void SetupAIHumans();
    void SetupAIManager();

    void RemoveGameplayHUD();

}; 