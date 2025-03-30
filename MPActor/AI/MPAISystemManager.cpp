#include "MPAISystemManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

AMPAISystemManager::AMPAISystemManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AMPAISystemManager::InitializeAI()
{
    if (!GetWorld())
    {
        UE_LOG(LogTemp, Warning, TEXT("MPAISystemManager::InitializeAI - World is null"));
        return;
    }

    /* ai controller generate through game mode 
    // Loop through the number of AI to spawn.
    for (int32 i = 0; i < MissionZoneInfo.AINums; i++)
    {
        // Choose an AI class using modulo in case there are multiple classes.
        int32 ClassIndex = i % MissionZoneInfo.AICharacterClasses.Num();
        TSubclassOf<AMPCharacter> AIClassToSpawn = MissionZoneInfo.AICharacterClasses[ClassIndex];
        if (!*AIClassToSpawn)
        {
            UE_LOG(LogTemp, Warning, TEXT("MPAISystemManager::InitializeAI - Invalid AI class at index %d"), ClassIndex);
            continue;
        }

        // Choose a spawn location (cycle through the provided locations).
        int32 SpawnLocationIndex = i % MissionZoneInfo.SpawnLocations.Num();
        FVector SpawnLocation = MissionZoneInfo.SpawnLocations[SpawnLocationIndex];

        // Set a default rotation (can be adjusted as needed).
        FRotator SpawnRotation = FRotator::ZeroRotator;

        // Spawn the AI character.
        AMPCharacter* SpawnedCharacter = GetWorld()->SpawnActor<AMPCharacter>(AIClassToSpawn, SpawnLocation, SpawnRotation);
        if (SpawnedCharacter)
        {
            // Optionally assign patrol points if your AI character supports it.
            // e.g., SpawnedCharacter->SetPatrolPoints(MissionZoneInfo.PatrolPoints);
            
            SpawnedAICharacters.Add(SpawnedCharacter);
            UE_LOG(LogTemp, Log, TEXT("MPAISystemManager::InitializeAI - Spawned AI at %s"), *SpawnLocation.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("MPAISystemManager::InitializeAI - Failed to spawn AI character"));
        }
    }*/
}

void AMPAISystemManager::AllocateUrgentLogic()
{
    // Example: Iterate over all spawned AI characters and trigger an urgent behavior.
    for (AMPCharacter* AICharacter : SpawnedAICharacters)
    {
        if (AICharacter)
        {
            UE_LOG(LogTemp, Log, TEXT("MPAISystemManager::AllocateUrgentLogic - Allocating urgent logic to AI: %s"), *AICharacter->GetName());
        }
    }
}