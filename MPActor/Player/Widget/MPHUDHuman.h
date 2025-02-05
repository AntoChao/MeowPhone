#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "MPHUDHuman.generated.h"

UCLASS()
class UMPHUDHuman : public UMPHUD
{
	GENERATED_BODY()

public:
    UMPHUDHuman();

protected :
    virtual void NativeConstruct() override;
}