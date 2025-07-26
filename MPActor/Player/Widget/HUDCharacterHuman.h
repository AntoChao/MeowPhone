#pragma once

#include "HUDCharacter.h"
#include "HUDCharacterHuman.generated.h"

class UProgressBar;

UCLASS()
class UHUDCharacterHuman : public UHUDCharacter
{
    GENERATED_BODY()

public:
    UHUDCharacterHuman();

protected:
    virtual void NativeConstruct() override;

    // Human-specific UI components
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UProgressBar* hpBar;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UProgressBar* opponentWinProgression;

public:
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateHPBar(float hpPercent);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateOpponentWinProgression(float progress);
};