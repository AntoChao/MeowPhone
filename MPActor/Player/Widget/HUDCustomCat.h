#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDCustomCat.generated.h"

UCLASS()
class UHUDCustomCat : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDCustomCat();

protected:
    virtual void NativeConstruct() override;
};