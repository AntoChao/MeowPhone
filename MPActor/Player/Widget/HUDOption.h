#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDOption.generated.h"

UCLASS()
class UHUDOption : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDOption();

protected:
    virtual void NativeConstruct() override;
};