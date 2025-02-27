#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDEnd.generated.h"

UCLASS()
class UHUDEnd : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDEnd();

protected:
    virtual void NativeConstruct() override;
};