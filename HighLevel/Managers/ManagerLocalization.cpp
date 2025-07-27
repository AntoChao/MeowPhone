#include "Managers/ManagerLocalization.h"
#include "Engine/DataTable.h"
#include "../MPActor/Player/Widget/MPHUD.h"
#include "Managers/ManagerLog.h"
#include "Kismet/GameplayStatics.h"

UManagerLocalization::UManagerLocalization()
{
    currentLanguage = ELanguage::EEnglish;
    defaultText = FText::FromString(TEXT("TEXT_NOT_FOUND"));
}

void UManagerLocalization::InitializeLocalization()
{
	// Set default language
	currentLanguage = ELanguage::EEnglish;
	
	// Load text data
	LoadTextData();
	
	UManagerLog::LogInfo(FString::Printf(TEXT("Initialized with %d text entries"), cachedTextData.Num()), TEXT("ManagerLocalization"));
}

FText UManagerLocalization::GetLocalizedText(const FString& textKey) const
{
	return GetLocalizedTextForLanguage(textKey, currentLanguage);
}

FText UManagerLocalization::GetLocalizedTextForLanguage(const FString& textKey, ELanguage language) const
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
			const_cast<UManagerLocalization*>(this)->cachedTextData.Add(cacheKey, result);
			
			return result;
		}
	}
	
	// If not found, log warning and return the key as fallback
	UManagerLog::LogWarning(FString::Printf(TEXT("Text key '%s' not found for language %d"), *textKey, (int32)language), TEXT("ManagerLocalization"));
	return FText::FromString(textKey);
}

void UManagerLocalization::SetCurrentLanguage(ELanguage newLanguage)
{
	if (currentLanguage != newLanguage)
	{
		currentLanguage = newLanguage;
		UManagerLog::LogInfo(FString::Printf(TEXT("Language changed to %d"), (int32)newLanguage), TEXT("ManagerLocalization"));
		
		// Notify all subscribers of the language change
		RefreshAllUIText();
	}
}

void UManagerLocalization::RefreshAllUIText()
{
	UManagerLog::LogInfo(FString::Printf(TEXT("Refreshing all UI text - Language changed to %d"), (int32)currentLanguage), TEXT("ManagerLocalization"));
	
	// Broadcast the language change event to all subscribers
	OnLanguageChanged.Broadcast();
}

bool UManagerLocalization::HasTextKey(const FString& textKey) const
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

void UManagerLocalization::SubscribeToLanguageChanges(UMPHUD* subscriber)
{
	if (IsValid(subscriber))
	{
		OnLanguageChanged.AddUObject(subscriber, &UMPHUD::OnLanguageChanged);
	}
}

void UManagerLocalization::UnsubscribeFromLanguageChanges(UMPHUD* subscriber)
{
	if (IsValid(subscriber))
	{
		OnLanguageChanged.RemoveAll(subscriber);
	}
}

void UManagerLocalization::SetDataTable(UDataTable* newDataTable)
{
	textDataTable = newDataTable;
	
	if (textDataTable)
	{
		LoadTextData();
	}
	else
	{
		UManagerLog::LogError(TEXT("No data table assigned!"), TEXT("ManagerLocalization"));
	}
}

void UManagerLocalization::LoadTextData()
{
	cachedTextData.Empty();
	
	if (!textDataTable)
	{
		UManagerLog::LogError(TEXT("No data table assigned!"), TEXT("ManagerLocalization"));
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
	
	UManagerLog::LogInfo(FString::Printf(TEXT("Loaded %d text entries"), cachedTextData.Num()), TEXT("ManagerLocalization"));
}

UManagerLocalization* UManagerLocalization::GetInstance()
{
    static UManagerLocalization* Cached = nullptr;
    if (Cached && Cached->IsValidLowLevel())
    {
        return Cached;
    }

    // Attempt to fetch from any current world’s game instance
    if (GEngine && GEngine->GetWorldContexts().Num() > 0)
    {
        UWorld* World = GEngine->GetWorldContexts()[0].World();
        if (World)
        {
            if (UMPGI* GI = Cast<UMPGI>(UGameplayStatics::GetGameInstance(World)))
            {
                Cached = GI->GetLocalizationManager();
                return Cached;
            }
        }
    }

    return nullptr;
}