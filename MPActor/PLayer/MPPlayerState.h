#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "../../CommonEnum.h"
#include "MPPlayerState.generated.h"

/*
enum class ETeam : uint8;
enum class ECatRace : uint8;
enum class EAbility : uint8;
enum class EHumanProfession : uint8;*/

UCLASS(BlueprintType, Blueprintable)
class AMPPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    AMPPlayerState();

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Common playerstate
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Common Properties")
    FString playerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Common Properties")
    int playerIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_PlayerTeam, Category = "Common Properties")
    ETeam playerTeam;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Common Properties")
    ECatRace catRace;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Ability Properties")
    EAbility playerSelectedAbility;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Common Properties")
    EHumanProfession humanProfession;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Common Properties")
    EHat playerSelectedHat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_IsPlayerReady, Category = "Common Properties")
    bool isPlayerReady = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_IsPlayerDied, Category = "Common Properties")
    bool isPlayerDied = false;

protected:
    // Replication callbacks
    UFUNCTION()
    void OnRep_PlayerTeam();
    
    UFUNCTION()
    void OnRep_IsPlayerReady();
    
    UFUNCTION()
    void OnRep_IsPlayerDied();
};