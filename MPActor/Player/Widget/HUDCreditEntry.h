#pragma once

#include "Blueprint/UserWidget.h"
#include "HUDCredit.h"
#include "HUDCreditEntry.generated.h"

class UTextBlock;

UCLASS()
class UHUDCreditEntry : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Credit")
    UTextBlock* assetNameText;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Credit")
    UTextBlock* usageText;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Credit")
    UTextBlock* authorText;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Credit")
    UTextBlock* sourceText;

    UFUNCTION(BlueprintCallable, Category = "Credit")
    void SetCreditEntry(const FCreditEntryData& entry);
}; 