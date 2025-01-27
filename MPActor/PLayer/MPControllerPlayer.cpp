#include "MPControllerPlayer.h"

AMPControllerPlayer::AMPControllerPlayer()
{

}

// common player controller properties
virtual void BeginPlay() override;

// hud manager
void SetupInitWidget();
UFUNCTION(BlueprintCallable, Category = "HUD Method")
void SetupEndGPWidget();