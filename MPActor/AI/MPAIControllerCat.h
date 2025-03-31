#pragma once

#include "CoreMinimal.h"
#include "MPAIController.h"
#include "MPAIControllerCat.generated.h"

class AMPEnvActorComp;

UCLASS(BlueprintType, Blueprintable)
class AMPAIControllerCat : public AMPAIController
{
    GENERATED_BODY()

    // common functions
public:
    AMPAIControllerCat();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

// specific cat ai
protected:
    // Blackboard key names (must match your Blackboard asset)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
        FName BB_WantToInteract = TEXT("WantToInteract");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
        FName BB_MovingLocation = TEXT("MovingLocation");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
        FName BB_EnvActorToInteract = TEXT("EnvActorToInteract");

    // normal move 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat AI")
    float MoveLocationRadius = 1000.0f; // e.g., every 5 seconds
    
    UFUNCTION(BlueprintCallable, Category = "Cat AI")
    void UpdateWanderLocation();

    // find envactor to interact
    float InteractionTimer = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat AI")
    float InteractionInterval = 5.0f; // e.g., every 5 seconds
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat AI")
    AMPEnvActorComp* ActorToInteract = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Cat AI")
    void CheckForEnvActorInteraction();

};