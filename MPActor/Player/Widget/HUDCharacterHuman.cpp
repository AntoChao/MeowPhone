#include "HUDCharacterHuman.h"
#include "Components/ProgressBar.h"

UHUDCharacterHuman::UHUDCharacterHuman() {}

void UHUDCharacterHuman::NativeConstruct()
{
    Super::NativeConstruct();
}

void UHUDCharacterHuman::UpdateHPBar(float hpPercent)
{
    if (hpBar)
    {
        hpBar->SetPercent(hpPercent);
    }
}

void UHUDCharacterHuman::UpdateOpponentWinProgression(float progress)
{
    if (opponentWinProgression)
    {
        opponentWinProgression->SetPercent(progress);
    }
}