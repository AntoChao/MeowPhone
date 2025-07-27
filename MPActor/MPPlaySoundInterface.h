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
    virtual void PlaySoundLocally(USoundCue* aSound) = 0;

    virtual void PlaySoundBroadcast(USoundCue* aSound) = 0;
};
