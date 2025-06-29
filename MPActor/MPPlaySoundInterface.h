#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MPPlaySoundInterface.generated.h"

class USoundCue;

UINTERFACE(MinimalAPI, Meta = (CannotImplementInterfaceInBlueprint))
class UMPPlaySoundInterface : public UInterface
{
    GENERATED_BODY()
};

class IMPPlaySoundInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual void PlaySoundLocally(USoundCue* aSound) = 0;

    UFUNCTION(BlueprintCallable, Category = "Interface Method")
        virtual void PlaySoundBroadcast(USoundCue* aSound) = 0;
};
