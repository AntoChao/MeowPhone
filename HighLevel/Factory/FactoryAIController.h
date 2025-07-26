#pragma once

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "FactoryAIController.generated.h"

class AMPAIController;

UCLASS(Blueprintable)
class UFactoryAIController : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UFactoryAIController();

public :
	virtual AActor* SpawnMPActor(int actorCode, FVector actorLoItemion, FRotator actorRotation) override;

	// Spawn AI controller by team
	UFUNCTION(BlueprintCallable, Category = "AI Controller Factory")
	AMPAIController* SpawnAIController(ETeam team);

// all tsubclassof
protected :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Factory Stats")
		TSubclassOf<AMPAIController> catAIControllerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Factory Stats")
		TSubclassOf<AMPAIController> humanAIControllerClass;
};
