#pragma once

// [Meow-Phone Project]
//
// This is a singleton manager responsible for handling all text localization in the game.
// It reads from a DataTable to provide the correct text based on the currently selected language.
// It also provides a mechanism for UI elements to automatically update when the language changes.
//
// How to utilize in Blueprint:
// 1. Get the singleton instance of this manager using the static `GetInstance()` function.
// 2. To get a piece of text for a UI element, call `GetLocalizedText` with the appropriate `textKey`. The key corresponds to a row name in your localization DataTable.
// 3. In your game's Option/Settings menu, you would call `SetCurrentLanguage` on this manager when the player chooses a new language. This will trigger all subscribed UI elements to refresh.
// 4. Any custom HUD class (`UMPHUD`) that needs its text to update automatically when the language changes should call `SubscribeToLanguageChanges` on this manager during its `Initialize` or `Construct` event, passing in a reference to itself.
//
// Necessary things to define:
// - `textDataTable`: This is the most critical property. You MUST create a `UManagerLocalization` Blueprint and assign your localization DataTable to this property. The Game Instance should then be configured to use this specific Blueprint.
// - The DataTable itself must be structured correctly, typically with a key (FString or FName) as the first column and then separate columns for each supported language (e.g., "en", "es", "fr").
//
// How it interacts with other classes:
// - UManagerMP: Inherits from the base manager class.
// - UMPGI (Game Instance): The Game Instance is the ideal owner for this manager, as localization needs to persist across level changes. The GI is responsible for creating and initializing it.
// - UDataTable: It relies entirely on a DataTable asset for all of its source text.
// - UMPHUD: HUD widgets can subscribe to this manager's `OnLanguageChanged` delegate to know when they need to refresh their text fields.
// - CommonStruct.h (FLocalizedString): It likely uses structs defined here to structure the data within the DataTable.
// - FText: The manager's core output is Unreal's `FText` type, which is designed for localization.

#include "CoreMinimal.h"
#include "../../CommonStruct.h"
#include "ManagerMP.h"

#include "ManagerLocalization.generated.h"

class UDataTable;
class UMPHUD;

UCLASS(BlueprintType, Blueprintable)
class UManagerLocalization : public UManagerMP
{
    GENERATED_BODY()

public:
    UManagerLocalization();

    // Initialize the localization system
    UFUNCTION(BlueprintCallable, Category = "Localization")
    void InitializeLocalization();

    // Get localized text by key
    UFUNCTION(BlueprintCallable, Category = "Localization")
    FText GetLocalizedText(const FString& textKey) const;

    UFUNCTION(BlueprintCallable, Category = "Localization")
    FText GetLocalizedTextForLanguage(const FString& textKey, ELanguage language) const;

    // Set current language (called by Game Instance only)
    UFUNCTION(BlueprintCallable, Category = "Localization")
    void SetCurrentLanguage(ELanguage newLanguage);

    // Get current language
    UFUNCTION(BlueprintCallable, Category = "Localization")
    ELanguage GetCurrentLanguage() const { return currentLanguage; }

    	// Refresh all UI text (call when language changes)
	UFUNCTION(BlueprintCallable, Category = "Localization")
	void RefreshAllUIText();
	
	// Delegate for language change events
	DECLARE_MULTICAST_DELEGATE(FOnLanguageChanged);
	FOnLanguageChanged OnLanguageChanged;

    // Allow HUD widgets to subscribe/unsubscribe to language change events
    UFUNCTION(BlueprintCallable, Category = "Localization")
    void SubscribeToLanguageChanges(UMPHUD* subscriber);

    UFUNCTION(BlueprintCallable, Category = "Localization")
    void UnsubscribeFromLanguageChanges(UMPHUD* subscriber);

    // Check if text key exists
    UFUNCTION(BlueprintCallable, Category = "Localization")
    bool HasTextKey(const FString& textKey) const;

    UFUNCTION(BlueprintCallable, Category = "Localization")
    void SetDataTable(UDataTable* newDataTable);

protected:
    // Data table containing all localized text
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    UDataTable* textDataTable;

    // Current language (managed by Game Instance, not stored here)
    UPROPERTY()
    ELanguage currentLanguage;

    // Cached text for quick lookup: key_language -> localized FText
    UPROPERTY()
    TMap<FString, FText> cachedTextData;

    // Load text data from data table
    UFUNCTION(BlueprintCallable, Category = "Localization")
    void LoadTextData();

    // Default text to return if key not found
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FText defaultText;

public:
    /* Static global accessors */
    static UManagerLocalization* GetInstance();
}; 