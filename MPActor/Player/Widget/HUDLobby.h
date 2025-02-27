#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDLobby.generated.h"

UCLASS()
class UHUDLobby : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDLobby();

protected:
    virtual void NativeConstruct() override;
};