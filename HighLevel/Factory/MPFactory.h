#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "MPFactory.generated.h"

UCLASS(minimalapi)
class MEOWPHONE_API UMPFactory : public UObject
{
	GENERATED_BODY()
	
public:
	UMPFactory();

// common
protected:
	FActorSpawnParameters spawnParams

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Methods")
		UWorld* gameWorld;

public:
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		void SetGameWorld(UWorld* aWorld);

	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual AActor* SpawnMPActor(int actorCode, FVector actorLocation, FRotator actorRotation);

	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual UObject* SpawnMPObject(AActor* owner, int actorCode);
	
};
