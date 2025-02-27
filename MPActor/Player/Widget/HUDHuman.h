#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDHuman.generated.h"

UCLASS()
class UHUDHuman : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDHuman();

protected:
    virtual void NativeConstruct() override;
};