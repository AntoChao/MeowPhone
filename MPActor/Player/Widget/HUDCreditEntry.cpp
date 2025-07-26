#include "HUDCreditEntry.h"
#include "Components/TextBlock.h"

void UHUDCreditEntry::SetCreditEntry(const FCreditEntryData& entry)
{
    if (assetNameText)
        assetNameText->SetText(FText::FromString(entry.AssetName));
    if (usageText)
        usageText->SetText(FText::FromString(entry.Usage));
    if (authorText)
        authorText->SetText(FText::FromString(entry.Author));
    if (sourceText)
        sourceText->SetText(FText::FromString(entry.Source));
} 