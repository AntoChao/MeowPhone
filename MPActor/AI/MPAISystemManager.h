#pragma once

// [Meow-Phone Project]
//
// This is a high-level singleton-like actor that manages the overall AI ecosystem.
// It keeps track of all active AI Human controllers and is aware of special "urgent"
// environmental actors. Its primary role is to receive notifications about urgent events
// and delegate those tasks to an available AI.
//
// How to utilize in Blueprint:
// 1. There should be exactly ONE instance of this actor (or a Blueprint derived from it) placed in the gameplay level.
// 2. The `Initialize` function is likely called by the Game Mode (`AMPGMGameplay`) at the start of the match.
// 3. This manager works in the background. Its main interaction point is the `ReceiveUrgentNotification` function. Other actors or systems (e.g., an environmental actor that just caught on fire) would get a reference to this manager and call this function to request AI attention.
// 4. In a Blueprint derived from this class, you could add logic to further customize how AIs are selected for tasks.
//
// Necessary things to define:
// - A single instance of a Blueprint derived from this class must be placed in the level for it to function.
//
// How it interacts with other classes:
// - AActor: It is an actor that exists in the level, making it easy for other actors to find and reference.
// - AMPAIControllerHumanPlayer: It maintains a list of all human AI controllers in the game, which it populates by searching the level in `LocateAIHumans`.
// - AMPEnvActorComp: It specifically tracks environmental actors that are marked as "urgent". When one of these actors fires a notification, the manager's `AllocateUrgent` function finds a free AI from its list and assigns the task to it.
// - UManagerAIController (in `HighLevel/Managers`): There is a tight coupling here. The `UManagerAIController` is responsible for spawning the AI controllers, and this `AMPAISystemManager` is responsible for finding them in the level and giving them high-level tasks. The `UManagerAIController` likely holds a reference to this actor.

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPAISystemManager.generated.h"

class AMPAIControllerHumanPlayer;
class AMPEnvActorComp;

UCLASS(Blueprintable)
class AMPAISystemManager : public AActor
{
    GENERATED_BODY()
    
public:
    AMPAISystemManager();

public:
    UFUNCTION(BlueprintCallable, Category="AI Manager")
    void Initialize();

    UFUNCTION(BlueprintCallable, Category="AI Manager")
    void LocateAIHumans();

    UFUNCTION(BlueprintCallable, Category="AI Manager")
    void LocateUrgentEnvActors();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI Manager")
        TArray<AMPAIControllerHumanPlayer*> allAIHumanControllers;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI Manager")
        TArray<AMPEnvActorComp*> allUrgentEnvActor;
    
    // urgent event
public :
    UFUNCTION(BlueprintCallable, Category="AI Manager")
        void ReceiveUrgentNotification(AMPEnvActorComp* eventActor);

protected :
    UFUNCTION(BlueprintCallable, Category="AI Manager")
        void AllocateUrgent();
};