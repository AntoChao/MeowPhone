#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SoundEventUtility.generated.h"

UCLASS()
class USoundEventUtility : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="AI|Sound", meta=(WorldContext="WorldContextObject"))
    static void ReportAIGameplayNoise(UObject* WorldContextObject, FVector Location, float Loudness = 1.0f, AActor* Instigator = nullptr);
}; 