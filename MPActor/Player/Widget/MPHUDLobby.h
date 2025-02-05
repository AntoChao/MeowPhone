#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "MPHUDLobby.generated.h"

UCLASS()
class UMPHUDLobby : public UMPHUD
{
	GENERATED_BODY()

public:
    UMPHUDLobby();

protected :
    virtual void NativeConstruct() override;
}