#pragma once

#include "CoreMinimal.h"
#include "MPItem.h"

#include "MPItemMeowPhone.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMPItemMeowPhone : public AMPItem
{
    GENERATED_BODY()

public :
    AMPItemMeowPhone();

protected :
    UPROPERTY(BlueprintReadWrite, Category = "Usage Properties")
    float theMPModifierNum = 0.1f;

    virtual void ApplyUsageEffectDirect(AActor* targetActor);
};