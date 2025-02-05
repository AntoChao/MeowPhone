#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "MPHUDCat.generated.h"

UCLASS()
class UMPHUDCat : public UMPHUD
{
	GENERATED_BODY()

public:
    UMPHUDCat();

protected :
    virtual void NativeConstruct() override;
}