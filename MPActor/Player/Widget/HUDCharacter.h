#pragma once

#include "MPHUD.h"
#include "HUDCharacter.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class UHUDCharacter : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDCharacter();

protected:
    virtual void NativeConstruct() override;

    // Shared UI components for all character HUDs
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    TArray<UImage*> itemSlots;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UImage* centerImage;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UTextBlock* hintText;

public:
    // Shared logic for updating UI
    UFUNCTION(BlueprintCallable, Category = "HUD")
    virtual void UpdateItemSlots();
    UFUNCTION(BlueprintCallable, Category = "HUD")
    virtual void UpdateCenterImage();
    UFUNCTION(BlueprintCallable, Category = "HUD")
    virtual void UpdateHintText(const FString& hintKey);
}; 