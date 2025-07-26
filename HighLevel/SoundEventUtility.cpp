#include "SoundEventUtility.h"
#include "Perception/AISense_Hearing.h"
#include "Kismet/GameplayStatics.h"

void USoundEventUtility::ReportAIGameplayNoise(UObject* WorldContextObject, FVector Location, float Loudness, AActor* Instigator)
{
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
    if (!Instigator)
    {
        Instigator = UGameplayStatics::GetPlayerPawn(World, 0);
    }
    UAISense_Hearing::ReportNoiseEvent(World, Location, Loudness, Instigator, 0.f, TEXT("GameplayNoise"));
} 