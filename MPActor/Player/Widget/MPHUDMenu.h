#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "MPHUDMenu.generated.h"

UCLASS()
class UMPHUDMenu : public UMPHUD
{
	GENERATED_BODY()

public:
    UMPHUDMenu();

protected :
    virtual void NativeConstruct() override;
}