#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CommonStruct.h"
#include "MPLocalizationManager.generated.h"

UCLASS(BlueprintType, Blueprintable)
class MEOWPHONE_API UMPLocalizationManager : public UObject
{
    GENERATED_BODY()

public:
    UMPLocalizationManager();

    // Initialize the localization system
    UFUNCTION(BlueprintCallable, Category = "Localization")
    void InitializeLocalization();

    // Get localized text by key
    UFUNCTION(BlueprintCallable, Category = "Localization")
    FText GetLocalizedText(const FString& textKey, ELanguage language = ELanguage::EEnglish) const;

    // Get localized text for current game language
    UFUNCTION(BlueprintCallable, Category = "Localization")
    FText GetLocalizedTextForCurrentLanguage(const FString& textKey) const;

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

    // Check if text key exists
    UFUNCTION(BlueprintCallable, Category = "Localization")
    bool HasTextKey(const FString& textKey) const;

protected:
    // Data table containing all localized text
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    UDataTable* localizationDataTable;

    // Current language (managed by Game Instance, not stored here)
    UPROPERTY()
    ELanguage currentLanguage;

    // Cached text data for performance
    UPROPERTY()
    TMap<FString, FLocalizedText> cachedTextData;

    // Load text data from data table
    UFUNCTION(BlueprintCallable, Category = "Localization")
    void LoadTextData();

    // Default text to return if key not found
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FText defaultText;

public:
    // Static instance for easy access
    static UMPLocalizationManager* GetInstance(UObject* WorldContext);
}; 