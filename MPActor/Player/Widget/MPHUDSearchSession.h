#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "MPHUDSearchSession.generated.h"

UCLASS()
class UMPHUDSearchSession : public UMPHUD
{
	GENERATED_BODY()

public:
    UMPHUDSearchSession();

protected :
    virtual void NativeConstruct() override;
}