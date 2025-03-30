#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPAISystemManager.generated.h"

class AMPCharacter;
class AMissionZone;

UCLASS(Blueprintable)
class AMPAISystemManager : public AActor
{
    GENERATED_BODY()
    
public:
    AMPAISystemManager();

    // Initializes the AI system for the mission zone.
    UFUNCTION(BlueprintCallable, Category="AI")
    void InitializeAI();

    // Example function to allocate urgent logic across all spawned AI.
    UFUNCTION(BlueprintCallable, Category="AI")
    void AllocateUrgentLogic();

protected:
    // Array to store references to spawned AI characters.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
    TArray<AMPCharacter*> SpawnedAICharacters;
};