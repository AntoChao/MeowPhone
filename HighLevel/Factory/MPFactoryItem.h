#pragma once

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "MPFactoryItem.generated.h"

UCLASS(minimalapi)
class MEOWPHONE_API UMPFactoryItem : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UMPFactoryItem();

public :
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual AActor* SpawnMPActor(int actorCode, FVector actorLoItemion, FRotator actorRotation) override;

	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual UObject* SpawnMPObject(AActor* owner, int actorCode) override;
	
};
