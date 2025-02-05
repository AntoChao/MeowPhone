#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "MPHUDEnd.generated.h"

UCLASS()
class UMPHUDEnd : public UMPHUD
{
	GENERATED_BODY()

public:
    UMPHUDEnd();

protected :
    virtual void NativeConstruct() override;
}