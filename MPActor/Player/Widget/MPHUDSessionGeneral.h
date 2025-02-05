#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "MPHUDSessionGeneral.generated.h"

UCLASS()
class UMPHUDSessionGeneral : public UMPHUD
{
	GENERATED_BODY()

public:
    UMPHUDSessionGeneral();

protected :
    virtual void NativeConstruct() override;
}