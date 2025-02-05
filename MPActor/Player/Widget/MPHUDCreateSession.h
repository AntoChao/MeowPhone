#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "MPHUDCreateSession.generated.h"

UCLASS()
class UMPHUDCreateSession : public UMPHUD
{
	GENERATED_BODY()

public:
    UMPHUDCreateSession();

protected :
    virtual void NativeConstruct() override;
}