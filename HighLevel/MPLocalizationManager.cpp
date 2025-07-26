#include "MPLocalizationManager.h"
#include "Engine/DataTable.h"
#include "HighLevel/MPLogManager.h"

UMPLocalizationManager::UMPLocalizationManager()
{
    currentLanguage = ELanguage::EEnglish;
    defaultText = FText::FromString(TEXT("TEXT_NOT_FOUND"));
}

void UMPLocalizationManager::InitializeLocalization()
{
	// Set default language
	currentLanguage = ELanguage::EEnglish;
	
	// Load text data
	LoadTextData();
	
	UMPLogManager::LogInfo(FString::Printf(TEXT("Initialized with %d text entries"), cachedTextData.Num()), TEXT("MPLocalizationManager"));
}

FText UMPLocalizationManager::GetLocalizedText(const FString& textKey) const
{
	return GetLocalizedTextForLanguage(textKey, currentLanguage);
}

FText UMPLocalizationManager::GetLocalizedTextForLanguage(const FString& textKey, ELanguage language) const
{
	// Check if we have cached data for this key and language
	FString cacheKey = FString::Printf(TEXT("%s_%d"), *textKey, static_cast<int32>(language));
	
	if (const FText* cachedText = cachedTextData.Find(cacheKey))
	{
		return *cachedText;
	}
	
	// If not found in cache, try to get from data table
	if (textDataTable)
	{
		// Try to find the text entry in the data table
		FLocalizedText* textEntry = textDataTable->FindRow<FLocalizedText>(FName(*textKey), TEXT(""));
		if (textEntry)
		{
			FText result = textEntry->GetLocalizedText(language);
			
			// Cache the result for future use
			const_cast<UMPLocalizationManager*>(this)->cachedTextData.Add(cacheKey, result);
			
			return result;
		}
	}
	
	// If not found, log warning and return the key as fallback
	UMPLogManager::LogWarning(FString::Printf(TEXT("Text key '%s' not found for language %d"), *textKey, (int32)language), TEXT("MPLocalizationManager"));
	return FText::FromString(textKey);
}

FText UMPLocalizationManager::GetLocalizedTextForCurrentLanguage(const FString& textKey) const
{
    return GetLocalizedText(textKey, currentLanguage);
}

void UMPLocalizationManager::SetCurrentLanguage(ELanguage newLanguage)
{
	if (currentLanguage != newLanguage)
	{
		currentLanguage = newLanguage;
		UMPLogManager::LogInfo(FString::Printf(TEXT("Language changed to %d"), (int32)newLanguage), TEXT("MPLocalizationManager"));
		
		// Notify all subscribers of the language change
		RefreshAllUIText();
	}
}

void UMPLocalizationManager::RefreshAllUIText()
{
	UMPLogManager::LogInfo(FString::Printf(TEXT("Refreshing all UI text - Language changed to %d"), (int32)currentLanguage), TEXT("MPLocalizationManager"));
	
	// Broadcast the language change event to all subscribers
	OnLanguageChanged.Broadcast();
}

bool UMPLocalizationManager::HasTextKey(const FString& textKey) const
{
    // Check cached data first
    if (cachedTextData.Contains(textKey))
    {
        return true;
    }

    // Check data table
    if (textDataTable)
    {
        return textDataTable->FindRow<FLocalizedText>(FName(*textKey), TEXT("HasTextKey")) != nullptr;
    }

    return false;
}

void UMPLocalizationManager::SubscribeToLanguageChanges(UObject* subscriber)
{
	if (subscriber)
	{
		OnLanguageChanged.AddUObject(subscriber, &UMPHUD::OnLanguageChanged);
	}
}

void UMPLocalizationManager::UnsubscribeFromLanguageChanges(UObject* subscriber)
{
	if (subscriber)
	{
		OnLanguageChanged.RemoveAll(subscriber);
	}
}

void UMPLocalizationManager::SetDataTable(UDataTable* newDataTable)
{
	textDataTable = newDataTable;
	
	if (textDataTable)
	{
		LoadTextData();
	}
	else
	{
		UMPLogManager::LogError(TEXT("No data table assigned!"), TEXT("MPLocalizationManager"));
	}
}

void UMPLocalizationManager::LoadTextData()
{
	cachedTextData.Empty();
	
	if (!textDataTable)
	{
		UMPLogManager::LogError(TEXT("No data table assigned!"), TEXT("MPLocalizationManager"));
		return;
	}
	
	// Get all rows from the data table
	TArray<FName> rowNames = textDataTable->GetRowNames();
	
	for (const FName& rowName : rowNames)
	{
		FLocalizedText* textEntry = textDataTable->FindRow<FLocalizedText>(rowName, TEXT(""));
		if (textEntry)
		{
			FString key = rowName.ToString();
			
			// Cache text for all languages
			for (int32 langIndex = 0; langIndex < static_cast<int32>(ELanguage::Max); ++langIndex)
			{
				ELanguage language = static_cast<ELanguage>(langIndex);
				FString cacheKey = FString::Printf(TEXT("%s_%d"), *key, langIndex);
				FText localizedText = textEntry->GetLocalizedText(language);
				
				cachedTextData.Add(cacheKey, localizedText);
			}
		}
	}
	
	UMPLogManager::LogInfo(FString::Printf(TEXT("Loaded %d text entries"), cachedTextData.Num()), TEXT("MPLocalizationManager"));
}

UMPLocalizationManager* UMPLocalizationManager::GetInstance(UObject* WorldContext)
{
    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
    {
        if (UMPGI* gameInstance = Cast<UMPGI>(UGameplayStatics::GetGameInstance(World)))
        {
            // Return the localization manager from the game instance
            return gameInstance->localizationManager;
        }
    }
    
    return nullptr;
} 