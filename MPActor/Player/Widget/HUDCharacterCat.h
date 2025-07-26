#pragma once

#include "HUDCharacter.h"
#include "HUDCharacterCat.generated.h"

class UProgressBar;
class UImage;

UCLASS()
class UHUDCharacterCat : public UHUDCharacter
{
    GENERATED_BODY()

public:
    UHUDCharacterCat();

protected:
    virtual void NativeConstruct() override;

    // Cat-specific UI components
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UProgressBar* struggleBar;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UImage* abilitySlot;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UProgressBar* opponentWinProgression;

public:
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateStruggleBar(float progress, bool visible);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateAbilitySlot(UTexture2D* abilityIcon, bool visible);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateOpponentWinProgression(float progress);
};