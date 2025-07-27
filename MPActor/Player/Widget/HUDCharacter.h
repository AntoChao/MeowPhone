#pragma once

// [Meow-Phone Project]
//
// This is the base class for the in-game character HUDs (for both Cat and Human).
// It contains UI elements and functionality that are common to both teams, such as the
// item slots, the crosshair/center image, and the interaction hint text.
//
// How to utilize in Blueprint:
// 1. You should not create a widget directly from this class. Instead, create your character HUD widgets by inheriting from its child classes, `UHUDCharacterCat` and `UHUDCharacterHuman`.
// 2. In a child Blueprint (e.g., `WBP_CatHUD`), you will design the UI layout. You MUST name your UI elements to match the `meta = (BindWidget)` variables here. For example, you must have an `Image` named `centerImage` and a `TextBlock` named `hintText` in your UMG editor. The `itemSlots` array is a special case that will likely be populated in the child Blueprint's `NativeConstruct`.
// 3. Child classes will call the `Update...` functions from this base class and also add their own specific update logic.
//
// Necessary things to define:
// - Child Blueprints MUST have widgets named correctly to match the `BindWidget` properties (`centerImage`, `hintText`).
//
// How it interacts with other classes:
// - UMPHUD: The base class for all HUDs in this project.
// - UHUDCharacterCat / UHUDCharacterHuman: These are the concrete implementations that inherit from this class, add their own specific widgets (e.g., health bars, ability icons), and are actually created by the `AMPControllerPlayer`.
// - AMPCharacter: The logic in the child HUDs will get the possessed character to query its state (e.g., get its inventory to update the item slots, get its detected actor to update the hint text).

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