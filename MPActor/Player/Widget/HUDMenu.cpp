#include "HUDMenu.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "MPControllerPlayer.h"
#include "HighLevel/MPGMGameplay.h"
#include "HighLevel/MPLogManager.h"
#include "HUDOption.h"
#include "HUDInit.h"

UHUDMenu::UHUDMenu() {}

void UHUDMenu::NativeConstruct()
{
    Super::NativeConstruct();
    // Bind button events
    if (continueButton)
        continueButton->OnClicked.AddDynamic(this, &UHUDMenu::OnContinueClicked);
    if (optionButton)
        optionButton->OnClicked.AddDynamic(this, &UHUDMenu::OnOptionClicked);
    if (quitButton)
        quitButton->OnClicked.AddDynamic(this, &UHUDMenu::OnQuitClicked);
    UpdateButtonTexts();
}

void UHUDMenu::SetOwner(AMPControllerPlayer* theOwner)
{
    owner = theOwner;
}

void UHUDMenu::UpdateButtonTexts()
{
    if (continueText)
        continueText->SetText(GetLocalizedText(continueTextKey));
    if (optionText)
        optionText->SetText(GetLocalizedText(optionTextKey));
    if (quitText)
        quitText->SetText(GetLocalizedText(quitTextKey));
}

void UHUDMenu::OnContinueClicked()
{
    if (owner)
    {
        // Remove this HUD, restore gameplay input/cursor
        owner->RemoveHUD(EHUDType::EMenu);
        owner->TurnGameplayInputOn();
    }
}

void UHUDMenu::OnOptionClicked()
{
    if (owner)
    {
        // Remove this HUD, attach OptionHUD, set previousHUD to EMenu
        owner->RemoveHUD(EHUDType::EMenu);
        owner->AttachHUD(EHUDType::EOption, 0);
        if (owner->optionHUD)
        {
            owner->optionHUD->SetPreviousHUD(EHUDType::EMenu);
        }
    }
}

void UHUDMenu::OnQuitClicked()
{
    if (owner)
    {
        // Remove this HUD, attach InitHUD, notify GameMode
        owner->RemoveHUD(EHUDType::EMenu);
        owner->AttachHUD(EHUDType::EInit, 0);
        AMPGMGameplay* GM = Cast<AMPGMGameplay>(GetWorld()->GetAuthGameMode());
        if (GM)
        {
            // Notify GameMode to update player count/win condition
            GM->Logout(owner);
        }
    }
}

void UHUDMenu::ReturnFromOptionHUD()
{
    // Called by OptionHUD if previousHUD is EMenu
    if (owner)
    {
        owner->AttachHUD(EHUDType::EMenu, 0);
    }
}