#pragma once

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "FactoryAbility.generated.h"

UCLASS(Blueprintable)
class UFactoryAbility : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UFactoryAbility();

public :
	virtual AActor* SpawnMPActor(int actorCode, FVector actorLocation, FRotator actorRotation) override;
	
};
