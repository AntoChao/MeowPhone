#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PreviewManager.generated.h"

class AMPGMGameplay;
class AMPControllerPlayer;
class AActor;
enum class ETeam : uint8;

UCLASS()
class UPreviewManager : public UObject
{
    GENERATED_BODY()

public:
    void Initialize(AMPGMGameplay* InGameMode);

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

    // Preview management
    int32 FindFreePreviewSlot() const;
    int32 FindPlayerPreviewSlot(AMPControllerPlayer* Player) const;
    void DestroyPreviewCharacter(int32 SlotIdx);
    
public:
    // Allow GM to set locations/rotations from its own properties
    void SetPreviewTransforms(const TArray<FVector>& Locations, const TArray<FRotator>& Rotations);
}; 