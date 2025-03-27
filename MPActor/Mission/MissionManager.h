#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissionManager.generated.h"

struct FMissionDetails;
class AMissionInstance;
class AMissionZone;

UCLASS()
class AMissionManager : public AActor
{
    GENERATED_BODY()

public:
    AMissionManager();

protected:
    virtual void BeginPlay() override;

    // mission manager common
protected:
    // all mission definitions should be define in blueprint and assign to missionManager
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
    TArray<FMissionDetails> missionDetails;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mission")
    AMissionInstance* currentActiveMission;

public:
    UFUNCTION(BlueprintCallable, Category="Mission")
    void StartMission(int missionCode);
};
