#include "HUDEnd.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "../MPControllerPlayer.h"

void UHUDEnd::NativeConstruct()
{
    Super::NativeConstruct();
    if (returnLobbyButton)
    {
        returnLobbyButton->OnClicked.AddDynamic(this, &UHUDEnd::OnReturnLobbyClicked);
    }
}

void UHUDEnd::SetResultText(const FString& resultKey)
{
    if (resultText)
    {
        resultText->SetText(GetLocalizedText(resultKey));
    }
}

void UHUDEnd::SetHumanProgression(float percent)
{
    if (humanProgressionBar)
    {
        humanProgressionBar->SetPercent(percent);
    }
}

void UHUDEnd::SetCatProgression(float percent)
{
    if (catProgressionBar)
    {
        catProgressionBar->SetPercent(percent);
    }
}

void UHUDEnd::SetReturnLobbyButtonText(const FString& key)
{
    if (returnLobbyButtonText)
    {
        returnLobbyButtonText->SetText(GetLocalizedText(key));
    }
}

void UHUDEnd::OnReturnLobbyClicked()
{
    if (owner)
    {
        owner->RemoveHUD(EHUDType::EEnd);
        owner->AttachHUD(EHUDType::ELobby, 0);
    }
}