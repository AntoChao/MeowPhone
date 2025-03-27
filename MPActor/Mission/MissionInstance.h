#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissionInstance.generated.h"

struct FMissionDetails;

UCLASS()
class AMissionInstance : public AActor
{
    GENERATED_BODY()

    
protected:
    virtual void BeginPlay() override;
    
    virtual void Tick(float DeltaSeconds) override;

public:
    AMissionInstance();

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mission")
   FMissionDetails MissionDefinition;

   UFUNCTION(BlueprintCallable, Category="Mission")
   void InitializeMission(const FMissionDetails& InMissionDefinition);

};