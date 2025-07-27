#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ManagerMP.generated.h"

class AMPGMGameplay;

UCLASS()
class UManagerMP : public UObject
{
    GENERATED_BODY()

protected:
    AMPGMGameplay* gameMode;

    virtual void InitializeManager(AMPGMGameplay* inGameMode);
}; 