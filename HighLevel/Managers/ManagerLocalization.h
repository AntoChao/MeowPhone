#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../CommonStruct.h"

// Forward declaration to avoid circular include
class UMPHUD;

#include "ManagerLocalization.generated.h"

UCLASS(BlueprintType, Blueprintable)
class MEOWPHONE_API UManagerLocalization : public UObject
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
    // Static instance for easy access
    static UManagerLocalization* GetInstance(UObject* WorldContext);

    /* Static global accessors */
    static UManagerLocalization* GetInstance();
}; 