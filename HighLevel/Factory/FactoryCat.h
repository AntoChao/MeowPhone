#pragma once

#include "CoreMinimal.h"
#include "MPFactory.h"
#include "FactoryCat.generated.h"

class AMPCharacterCat;

UCLASS(Blueprintable)
class UFactoryCat : public UMPFactory
{
	GENERATED_BODY()
	
public:
	UFactoryCat();

public :
	virtual AActor* SpawnMPActor(int actorCode, FVector actorLocation, FRotator actorRotation) override;
	
	virtual UObject* SpawnMPObject(AActor* owner, int actorCode) override;
	
// all tsubclassof
protected :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Factory Stats")
		TSubclassOf<AMPCharacterCat> catEXPClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cat Factory Stats")
		TSubclassOf<AMPCharacterCat> catDiedClass;
};
