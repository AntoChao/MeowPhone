#pragma once

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