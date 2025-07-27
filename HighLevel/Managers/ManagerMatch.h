#pragma once

#include "CoreMinimal.h"
#include "ManagerMP.h"
#include "ManagerMatch.generated.h"

class AMPGMGameplay;
class AMPControllerPlayer;

UCLASS()
class UManagerMatch : public UManagerMP
{
    GENERATED_BODY()

public:
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