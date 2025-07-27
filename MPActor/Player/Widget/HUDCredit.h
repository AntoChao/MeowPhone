#pragma once

// [Meow-Phone Project]
//
// This class represents the Credits screen UI. It is designed to dynamically populate a
// scrollable list with credit entries (e.g., names and roles) from a data array.
//
// How to utilize in Blueprint:
// 1. Create a Widget Blueprint inheriting from this class (e.g., `WBP_Credits`).
// 2. In the UMG editor for this widget, create the necessary UI elements and name them to match the `BindWidget` properties (e.g., `creditListScrollBox`, `backButton`).
// 3. In the Blueprint's defaults, you must configure two key properties:
//    - **Credit Entries**: This is a `TArray<FCreditEntryData>`. You must populate this array with all the credit information (roles and names) that you want to display.
//    - **Credit Entry Widget Class**: You must create a separate Widget Blueprint for a single credit entry row (this widget should inherit from `UHUDCreditEntry`) and assign that Blueprint class here. This is the widget that will be spawned for each element in the `creditEntries` array.
// 4. When this widget is constructed, its C++ `NativeConstruct` calls `PopulateCreditList`. This function iterates through your `creditEntries` data array, and for each entry, it creates an instance of your `creditEntryWidgetClass`, populates it with the data, and adds it to the `creditListScrollBox`.
//
// Necessary things to define:
// - A `UScrollBox` named `creditListScrollBox` MUST exist in the child widget.
// - The `creditEntries` array MUST be filled out with data in the Blueprint's defaults.
// - The `creditEntryWidgetClass` MUST be set to a valid `UHUDCreditEntry` child Blueprint.
//
// How it interacts with other classes:
// - UMPHUD: The base HUD class.
// - FCreditEntryData (Struct): A struct (likely defined in `CommonStruct.h`) that holds the data for a single credit entry (e.g., `Role` and `Name`).
// - UHUDCreditEntry: A separate, simpler widget class that is responsible for displaying a single line of credit. This parent widget creates instances of it.
// - UScrollBox: The UMG widget used to contain and display the dynamically created list of credit entries.

#include "MPHUD.h"
#include "../../../CommonStruct.h"
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