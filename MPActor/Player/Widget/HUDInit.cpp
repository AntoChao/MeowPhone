#include "HUDInit.h"

#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "../../../HighLevel/Managers/ManagerLog.h"
#include "../MPControllerPlayer.h"
#include "../../../CommonEnum.h"

// ---------- Button callbacks ----------
void UHUDInit::GoSessionGeneralWidget()
{
    if (!owner) return;
    owner->AttachHUD(EHUDType::ESessionGeneral, 0);
    owner->RemoveHUD(EHUDType::EInit);
}

void UHUDInit::GoOptionWidget()
{
    if (!owner) return;
    owner->AttachHUD(EHUDType::EOption, 0);
    owner->RemoveHUD(EHUDType::EInit);
}

void UHUDInit::GoCreditWidget()
{
    if (!owner) return;
    owner->AttachHUD(EHUDType::ECredit, 0);
    owner->RemoveHUD(EHUDType::EInit);
}

void UHUDInit::QuitGame()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
    }
}

// ---------- Text localisation & validation ----------
void UHUDInit::UpdateTexts()
{
    // Ensure widgets are valid first
    if (!ValidateRootWidget()) return;

    if (startGameTXTBlock)
    {
        startGameTXTBlock->SetText(GetLocalizedText(startGameTextKey));
    }
    if (optionTXTBlock)
    {
        optionTXTBlock->SetText(GetLocalizedText(optionTextKey));
    }
    if (creditTXTBlock)
    {
        creditTXTBlock->SetText(GetLocalizedText(creditTextKey));
    }
    if (quitTXTBlock)
    {
        quitTXTBlock->SetText(GetLocalizedText(quitTextKey));
    }
}

bool UHUDInit::ValidateRootWidget()
{
    const bool bValid = (rootCanvas != nullptr);
    if (!bValid)
    {
        UManagerLog::LogError(TEXT("HUDInit: RootCanvas not bound – check UMG blueprint"), TEXT("HUDInit"));
    }
    return bValid;
}

void UHUDInit::NativeDestruct()
{
    // Unsubscribe from language change delegate before destruction
    UnsubscribeFromLanguageChanges();
    Super::NativeDestruct();
} 