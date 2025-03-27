#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissionZone.generated.h"

class AMPEnvActorComp;

UCLASS()
class AMissionZone : public AActor
{
    GENERATED_BODY()

    AMissionZone();

    // basic information
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    int missionID = 0; // no need as right now mission zone is directly refered by fMissionDetails

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    TArray<FVector> playSpawnPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    TArray<FVector> aiSpawnPoints;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    int32 numAIsToSpawn;
    
// set up:
public:
    UFUNCTION(BlueprintCallable, Category="Mission")
    void RandomlizeZone();
};