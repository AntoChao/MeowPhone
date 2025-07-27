#include "ManagerPreview.h"
#include "../MPGMGameplay.h"
#include "../../MPActor/Player/MPControllerPlayer.h"
#include "../Factory/FactoryCat.h"
#include "../Factory/FactoryHuman.h"
#include "../../CommonEnum.h"
#include "Engine/World.h"

void UManagerPreview::InitializeManager(AMPGMGameplay* inGameMode)
{
    Super::InitializeManager(inGameMode);
    if (gameMode)
    {
        previewCharacters.SetNum(8);
        previewSlotOwners.SetNum(8);
    }
}

void UManagerPreview::SetPreviewTransforms(const TArray<FVector>& Locations, const TArray<FRotator>& Rotations)
{
    characterPreviewLocations = Locations;
    characterPreviewRotations = Rotations;
}

int32 UManagerPreview::GetPlayerPreviewSlot(AMPControllerPlayer* Player) const
{
    return FindPlayerPreviewSlot(Player);
}

void UManagerPreview::RequestPreviewCharacterUpdate(AMPControllerPlayer* Player, ETeam Team, int CatRace, int HumanProfession, int Hat)
{
    int32 slot = FindPlayerPreviewSlot(Player);
    if (slot != -1) {
        SpawnOrReplacePreviewCharacter(Player, Team, slot, CatRace, HumanProfession, Hat);
    }
}

void UManagerPreview::AssignPreviewSlot(AMPControllerPlayer* Player)
{
    int32 slot = FindFreePreviewSlot();
    if (slot != -1 && Player) {
        previewSlotOwners[slot] = Player;
        Player->PreviewSlotIndex = slot;
    }
}

void UManagerPreview::FreePreviewSlot(AMPControllerPlayer* Player)
{
    int32 slot = FindPlayerPreviewSlot(Player);
    if (slot != -1) {
        previewSlotOwners[slot] = nullptr;
        DestroyPreviewCharacter(slot);
        if(Player)
        {
            Player->PreviewSlotIndex = -1;
        }
    }
}

void UManagerPreview::DestroyAllPreviewCharacters()
{
    for (int32 i = 0; i < previewCharacters.Num(); ++i) {
        DestroyPreviewCharacter(i);
    }
}

void UManagerPreview::SpawnOrReplacePreviewCharacter(AMPControllerPlayer* Player, ETeam Team, int32 SlotIdx, int CatRace, int HumanProfession, int Hat)
{
    if (!gameMode) return;

    DestroyPreviewCharacter(SlotIdx);
    FVector loc = characterPreviewLocations.IsValidIndex(SlotIdx) ? characterPreviewLocations[SlotIdx] : FVector::ZeroVector;
    FRotator rot = characterPreviewRotations.IsValidIndex(SlotIdx) ? characterPreviewRotations[SlotIdx] : FRotator::ZeroRotator;
    AActor* newChar = nullptr;
    if (Team == ETeam::ECat && gameMode->catFactoryInstance) {
        newChar = gameMode->catFactoryInstance->SpawnMPActor(CatRace, loc, rot);
    }
    else if (Team == ETeam::EHuman && gameMode->humanFactoryInstance) {
        newChar = gameMode->humanFactoryInstance->SpawnMPActor(HumanProfession, loc, rot);
    }
    if (newChar) {
        previewCharacters[SlotIdx] = newChar;
        // Set hat/appearance if needed (not implemented here)
    }
}

int32 UManagerPreview::FindFreePreviewSlot() const
{
    for (int32 i = 0; i < previewSlotOwners.Num(); ++i)
        if (previewSlotOwners[i] == nullptr)
            return i;
    return -1;
}

int32 UManagerPreview::FindPlayerPreviewSlot(AMPControllerPlayer* Player) const
{
    for (int32 i = 0; i < previewSlotOwners.Num(); ++i)
        if (previewSlotOwners[i] == Player)
            return i;
    return -1;
}

void UManagerPreview::DestroyPreviewCharacter(int32 SlotIdx)
{
    if (previewCharacters.IsValidIndex(SlotIdx) && previewCharacters[SlotIdx]) {
        previewCharacters[SlotIdx]->Destroy();
        previewCharacters[SlotIdx] = nullptr;
    }
} 