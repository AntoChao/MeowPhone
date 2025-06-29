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

	virtual UObject* SpawnMPObject(AActor* owner, int actorCode) override;

// all tsubclassof
protected :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Factory Stats")
		TSubclassOf<AMPAIController> catAIControllerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Factory Stats")
		TSubclassOf<AMPAIController> humanAIControllerClass;
};
