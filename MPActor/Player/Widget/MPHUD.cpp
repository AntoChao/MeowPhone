#include "MPHUD.h"
#include "../../../HighLevel/MPGI.h"
#include "../../../HighLevel/Managers/ManagerLocalization.h"
#include "../../../HighLevel/Managers/ManagerLog.h"
#include "../MPControllerPlayer.h"

void UMPHUD::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Get game instance and set it
	curGameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
	
	// Validate root widget
	if (!ValidateRootWidget())
	{
		UManagerLog::LogError(TEXT("Root widget validation failed! Widget will not function properly."), TEXT("MPHUD"));
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
	UManagerLog::LogWarning(TEXT("UpdateTexts called on base class"), TEXT("MPHUD"));
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
		UManagerLog::LogInfo(TEXT("Subscribed to language changes"), TEXT("MPHUD"));
	}
	else
	{
		UManagerLog::LogWarning(TEXT("Cannot subscribe - GameInstance or LocalizationManager not ready"), TEXT("MPHUD"));
	}
}

void UMPHUD::UnsubscribeFromLanguageChanges()
{
	if (curGameInstance && curGameInstance->GetLocalizationManager())
	{
		curGameInstance->GetLocalizationManager()->UnsubscribeFromLanguageChanges(this);
		UManagerLog::LogInfo(TEXT("Unsubscribed from language changes"), TEXT("MPHUD"));
	}
}

bool UMPHUD::ValidateRootWidget()
{
	// Base implementation - child classes should override this
	UManagerLog::LogWarning(TEXT("ValidateRootWidget called on base class"), TEXT("MPHUD"));
	return true;
}

