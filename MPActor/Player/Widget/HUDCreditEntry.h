#pragma once

// [Meow-Phone Project]
//
// This class represents a single row or entry in the credits screen. It is a simple,
// data-driven widget whose only job is to display the text for one credit item.
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_CreditEntry`). This will serve as the template for each row in the credit list.
// 2. In the UMG editor, design the layout for a single row. You must include `TextBlock` widgets and name them `assetNameText`, `usageText`, `authorText`, and `sourceText` to match the `BindWidget` properties below.
// 3. You do not create this widget directly. Instead, you assign your `WBP_CreditEntry` Blueprint to the `creditEntryWidgetClass` property on the main `UHUDCredit` widget.
// 4. The main `UHUDCredit` widget will then create instances of your `WBP_CreditEntry` for each item in its data array and call the `SetCreditEntry` function to populate the text fields.
//
// Necessary things to define:
// - The child Widget Blueprint MUST have `TextBlock` widgets that are correctly named to match the `BindWidget` properties.
//
// How it interacts with other classes:
// - UUserWidget: The base class for all UMG widgets.
// - UHUDCredit: The "parent" widget that creates and manages instances of this entry widget.
// - FCreditEntryData (Struct): The `SetCreditEntry` function takes this struct and uses its data to set the contents of its `TextBlock` widgets.

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