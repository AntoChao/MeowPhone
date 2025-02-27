#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDMenu.generated.h"

UCLASS()
class UHUDMenu : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDMenu();

protected:
    virtual void NativeConstruct() override;
};