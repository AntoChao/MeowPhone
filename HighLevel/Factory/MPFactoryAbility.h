#pragma once

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "MPFactoryAbility.generated.h"

UCLASS(minimalapi)
class MEOWPHONE_API UMPFactoryAbility : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UMPFactoryAbility();

public :
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual AActor* SpawnMPActor(int actorCode, FVector actorLocation, FRotator actorRotation) override;

	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual UObject* SpawnMPObject(AActor* owner, int actorCode) override;
	
};
