#include "MissionManager.h"
#include "Kismet/GameplayStatics.h"

#include "MissionZone.h"
#include "MissionInstance.h"
#include "../../CommonStruct.h"

AMissionManager::AMissionManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMissionManager::BeginPlay()
{
    Super::BeginPlay();
}

/*
Player will select the mission via HUD and pass mission manager the code (0, 1, 2, 3)
*/
void AMissionManager::StartMission(int nextMissionID)
{
    // find the correponding mission details 
    FMissionDetails selectedMissionDetails = nullptr;

    for (FMissionDetails eachMissionDefinition : missionDetails)
    {
        if (eachMissionDefinition->missionID == nextMissionID)
        {
            selectedMissionDetails = eachMissionDefinition;
            break;
        }
    }

    if (selectedMissionDetails)
    {
        if (AMissionZone* selectedMissionZone = selectedMissionZone->missionZone)
        {
            // Spawn the mission instance at the mission zone's location
            FActorSpawnParameters SpawnParams;
            FVector SpawnLocation = selectedMissionZone->GetActorLocation();
            FRotator SpawnRotation = selectedMissionZone->GetActorRotation();

            currentActiveMission = GetWorld()->SpawnActor<AMissionInstance>(AMissionInstance::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
            if (currentActiveMission)
            {
                currentActiveMission->InitializeMission(selectedMissionDetails);
            }
        }
    }
}