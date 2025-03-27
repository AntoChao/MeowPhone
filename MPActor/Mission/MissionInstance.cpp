#include "MissionInstance.h"
#include "MissionZone.h"
#include "../../CommonStruct.h"

AMissionInstance::AMissionInstance()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMissionInstance::BeginPlay()
{
    Super::BeginPlay();
    
}

void AMissionInstance::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AMissionInstance::InitializeMission(const FMissionDetails& InMissionDefinition)
{
    MissionDefinition = InMissionDefinition;
}