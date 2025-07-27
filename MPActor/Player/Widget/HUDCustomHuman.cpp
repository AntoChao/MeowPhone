#include "HUDCustomHuman.h"
#include "Components/PanelWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Viewport.h"
#include "../MPControllerPlayer.h"
#include "../MPPlayerState.h"
#include "../../../HighLevel/Managers/ManagerLog.h"
#include "../../../HighLevel/MPGMGameplay.h"

UHUDCustomHuman::UHUDCustomHuman()
{
    // Initialize default values
    selectedHumanProfession = EHumanProfession::EHuman1;
    selectedHat = EHat::ENone;
    selectedAbility = EAbility::ENone;
}

void UHUDCustomHuman::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind button events
    if (characterLeftButton)
    {
        characterLeftButton->OnClicked.AddDynamic(this, &UHUDCustomHuman::OnHumanProfessionLeftClicked);
    }
    
    if (characterRightButton)
    {
        characterRightButton->OnClicked.AddDynamic(this, &UHUDCustomHuman::OnHumanProfessionRightClicked);
    }
    
    if (hatLeftButton)
    {
        hatLeftButton->OnClicked.AddDynamic(this, &UHUDCustomHuman::OnHatLeftClicked);
    }
    
    if (hatRightButton)
    {
        hatRightButton->OnClicked.AddDynamic(this, &UHUDCustomHuman::OnHatRightClicked);
    }
    
    if (abilityLeftButton)
    {
        abilityLeftButton->OnClicked.AddDynamic(this, &UHUDCustomHuman::OnAbilityLeftClicked);
    }
    
    if (abilityRightButton)
    {
        abilityRightButton->OnClicked.AddDynamic(this, &UHUDCustomHuman::OnAbilityRightClicked);
    }

    // Load current customization from player state
    LoadCustomization();
    
    // Initialize UI
    UpdateUI();
    
    UManagerLog::LogInfo(TEXT("Human Customization HUD constructed and initialized"), TEXT("HUDCustomHuman"));
}

void UHUDCustomHuman::NativeDestruct()
{
    // Unbind button events
    if (characterLeftButton)
    {
        characterLeftButton->OnClicked.RemoveAll(this);
    }
    
    if (characterRightButton)
    {
        characterRightButton->OnClicked.RemoveAll(this);
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
    
    UManagerLog::LogInfo(TEXT("Human Customization HUD destroyed"), TEXT("HUDCustomHuman"));
}

void UHUDCustomHuman::UpdateCustomizationOptions()
{
    // Update character text
    if (characterText)
    {
        characterText->SetText(GetHumanProfessionDisplayText(selectedHumanProfession));
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
    
    UManagerLog::LogDebug(FString::Printf(TEXT("Updated customization options - Profession: %d, Hat: %d, Ability: %d"), 
        (int32)selectedHumanProfession, (int32)selectedHat, (int32)selectedAbility), TEXT("HUDCustomHuman"));
}

void UHUDCustomHuman::UpdateCharacterPreview()
{
    // Update the main world preview character via GameMode and focus the preview camera via PlayerController.
    // There is no local preview manager; all preview logic is centralized.
    if (owner)
    {
        AMPGMGameplay* GM = Cast<AMPGMGameplay>(GetWorld()->GetAuthGameMode());
        if (GM)
        {
            GM->RequestPreviewCharacterUpdate(owner, ETeam::EHuman, 0, (int)selectedHumanProfession, (int)selectedHat);
        }
        owner->FocusPreviewCamera();
    }
    UManagerLog::LogDebug(FString::Printf(TEXT("Character preview updated - Profession: %d, Hat: %d, Ability: %d"), 
        (int32)selectedHumanProfession, (int32)selectedHat, (int32)selectedAbility), TEXT("HUDCustomHuman"));
}

void UHUDCustomHuman::SaveCustomization()
{
    if (owner)
    {
        AMPPlayerState* playerState = Cast<AMPPlayerState>(owner->PlayerState);
        if (playerState)
        {
            playerState->humanProfession = selectedHumanProfession;
            playerState->playerSelectedHat = selectedHat;
            playerState->playerSelectedAbility = selectedAbility;
            
            UManagerLog::LogInfo(FString::Printf(TEXT("Customization saved - Profession: %d, Hat: %d, Ability: %d"), 
                (int32)selectedHumanProfession, (int32)selectedHat, (int32)selectedAbility), TEXT("HUDCustomHuman"));
        }
        else
        {
            UManagerLog::LogError(TEXT("Player state is null, cannot save customization"), TEXT("HUDCustomHuman"));
        }
    }
    else
    {
        UManagerLog::LogError(TEXT("Owner is null, cannot save customization"), TEXT("HUDCustomHuman"));
    }
}

void UHUDCustomHuman::LoadCustomization()
{
    if (owner)
    {
        AMPPlayerState* playerState = Cast<AMPPlayerState>(owner->PlayerState);
        if (playerState)
        {
            selectedHumanProfession = playerState->humanProfession;
            selectedHat = playerState->playerSelectedHat;
            selectedAbility = playerState->playerSelectedAbility;
            
            UManagerLog::LogDebug(FString::Printf(TEXT("Customization loaded - Profession: %d, Hat: %d, Ability: %d"), 
                (int32)selectedHumanProfession, (int32)selectedHat, (int32)selectedAbility), TEXT("HUDCustomHuman"));
        }
        else
        {
            UManagerLog::LogWarning(TEXT("Player state is null, using default customization"), TEXT("HUDCustomHuman"));
        }
    }
    else
    {
        UManagerLog::LogWarning(TEXT("Owner is null, using default customization"), TEXT("HUDCustomHuman"));
    }
}

ETeam UHUDCustomHuman::GetCurrentTeam() const
{
    if (owner)
    {
        return owner->GetCurrentTeam();
    }
    return ETeam::EHuman;
}

void UHUDCustomHuman::OnHumanProfessionLeftClicked()
{
    UManagerLog::LogInfo(TEXT("Human profession left button clicked"), TEXT("HUDCustomHuman"));
    
    // Cycle through human professions (EHuman1, EHuman2, EHuman3 only)
    int32 currentIndex = (int32)selectedHumanProfession;
    
    // Skip invalid values (EHumanExp, EDiedHuman, EHumanMax)
    if (currentIndex == (int32)EHumanProfession::EHumanExp || 
        currentIndex == (int32)EHumanProfession::EDiedHuman || 
        currentIndex == (int32)EHumanProfession::EHumanMax)
    {
        currentIndex = (int32)EHumanProfession::EHuman1;
    }
    
    // Cycle to previous valid value
    currentIndex = (currentIndex - 1);
    if (currentIndex < (int32)EHumanProfession::EHuman1)
    {
        currentIndex = (int32)EHumanProfession::EHuman3; // Wrap to last valid value
    }
    
    selectedHumanProfession = (EHumanProfession)currentIndex;
    
    UpdateCustomizationOptions();
    UpdateCharacterPreview();
    SaveCustomization();
}

void UHUDCustomHuman::OnHumanProfessionRightClicked()
{
    UManagerLog::LogInfo(TEXT("Human profession right button clicked"), TEXT("HUDCustomHuman"));
    
    // Cycle through human professions (EHuman1, EHuman2, EHuman3 only)
    int32 currentIndex = (int32)selectedHumanProfession;
    
    // Skip invalid values (EHumanExp, EDiedHuman, EHumanMax)
    if (currentIndex == (int32)EHumanProfession::EHumanExp || 
        currentIndex == (int32)EHumanProfession::EDiedHuman || 
        currentIndex == (int32)EHumanProfession::EHumanMax)
    {
        currentIndex = (int32)EHumanProfession::EHuman1;
    }
    
    // Cycle to next valid value
    currentIndex = (currentIndex + 1);
    if (currentIndex > (int32)EHumanProfession::EHuman3)
    {
        currentIndex = (int32)EHumanProfession::EHuman1; // Wrap to first valid value
    }
    
    selectedHumanProfession = (EHumanProfession)currentIndex;
    
    UpdateCustomizationOptions();
    UpdateCharacterPreview();
    SaveCustomization();
}

void UHUDCustomHuman::OnHatLeftClicked()
{
    UManagerLog::LogInfo(TEXT("Hat left button clicked"), TEXT("HUDCustomHuman"));
    
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

void UHUDCustomHuman::OnHatRightClicked()
{
    UManagerLog::LogInfo(TEXT("Hat right button clicked"), TEXT("HUDCustomHuman"));
    
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

void UHUDCustomHuman::OnAbilityLeftClicked()
{
    UManagerLog::LogInfo(TEXT("Ability left button clicked"), TEXT("HUDCustomHuman"));
    
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

void UHUDCustomHuman::OnAbilityRightClicked()
{
    UManagerLog::LogInfo(TEXT("Ability right button clicked"), TEXT("HUDCustomHuman"));
    
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

void UHUDCustomHuman::UpdateUI()
{
    // Update title
    if (titleText)
    {
        titleText->SetText(GetLocalizedText(customHumanTitleKey));
    }
    
    // Update labels
    if (characterLabel)
    {
        characterLabel->SetText(GetLocalizedText(characterLabelKey));
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

FText UHUDCustomHuman::GetHumanProfessionDisplayText(EHumanProfession profession) const
{
    switch (profession)
    {
    case EHumanProfession::EHuman1:
        return FText::FromString(TEXT("Human 1"));
    case EHumanProfession::EHuman2:
        return FText::FromString(TEXT("Human 2"));
    case EHumanProfession::EHuman3:
        return FText::FromString(TEXT("Human 3"));
    default:
        return FText::FromString(TEXT("Unknown"));
    }
}

FText UHUDCustomHuman::GetHatDisplayText(EHat hat) const
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

FText UHUDCustomHuman::GetAbilityDisplayText(EAbility ability) const
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