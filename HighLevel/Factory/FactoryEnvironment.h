#pragma once

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "FactoryEnvironment.generated.h"

UCLASS(Blueprintable)
class UFactoryEnvironment : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UFactoryEnvironment();

public :
	virtual AActor* SpawnMPActor(int actorCode, FVector actorLoEnvironmention, FRotator actorRotation) override;
	
};
