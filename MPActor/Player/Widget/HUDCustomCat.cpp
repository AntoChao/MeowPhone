#include "HUDCustomCat.h"
#include "Components/PanelWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Viewport.h"
#include "MPControllerPlayer.h"
#include "MPPlayerState.h"
#include "../HighLevel/MPLogManager.h"
#include "HighLevel/MPGMGameplay.h"

UHUDCustomCat::UHUDCustomCat()
{
    // Initialize default values
    selectedCatRace = ECatRace::ECat1;
    selectedHat = EHat::ENone;
    selectedAbility = EAbility::ENone;
    previewManager = nullptr;
}

void UHUDCustomCat::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind button events
    if (raceLeftButton)
    {
        raceLeftButton->OnClicked.AddDynamic(this, &UHUDCustomCat::OnCatRaceLeftClicked);
    }
    
    if (raceRightButton)
    {
        raceRightButton->OnClicked.AddDynamic(this, &UHUDCustomCat::OnCatRaceRightClicked);
    }
    
    if (hatLeftButton)
    {
        hatLeftButton->OnClicked.AddDynamic(this, &UHUDCustomCat::OnHatLeftClicked);
    }
    
    if (hatRightButton)
    {
        hatRightButton->OnClicked.AddDynamic(this, &UHUDCustomCat::OnHatRightClicked);
    }
    
    if (abilityLeftButton)
    {
        abilityLeftButton->OnClicked.AddDynamic(this, &UHUDCustomCat::OnAbilityLeftClicked);
    }
    
    if (abilityRightButton)
    {
        abilityRightButton->OnClicked.AddDynamic(this, &UHUDCustomCat::OnAbilityRightClicked);
    }

    // Load current customization from player state
    LoadCustomization();
    
    // Initialize UI
    UpdateUI();
    
    UMPLogManager::LogInfo(TEXT("Cat Customization HUD constructed and initialized"), TEXT("HUDCustomCat"));
}

void UHUDCustomCat::NativeDestruct()
{
    // Unbind button events
    if (raceLeftButton)
    {
        raceLeftButton->OnClicked.RemoveAll(this);
    }
    
    if (raceRightButton)
    {
        raceRightButton->OnClicked.RemoveAll(this);
    }
    
    if (hatLeftButton)
    {
        hatLeftButton->OnClicked.RemoveAll(this);
    }
    
    if (hatRightButton)
    {
        hatRightButton->OnClicked.RemoveAll(this);
    }
    
    if (abilityLeftButton)
    {
        abilityLeftButton->OnClicked.RemoveAll(this);
    }
    
    if (abilityRightButton)
    {
        abilityRightButton->OnClicked.RemoveAll(this);
    }

    Super::NativeDestruct();
    
    UMPLogManager::LogInfo(TEXT("Cat Customization HUD destroyed"), TEXT("HUDCustomCat"));
}

void UHUDCustomCat::UpdateCustomizationOptions()
{
    // Update cat race text
    if (raceText)
    {
        raceText->SetText(GetCatRaceDisplayText(selectedCatRace));
    }
    
    // Update hat text
    if (hatText)
    {
        hatText->SetText(GetHatDisplayText(selectedHat));
    }
    
    // Update ability text
    if (abilityText)
    {
        abilityText->SetText(GetAbilityDisplayText(selectedAbility));
    }
    
    UMPLogManager::LogDebug(FString::Printf(TEXT("Updated customization options - Race: %d, Hat: %d, Ability: %d"), 
        (int32)selectedCatRace, (int32)selectedHat, (int32)selectedAbility), TEXT("HUDCustomCat"));
}

void UHUDCustomCat::UpdateCharacterPreview()
{
    // Update the main world preview character via GameMode and focus the preview camera via PlayerController.
    // There is no local preview manager; all preview logic is centralized.
    if (owner)
    {
        AMPGMGameplay* GM = Cast<AMPGMGameplay>(GetWorld()->GetAuthGameMode());
        if (GM)
        {
            GM->RequestPreviewCharacterUpdate(owner, ETeam::ECat, (int)selectedCatRace, 0, (int)selectedHat);
        }
        owner->FocusPreviewCamera();
    }
    UMPLogManager::LogDebug(FString::Printf(TEXT("Character preview updated - Race: %d, Hat: %d, Ability: %d"), 
        (int32)selectedCatRace, (int32)selectedHat, (int32)selectedAbility), TEXT("HUDCustomCat"));
}

void UHUDCustomCat::SaveCustomization()
{
    if (owner)
    {
        AMPPlayerState* playerState = Cast<AMPPlayerState>(owner->PlayerState);
        if (playerState)
        {
            playerState->catRace = selectedCatRace;
            playerState->playerSelectedHat = selectedHat;
            playerState->playerSelectedAbility = selectedAbility;
            
            UMPLogManager::LogInfo(FString::Printf(TEXT("Customization saved - Race: %d, Hat: %d, Ability: %d"), 
                (int32)selectedCatRace, (int32)selectedHat, (int32)selectedAbility), TEXT("HUDCustomCat"));
        }
        else
        {
            UMPLogManager::LogError(TEXT("Player state is null, cannot save customization"), TEXT("HUDCustomCat"));
        }
    }
    else
    {
        UMPLogManager::LogError(TEXT("Owner is null, cannot save customization"), TEXT("HUDCustomCat"));
    }
}

void UHUDCustomCat::LoadCustomization()
{
    if (owner)
    {
        AMPPlayerState* playerState = Cast<AMPPlayerState>(owner->PlayerState);
        if (playerState)
        {
            selectedCatRace = playerState->catRace;
            selectedHat = playerState->playerSelectedHat;
            selectedAbility = playerState->playerSelectedAbility;
            
            UMPLogManager::LogDebug(FString::Printf(TEXT("Customization loaded - Race: %d, Hat: %d, Ability: %d"), 
                (int32)selectedCatRace, (int32)selectedHat, (int32)selectedAbility), TEXT("HUDCustomCat"));
        }
        else
        {
            UMPLogManager::LogWarning(TEXT("Player state is null, using default customization"), TEXT("HUDCustomCat"));
        }
    }
    else
    {
        UMPLogManager::LogWarning(TEXT("Owner is null, using default customization"), TEXT("HUDCustomCat"));
    }
}

ETeam UHUDCustomCat::GetCurrentTeam() const
{
    if (owner)
    {
        return owner->GetCurrentTeam();
    }
    return ETeam::ECat;
}

void UHUDCustomCat::OnCatRaceLeftClicked()
{
    UMPLogManager::LogInfo(TEXT("Cat race left button clicked"), TEXT("HUDCustomCat"));
    
    // Cycle through cat races (ECat1, ECat2, ECat3 only)
    int32 currentIndex = (int32)selectedCatRace;
    
    // Skip invalid values (ECatExp, EDiedCat, ECatMax)
    if (currentIndex == (int32)ECatRace::ECatExp || 
        currentIndex == (int32)ECatRace::EDiedCat || 
        currentIndex == (int32)ECatRace::ECatMax)
    {
        currentIndex = (int32)ECatRace::ECat1;
    }
    
    // Cycle to previous valid value
    currentIndex = (currentIndex - 1);
    if (currentIndex < (int32)ECatRace::ECat1)
    {
        currentIndex = (int32)ECatRace::ECat3; // Wrap to last valid value
    }
    
    selectedCatRace = (ECatRace)currentIndex;
    
    UpdateCustomizationOptions();
    UpdateCharacterPreview();
    SaveCustomization();
}

void UHUDCustomCat::OnCatRaceRightClicked()
{
    UMPLogManager::LogInfo(TEXT("Cat race right button clicked"), TEXT("HUDCustomCat"));
    
    // Cycle through cat races (ECat1, ECat2, ECat3 only)
    int32 currentIndex = (int32)selectedCatRace;
    
    // Skip invalid values (ECatExp, EDiedCat, ECatMax)
    if (currentIndex == (int32)ECatRace::ECatExp || 
        currentIndex == (int32)ECatRace::EDiedCat || 
        currentIndex == (int32)ECatRace::ECatMax)
    {
        currentIndex = (int32)ECatRace::ECat1;
    }
    
    // Cycle to next valid value
    currentIndex = (currentIndex + 1);
    if (currentIndex > (int32)ECatRace::ECat3)
    {
        currentIndex = (int32)ECatRace::ECat1; // Wrap to first valid value
    }
    
    selectedCatRace = (ECatRace)currentIndex;
    
    UpdateCustomizationOptions();
    UpdateCharacterPreview();
    SaveCustomization();
}

void UHUDCustomCat::OnHatLeftClicked()
{
    UMPLogManager::LogInfo(TEXT("Hat left button clicked"), TEXT("HUDCustomCat"));
    
    // Cycle through hats (excluding EHatMax)
    int32 currentIndex = (int32)selectedHat;
    int32 maxValidIndex = (int32)EHat::EHatMax - 1; // Exclude EHatMax
    
    currentIndex = (currentIndex - 1);
    if (currentIndex < (int32)EHat::ENone)
    {
        currentIndex = maxValidIndex;
    }
    
    selectedHat = (EHat)currentIndex;
    
    UpdateCustomizationOptions();
    UpdateCharacterPreview();
    SaveCustomization();
}

void UHUDCustomCat::OnHatRightClicked()
{
    UMPLogManager::LogInfo(TEXT("Hat right button clicked"), TEXT("HUDCustomCat"));
    
    // Cycle through hats (excluding EHatMax)
    int32 currentIndex = (int32)selectedHat;
    int32 maxValidIndex = (int32)EHat::EHatMax - 1; // Exclude EHatMax
    
    currentIndex = (currentIndex + 1);
    if (currentIndex > maxValidIndex)
    {
        currentIndex = (int32)EHat::ENone;
    }
    
    selectedHat = (EHat)currentIndex;
    
    UpdateCustomizationOptions();
    UpdateCharacterPreview();
    SaveCustomization();
}

void UHUDCustomCat::OnAbilityLeftClicked()
{
    UMPLogManager::LogInfo(TEXT("Ability left button clicked"), TEXT("HUDCustomCat"));
    
    // Cycle through abilities (excluding EAbilityMax)
    int32 currentIndex = (int32)selectedAbility;
    int32 maxValidIndex = (int32)EAbility::EAbilityMax - 1; // Exclude EAbilityMax
    
    currentIndex = (currentIndex - 1);
    if (currentIndex < (int32)EAbility::ENone)
    {
        currentIndex = maxValidIndex;
    }
    
    selectedAbility = (EAbility)currentIndex;
    
    UpdateCustomizationOptions();
    UpdateCharacterPreview();
    SaveCustomization();
}

void UHUDCustomCat::OnAbilityRightClicked()
{
    UMPLogManager::LogInfo(TEXT("Ability right button clicked"), TEXT("HUDCustomCat"));
    
    // Cycle through abilities (excluding EAbilityMax)
    int32 currentIndex = (int32)selectedAbility;
    int32 maxValidIndex = (int32)EAbility::EAbilityMax - 1; // Exclude EAbilityMax
    
    currentIndex = (currentIndex + 1);
    if (currentIndex > maxValidIndex)
    {
        currentIndex = (int32)EAbility::ENone;
    }
    
    selectedAbility = (EAbility)currentIndex;
    
    UpdateCustomizationOptions();
    UpdateCharacterPreview();
    SaveCustomization();
}

void UHUDCustomCat::UpdateUI()
{
    // Update title
    if (titleText)
    {
        titleText->SetText(GetLocalizedText(customCatTitleKey));
    }
    
    // Update labels
    if (raceLabel)
    {
        raceLabel->SetText(GetLocalizedText(raceLabelKey));
    }
    
    if (hatLabel)
    {
        hatLabel->SetText(GetLocalizedText(hatLabelKey));
    }
    
    if (abilityLabel)
    {
        abilityLabel->SetText(GetLocalizedText(abilityLabelKey));
    }
    
    // Update customization options
    UpdateCustomizationOptions();
    UpdateCharacterPreview();
}

FText UHUDCustomCat::GetCatRaceDisplayText(ECatRace catRace) const
{
    switch (catRace)
    {
    case ECatRace::ECat1:
        return FText::FromString(TEXT("Cat 1"));
    case ECatRace::ECat2:
        return FText::FromString(TEXT("Cat 2"));
    case ECatRace::ECat3:
        return FText::FromString(TEXT("Cat 3"));
    default:
        return FText::FromString(TEXT("Unknown"));
    }
}

FText UHUDCustomCat::GetHatDisplayText(EHat hat) const
{
    switch (hat)
    {
    case EHat::ENone:
        return FText::FromString(TEXT("None"));
    case EHat::ECap:
        return FText::FromString(TEXT("Cap"));
    case EHat::ECrown:
        return FText::FromString(TEXT("Crown"));
    case EHat::EHat1:
        return FText::FromString(TEXT("Hat 1"));
    case EHat::EHat2:
        return FText::FromString(TEXT("Hat 2"));
    case EHat::EHat3:
        return FText::FromString(TEXT("Hat 3"));
    default:
        return FText::FromString(TEXT("Unknown"));
    }
}

FText UHUDCustomCat::GetAbilityDisplayText(EAbility ability) const
{
    switch (ability)
    {
    case EAbility::ENone:
        return FText::FromString(TEXT("None"));
    case EAbility::EAbility1:
        return FText::FromString(TEXT("Ability 1"));
    case EAbility::EAbility2:
        return FText::FromString(TEXT("Ability 2"));
    case EAbility::EAbility3:
        return FText::FromString(TEXT("Ability 3"));
    default:
        return FText::FromString(TEXT("Unknown"));
    }
}