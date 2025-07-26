#include "HUDSessionGeneral.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "HighLevel/MPGI.h"
#include "HighLevel/MPLogManager.h"
#include "MPActor/Player/MPControllerPlayer.h"

void UHUDSessionGeneral::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Validate root widget first
	if (!ValidateRootWidget())
	{
		UMPLogManager::LogError(TEXT("Root widget validation failed!"), TEXT("HUDSessionGeneral"));
		return;
	}
	
	// Update texts after construction
	UpdateTexts();
	
	// Bind button events
	if (createSessionButton)
	{
		createSessionButton->OnClicked.AddDynamic(this, &UHUDSessionGeneral::OnCreateSessionClicked);
	}
	
	if (searchSessionButton)
	{
		searchSessionButton->OnClicked.AddDynamic(this, &UHUDSessionGeneral::OnSearchSessionClicked);
	}
	
	if (backButton)
	{
		backButton->OnClicked.AddDynamic(this, &UHUDSessionGeneral::OnBackClicked);
	}
}

void UHUDSessionGeneral::NativeDestruct()
{
	// Unbind button events
	if (createSessionButton)
	{
		createSessionButton->OnClicked.RemoveAll(this);
	}
	
	if (searchSessionButton)
	{
		searchSessionButton->OnClicked.RemoveAll(this);
	}
	
	if (backButton)
	{
		backButton->OnClicked.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UHUDSessionGeneral::UpdateTexts()
{
	Super::UpdateTexts();
	
	// Update title
	if (titleText)
	{
		titleText->SetText(GetLocalizedText(sessionTitleTextKey));
	}
	
	// Update button texts
	if (createSessionButton)
	{
		UTextBlock* buttonText = Cast<UTextBlock>(createSessionButton->GetChildAt(0));
		if (buttonText)
		{
			buttonText->SetText(GetLocalizedText(createSessionTextKey));
		}
	}
	
	if (searchSessionButton)
	{
		UTextBlock* buttonText = Cast<UTextBlock>(searchSessionButton->GetChildAt(0));
		if (buttonText)
		{
			buttonText->SetText(GetLocalizedText(searchSessionTextKey));
		}
	}
	
	if (backButton)
	{
		UTextBlock* buttonText = Cast<UTextBlock>(backButton->GetChildAt(0));
		if (buttonText)
		{
			buttonText->SetText(GetLocalizedText(backTextKey));
		}
	}
}

bool UHUDSessionGeneral::ValidateRootWidget()
{
	if (!rootCanvas)
	{
		UMPLogManager::LogError(TEXT("Root canvas is missing!"), TEXT("HUDSessionGeneral"));
		return false;
	}
	return true;
}

// Button click functions
void UHUDSessionGeneral::OnCreateSessionClicked()
{
	UMPLogManager::LogInfo(TEXT("Create session clicked"), TEXT("HUDSessionGeneral"));
	
	if (!owner)
	{
		UMPLogManager::LogError(TEXT("Owner is null, cannot navigate"), TEXT("HUDSessionGeneral"));
		return;
	}
	
	// Remove current HUD and add Create Session HUD
	owner->RemoveHUD(EHUDType::ESessionGeneral);
	owner->AttachHUD(EHUDType::ECreateSession, 0);
	
	UMPLogManager::LogInfo(TEXT("Switched to Create Session HUD"), TEXT("HUDSessionGeneral"));
}

void UHUDSessionGeneral::OnSearchSessionClicked()
{
	UMPLogManager::LogInfo(TEXT("Search session clicked"), TEXT("HUDSessionGeneral"));
	
	if (!owner)
	{
		UMPLogManager::LogError(TEXT("Owner is null, cannot navigate"), TEXT("HUDSessionGeneral"));
		return;
	}
	
	// Remove current HUD and add Search Session HUD
	owner->RemoveHUD(EHUDType::ESessionGeneral);
	owner->AttachHUD(EHUDType::ESearchSession, 0);
	
	UMPLogManager::LogInfo(TEXT("Switched to Search Session HUD"), TEXT("HUDSessionGeneral"));
}

void UHUDSessionGeneral::OnBackClicked()
{
	UMPLogManager::LogInfo(TEXT("Back clicked"), TEXT("HUDSessionGeneral"));
	
	if (!owner)
	{
		UMPLogManager::LogError(TEXT("Owner is null, cannot navigate"), TEXT("HUDSessionGeneral"));
		return;
	}
	
	// Remove current HUD and add Init HUD
	owner->RemoveHUD(EHUDType::ESessionGeneral);
	owner->AttachHUD(EHUDType::EInit, 0);
	
	UMPLogManager::LogInfo(TEXT("Switched back to Init HUD"), TEXT("HUDSessionGeneral"));
}