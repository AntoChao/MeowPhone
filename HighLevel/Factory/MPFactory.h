#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "MPFactory.generated.h"

class UWorld;

UCLASS(Blueprintable)
class UMPFactory : public UObject
{
	GENERATED_BODY()
	
public:
	UMPFactory();

// common
protected:
	FActorSpawnParameters spawnParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Property")
		UWorld* gameWorld;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Property")
		AActor* spawnedActor = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Property")
		UObject* spawnedObject = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		void SetGameWorld(UWorld* aWorld);

	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual AActor* SpawnMPActor(int actorCode, FVector actorLocation, FRotator actorRotation);

	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual UObject* SpawnMPObject(AActor* owner, int actorCode);
	
};
