#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDCredit.generated.h"

UCLASS()
class UHUDCredit : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDCredit();

protected:
    virtual void NativeConstruct() override;
};