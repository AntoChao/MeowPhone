#include "MPEnvActorCompCage.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "HighLevel/MPLogManager.h"
#include "HighLevel/MPGS.h"
#include "MPActor/Character/MPCharacter.h"
#include "MPActor/Character/MPCharacterCat.h"
#include "MPActor/Character/MPCharacterHuman.h"

AMPEnvActorCompCage::AMPEnvActorCompCage()
{
    envActorTag = EEnvActor::ECustom;
    envActorType = EEnvActorType::EDirectInteract;

    PrimaryActorTick.bCanEverTick = false;
}

void AMPEnvActorCompCage::BeginPlay()
{
    Super::BeginPlay();
}

void AMPEnvActorCompCage::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMPEnvActorCompCage, isOccupiedReplicated);
}

bool AMPEnvActorCompCage::IsInteractable(AMPCharacter* targetActor)
{
    if (!targetActor) { return false; }

    // Only humans can interact with cages
    AMPCharacterHuman* humanActor = Cast<AMPCharacterHuman>(targetActor);
    if (!humanActor) { return false; }

    // Check if human is holding a cat
    if (!humanActor->IsHoldingCat()) { return false; }

    // Cage is always available for use (removed occupancy check)
    return true;
}

FText AMPEnvActorCompCage::GetInteractHintText(AMPCharacter* targetActor)
{
    return Super::GetInteractHintText(targetActor);
}

void AMPEnvActorCompCage::ApplyInteractEffectDirect(AMPCharacter* targetActor)
{
    if (!HasAuthority()) { return; }

    AMPCharacterHuman* humanActor = Cast<AMPCharacterHuman>(targetActor);
    if (!humanActor) { return; }

    // Get the cat being held by the human
    AMPCharacterCat* heldCat = humanActor->GetHeldCat();
    if (!heldCat) { return; }

    // Catch the cat
    CatchCat(humanActor, heldCat);
}

bool AMPEnvActorCompCage::CanAcceptCat(AMPCharacter* humanActor)
{
    return IsInteractable(humanActor);
}

void AMPEnvActorCompCage::CatchCat(AMPCharacter* humanActor, AMPCharacterCat* catToCatch)
{
    if (!HasAuthority()) { return; }
    if (!humanActor || !catToCatch) { return; }

    // Check if cat is still valid and not being destroyed
    if (!IsValid(catToCatch) || catToCatch->IsPendingKill())
    {
        UMPLogManager::LogWarning(TEXT("ERROR: Trying to catch invalid or destroyed cat!"), TEXT("MPEnvActorCompCage"));
        return;
    }

    AMPCharacterHuman* human = Cast<AMPCharacterHuman>(humanActor);
    if (!human) { return; }

    // Verify that the human is actually holding this specific cat
    if (!human->IsHoldingCat() || human->GetHeldCat() != catToCatch)
    {
        UMPLogManager::LogWarning(TEXT("ERROR: Human tried to catch a cat they're not holding!"), TEXT("MPEnvActorCompCage"));
        return;
    }

    // Check if the cat is a player or AI
    bool isPlayerCat = false;
    AMPControllerPlayer* catController = Cast<AMPControllerPlayer>(catToCatch->GetController());
    if (catController)
    {
        isPlayerCat = true;
    }

    // Update human progression based on cat type
    AGameModeBase* gameMode = UGameplayStatics::GetGameMode(GetWorld());
    AMPGMGameplay* mpGameMode = Cast<AMPGMGameplay>(gameMode);
    
    if (mpGameMode)
    {
        AMPGS* gameState = Cast<AMPGS>(mpGameMode->GameState);
        if (gameState)
        {
            if (isPlayerCat)
            {
                // Player cat caught - update human progression
                gameState->UpdateHumanProgression(1);
                UMPLogManager::LogInfo(TEXT("Player Cat Caught! Human Progression Updated"), TEXT("MPEnvActorCompCage"));
            }
            else
            {
                // AI cat caught - penalize human health
                human->TakeHealthDamage(1);
                UMPLogManager::LogInfo(TEXT("AI Cat Caught! Human Health Penalized"), TEXT("MPEnvActorCompCage"));
            }
        }
    }

    // Release the cat from human's hold
    human->PutCatInCage();

    // Destroy the cat (or make it disappear)
    catToCatch->Destroy();

    // Cage remains available for reuse (don't mark as occupied)
    // isOccupied = false;
    // isOccupiedReplicated = false;

    UMPLogManager::LogInfo(FString::Printf(TEXT("Cat caught in cage! IsPlayer: %s"), isPlayerCat ? TEXT("Yes") : TEXT("No")), TEXT("MPEnvActorCompCage"));
}

void AMPEnvActorCompCage::OnRep_IsOccupied()
{
    isOccupied = isOccupiedReplicated;
} 