#pragma once

// [Meow-Phone Project]
//
// This is a Blueprint Function Library that provides a convenient, globally-accessible
// function for reporting gameplay noises to the AI system.
//
// How to utilize in Blueprint:
// 1. Because this is a Blueprint Function Library, its functions can be called from any Blueprint graph without needing an instance of the class.
// 2. When a gameplay event occurs that should attract the attention of AI (e.g., an explosion, a falling object landing), call the `ReportAIGameplayNoise` function.
// 3. You must provide a `WorldContextObject` (which can usually be a `self` reference), the `Location` of the sound, and you can optionally specify the `Loudness` and the `Instigator` actor that caused the sound.
//
// Necessary things to define:
// - Nothing needs to be defined for this class. It is a stateless collection of utility functions.
//
// How it interacts with other classes:
// - UBlueprintFunctionLibrary: The base class that allows its static functions to be exposed to all Blueprints.
// - UAISense_Hearing: The `ReportAIGameplayNoise` function's primary purpose is to call `UAISense_Hearing::ReportNoiseEvent`. This makes the sound perceivable by any AI in the world that has a hearing component in its AIPerceptionComponent.
// - Any Actor: Any actor in the world can call this function to generate a sound event for the AI system.

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