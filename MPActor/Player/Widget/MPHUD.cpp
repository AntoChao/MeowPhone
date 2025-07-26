#include "MPHUD.h"
#include "HighLevel/MPGI.h"
#include "HighLevel/MPLocalizationManager.h"
#include "HighLevel/MPLogManager.h"
#include "MPActor/Player/MPControllerPlayer.h"

void UMPHUD::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Get game instance and set it
	curGameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
	
	// Validate root widget
	if (!ValidateRootWidget())
	{
		UMPLogManager::LogError(TEXT("Root widget validation failed! Widget will not function properly."), TEXT("MPHUD"));
		return;
	}
	
	// Update texts after construction
	UpdateTexts();
}

void UMPHUD::NativeDestruct()
{
	// Unsubscribe from language changes
	UnsubscribeFromLanguageChanges();
	
	Super::NativeDestruct();
}

void UMPHUD::SetOwner(AMPControllerPlayer* theOwner)
{
	owner = theOwner;
}

void UMPHUD::UpdateTexts()
{
	// Base implementation - child classes should override this
	UMPLogManager::LogWarning(TEXT("UpdateTexts called on base class"), TEXT("MPHUD"));
}

void UMPHUD::OnLanguageChanged()
{
	// Base implementation - child classes should override this
	UpdateTexts();
}

FText UMPHUD::GetLocalizedText(const FString& textKey) const
{
	if (curGameInstance && curGameInstance->GetLocalizationManager())
	{
		return curGameInstance->GetLocalizationManager()->GetLocalizedText(textKey);
	}
	
	// Fallback to key if localization is not available
	return FText::FromString(textKey);
}

void UMPHUD::SubscribeToLanguageChanges()
{
	if (curGameInstance && curGameInstance->GetLocalizationManager())
	{
		curGameInstance->GetLocalizationManager()->SubscribeToLanguageChanges(this);
		UMPLogManager::LogInfo(TEXT("Subscribed to language changes"), TEXT("MPHUD"));
	}
	else
	{
		UMPLogManager::LogWarning(TEXT("Cannot subscribe - GameInstance or LocalizationManager not ready"), TEXT("MPHUD"));
	}
}

void UMPHUD::UnsubscribeFromLanguageChanges()
{
	if (curGameInstance && curGameInstance->GetLocalizationManager())
	{
		curGameInstance->GetLocalizationManager()->UnsubscribeFromLanguageChanges(this);
		UMPLogManager::LogInfo(TEXT("Unsubscribed from language changes"), TEXT("MPHUD"));
	}
}

bool UMPHUD::ValidateRootWidget()
{
	// Base implementation - child classes should override this
	UMPLogManager::LogWarning(TEXT("ValidateRootWidget called on base class"), TEXT("MPHUD"));
	return true;
}

