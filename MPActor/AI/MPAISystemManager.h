#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MPAISystemManager.generated.h"

class AMPAIControllerHuman;
class AEnvActorComp;

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
        TArray<AMPAIControllerHuman*> allAIHumanControllers;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI Manager")
        TArray<AEnvActorComp*> allUrgentEnvActor;
    
    // urgent event
public :
    UFUNCTION(BlueprintCallable, Category="AI Manager")
        void ReceiveUrgentNotification(AEnvActorComp* eventActor);

protected :
    UFUNCTION(BlueprintCallable, Category="AI Manager")
        void AllocateUrgent();
};