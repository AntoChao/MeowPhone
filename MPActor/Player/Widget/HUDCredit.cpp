#include "HUDCredit.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "HUDCreditEntry.h"
#include "../MPControllerPlayer.h"

void UHUDCredit::NativeConstruct()
{
    Super::NativeConstruct();
    if (backButton)
    {
        backButton->OnClicked.AddDynamic(this, &UHUDCredit::OnBackButtonClicked);
    }
    PopulateCreditList();
}

void UHUDCredit::SetThanksText(const FString& key)
{
    if (thanksText)
    {
        thanksText->SetText(GetLocalizedText(key));
    }
}

void UHUDCredit::SetBackButtonText(const FString& key)
{
    if (backButtonText)
    {
        backButtonText->SetText(GetLocalizedText(key));
    }
}

void UHUDCredit::OnBackButtonClicked()
{
    if (owner)
    {
        owner->RemoveHUD(EHUDType::ECredit);
        owner->AttachHUD(EHUDType::EInit, 0);
    }
}

void UHUDCredit::PopulateCreditList()
{
    if (!creditListScrollBox || !creditEntryWidgetClass)
        return;
    creditListScrollBox->ClearChildren();
    creditEntryWidgets.Empty();
    for (const FCreditEntryData& entry : creditEntries)
    {
        UHUDCreditEntry* entryWidget = CreateWidget<UHUDCreditEntry>(this, creditEntryWidgetClass);
        if (entryWidget)
        {
            entryWidget->SetCreditEntry(entry);
            creditListScrollBox->AddChild(entryWidget);
            creditEntryWidgets.Add(entryWidget);
        }
    }
}