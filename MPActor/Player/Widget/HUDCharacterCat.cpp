#include "HUDCharacterCat.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"

UHUDCharacterCat::UHUDCharacterCat() {}

void UHUDCharacterCat::NativeConstruct()
{
    Super::NativeConstruct();
}

void UHUDCharacterCat::UpdateStruggleBar(float progress, bool visible)
{
    if (struggleBar)
    {
        struggleBar->SetPercent(progress);
        struggleBar->SetVisibility(visible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void UHUDCharacterCat::UpdateAbilitySlot(UTexture2D* abilityIcon, bool visible)
{
    if (abilitySlot)
    {
        if (abilityIcon)
        {
            FSlateBrush brush;
            brush.SetResourceObject(abilityIcon);
            abilitySlot->SetBrush(brush);
        }
        abilitySlot->SetVisibility(visible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void UHUDCharacterCat::UpdateOpponentWinProgression(float progress)
{
    if (opponentWinProgression)
    {
        opponentWinProgression->SetPercent(progress);
    }
}