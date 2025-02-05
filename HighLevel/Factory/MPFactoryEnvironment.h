#pragma once

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "MPFactoryEnvironment.generated.h"

UCLASS(minimalapi)
class MEOWPHONE_API UMPFactoryEnvironment : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UMPFactoryEnvironment();

public :
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual AActor* SpawnMPActor(int actorCode, FVector actorLoEnvironmention, FRotator actorRotation) override;

	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual UObject* SpawnMPObject(AActor* owner, int actorCode) override;
	
};
