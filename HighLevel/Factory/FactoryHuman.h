#pragma once

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "FactoryHuman.generated.h"

UCLASS(Blueprintable)
class UFactoryHuman : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UFactoryHuman();

public :
	virtual AActor* SpawnMPActor(int actorCode, FVector actorLocation, FRotator actorRotation) override;
	
};
