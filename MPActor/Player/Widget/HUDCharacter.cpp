#include "HUDCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

UHUDCharacter::UHUDCharacter() {}

void UHUDCharacter::NativeConstruct()
{
    Super::NativeConstruct();
    // Initialize or update shared UI components here
    UpdateItemSlots();
    UpdateCenterImage();
    UpdateHintText("");
}

void UHUDCharacter::UpdateItemSlots() {
    // Get the owning character
    AMPCharacter* character = Cast<AMPCharacter>(GetOwningPlayerPawn());
    if (!character) return;

    const TArray<AMPItem*>& inventory = character->GetInventory();
    int32 slotCount = itemSlots.Num();
    for (int32 i = 0; i < slotCount; ++i) {
        if (itemSlots[i]) {
            if (i < inventory.Num() && inventory[i]) {
                // TODO: Set itemSlots[i]->SetBrushFromTexture or similar with the item's icon
                itemSlots[i]->SetVisibility(ESlateVisibility::Visible);
            } else {
                // Empty slot
                itemSlots[i]->SetVisibility(ESlateVisibility::Hidden);
            }
        }
    }
}

void UHUDCharacter::UpdateCenterImage() {
    AMPCharacter* character = Cast<AMPCharacter>(GetOwningPlayerPawn());
    if (!centerImage) return;
    if (!character) {
        centerImage->SetVisibility(ESlateVisibility::Hidden);
        return;
    }
    if (character->GetCurHoldingItem()) {
        // TODO: Set centerImage->SetBrushFromTexture with the held item's icon
        centerImage->SetVisibility(ESlateVisibility::Visible);
    } else {
        // Show default blackpoint or detection trace image
        // TODO: Set centerImage->SetBrushFromTexture with default image
        centerImage->SetVisibility(ESlateVisibility::Visible);
    }
}

void UHUDCharacter::UpdateHintText(const FString& hintKey) {
    AMPCharacter* character = Cast<AMPCharacter>(GetOwningPlayerPawn());
    if (!hintText) return;
    FString key = hintKey;
    if (character) {
        if (key.IsEmpty()) {
            key = TEXT("none");
        }
        FText localizedText = GetLocalizedText(key);
        hintText->SetText(localizedText);
    } else {
        hintText->SetText(GetLocalizedText(TEXT("none")));
    }
} 