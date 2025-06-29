#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDCustomHuman.generated.h"

UCLASS()
class UHUDCustomHuman : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDCustomHuman();

protected:
    virtual void NativeConstruct() override;
};