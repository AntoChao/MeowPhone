#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDSessionGeneral.generated.h"

UCLASS()
class UHUDSessionGeneral : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDSessionGeneral();

protected:
    virtual void NativeConstruct() override;
};