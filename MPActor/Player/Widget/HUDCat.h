#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDCat.generated.h"

UCLASS()
class UHUDCat : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDCat();

protected:
    virtual void NativeConstruct() override;
};