#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "MPHUDCredit.generated.h"

UCLASS()
class UMPHUDCredit : public UMPHUD
{
	GENERATED_BODY()

public:
    UMPHUDCredit();

protected :
    virtual void NativeConstruct() override;
}