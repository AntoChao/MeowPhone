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
class MEOWPHONE_API AMPPlayerState : public APlayerController
{
    GENERATED_BODY()

public :
    AMPPlayerState();

// Common playerstate
public :
    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        FString playerName;

    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        int playerIndex;
    
    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        ETeam playerTeam;
    
    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        ECatRace catRace;

    UPROPERTY(BlueprintReadWrite, Category = "Ability Properties")
        EAbility playerSelectedAbility;
    
    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        EHumanProfession humanProfession;

    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        bool isReady = false;
    
    UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
        bool isDied = false;
}