#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MPHUD.generated.h"

class UMPGI;
class AMPControllerPlayer;
enum class EAbility : uint8;

UCLASS()
class UMPHUD : public UUserWidget
{
	GENERATED_BODY()

public:
    UMPHUD();

protected :
    virtual void NativeConstruct() override;

// common HUD properties
protected :
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        EHUDType curHUDType;

    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        UMPGI* curGameInstance;

    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        AMPControllerPlayer* owner;

public :
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
		void SetOwner(AMPControllerPlayer* theOwner);
}