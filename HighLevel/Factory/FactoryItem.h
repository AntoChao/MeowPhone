#pragma once

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "FactoryItem.generated.h"

UCLASS(Blueprintable)
class UFactoryItem : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UFactoryItem();

public :
	virtual AActor* SpawnMPActor(int actorCode, FVector actorLoItemion, FRotator actorRotation) override;
	
};
