#pragma once

// [Meow-Phone Project]
//
// This manager handles the spawning and management of preview characters, such as those
// seen in the lobby or character customization screens. It ensures that each player has a
// dedicated slot and that their chosen character appearance is correctly displayed.
//
// How to utilize in Blueprint:
// 1. This manager is created and owned by the Game Mode (`AMPGMGameplay`).
// 2. The Game Mode is responsible for providing the spawn locations and rotations for the preview characters by calling `SetPreviewTransforms`. These transforms are typically defined as properties on the Game Mode Blueprint itself and are gathered from the level.
// 3. The character customization UI (`HUDCustomCat`, `HUDCustomHuman`) is the primary user of this manager. When a player changes their character's appearance (e.g., selects a new hat), the UI calls `RequestPreviewCharacterUpdate` on this manager.
// 4. `AssignPreviewSlot` and `FreePreviewSlot` are called by the lobby or customization logic as players enter and leave the customization state.
//
// Necessary things to define:
// - The Game Mode that owns this manager must have a way to define the `characterPreviewLocations` and `characterPreviewRotations`. A common approach is to have `TargetPoint` actors in the level and have the Game Mode collect their transforms at runtime to pass into this manager.
//
// How it interacts with other classes:
// - UManagerMP: Inherits from the base manager class.
// - AMPGMGameplay: The Game Mode owns this manager and provides it with the necessary transform data for spawning the preview actors. It also drives the overall state.
// - AMPControllerPlayer: Used as a key to associate a player with a specific preview slot and character.
// - HUDs (e.g., `HUDCustomCat`): The UI sends requests to this manager to update a player's preview character whenever a customization option is changed.
// - Character Factories (e.g., `UFactoryCat`, `UFactoryHuman`): In its C++ implementation, this manager uses the character factories to spawn the actual preview character actors based on the requested team, race, profession, and hat.

#include "CoreMinimal.h"
#include "ManagerMP.h"
#include "ManagerPreview.generated.h"

class AMPGMGameplay;
class AMPControllerPlayer;
class AActor;
enum class ETeam : uint8;

UCLASS()
class UManagerPreview : public UManagerMP
{
    GENERATED_BODY()

public:
    virtual void InitializeManager(AMPGMGameplay* inGameMode) override;

    // Preview system API
    int32 GetPlayerPreviewSlot(AMPControllerPlayer* Player) const;
    void RequestPreviewCharacterUpdate(AMPControllerPlayer* Player, ETeam Team, int CatRace, int HumanProfession, int Hat);
    void AssignPreviewSlot(AMPControllerPlayer* Player);
    void FreePreviewSlot(AMPControllerPlayer* Player);
    void DestroyAllPreviewCharacters();
    void SpawnOrReplacePreviewCharacter(AMPControllerPlayer* Player, ETeam Team, int32 SlotIdx, int CatRace = 0, int HumanProfession = 0, int Hat = 0);

protected:
    UPROPERTY()
    AMPGMGameplay* GameMode;

    // Preview system for lobby customization
    UPROPERTY(VisibleAnywhere, Category = "Preview")
    TArray<FVector> characterPreviewLocations;
    UPROPERTY(VisibleAnywhere, Category = "Preview")
    TArray<FRotator> characterPreviewRotations;

    UPROPERTY(VisibleAnywhere, Category = "Preview")
    TArray<TObjectPtr<AActor>> previewCharacters;
    UPROPERTY(VisibleAnywhere, Category = "Preview")
    TArray<TObjectPtr<AMPControllerPlayer>> previewSlotOwners;

    
public:
    TArray<TObjectPtr<AActor>> GetPreviewCharacters() const { return previewCharacters; }

    // Preview management
    int32 FindFreePreviewSlot() const;
    int32 FindPlayerPreviewSlot(AMPControllerPlayer* Player) const;
    void DestroyPreviewCharacter(int32 SlotIdx);

public:
    // Allow GM to set locations/rotations from its own properties
    void SetPreviewTransforms(const TArray<FVector>& Locations, const TArray<FRotator>& Rotations);
}; 