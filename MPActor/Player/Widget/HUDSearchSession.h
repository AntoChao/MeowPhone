#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDSearchSession.generated.h"

UCLASS()
class UHUDSearchSession : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDSearchSession();

protected:
    virtual void NativeConstruct() override;
};