#pragma once

#include "MPHUD.h"
#include "CommonStruct.h"
#include "HUDCredit.generated.h"

class UTextBlock;
class UButton;
class UScrollBox;
class UHUDCreditEntry;

UCLASS()
class UHUDCredit : public UMPHUD
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UTextBlock* thanksText;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UButton* backButton;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    UTextBlock* backButtonText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    class UScrollBox* creditListScrollBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credit")
    TArray<FCreditEntryData> creditEntries;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Credit")
    TSubclassOf<UHUDCreditEntry> creditEntryWidgetClass;

    UPROPERTY(BlueprintReadWrite, Category = "Credit")
    TArray<UHUDCreditEntry*> creditEntryWidgets;

public:
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetThanksText(const FString& key);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetBackButtonText(const FString& key);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void OnBackButtonClicked();
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void PopulateCreditList();
};