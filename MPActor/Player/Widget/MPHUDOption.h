#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "MPHUDOption.generated.h"

UCLASS()
class UMPHUDOption : public UMPHUD
{
	GENERATED_BODY()

public:
    UMPHUDOption();

protected :
    virtual void NativeConstruct() override;
}