#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDCreateSession.generated.h"

UCLASS()
class UHUDCreateSession : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDCreateSession();

protected:
    virtual void NativeConstruct() override;
};