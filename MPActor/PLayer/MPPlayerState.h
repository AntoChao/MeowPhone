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

    // Common playerstate
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    FString playerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    int playerIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    ETeam playerTeam;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    ECatRace catRace;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability Properties")
    EAbility playerSelectedAbility;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    EHumanProfession humanProfession;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    bool isPlayerReady = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
    bool isDied = false;
};