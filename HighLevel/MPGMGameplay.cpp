#include "MPGMGameplay.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

#include "../CommonEnum.h"

#include "MPGI.h"
#include "MPGS.h"
#include "HighLevel/MPLogManager.h"

#include "Factory/FactoryHuman.h"
#include "Factory/FactoryCat.h"
#include "Factory/FactoryEnvironment.h"
#include "Factory/FactoryItem.h"
#include "Factory/FactoryAbility.h"
#include "Factory/FactoryAIController.h"

#include "../MPActor/Player/MPControllerPlayer.h"
#include "../MPActor/Player/MPPlayerState.h"
#include "../MPActor/Character/MPCharacter.h"
#include "../MPActor/Item/MPItem.h"
#include "../MPActor/AI/MPAIController.h"
#include "../MPActor/AI/MPAISystemManager.h"
#include "../MPActor/Ability/MPAbility.h"
#include "../MPActor/EnvActor/MPEnvActorComp.h"
#include "../MPActor/EnvActor/MPEnvActorCompPushable.h"
#include "MPActor/Character/MPCharacterHuman.h"
#include "MPActor/Character/MPCharacterCat.h"
#include "Camera/CameraComponent.h"
#include "Managers/LobbyManager.h"
#include "Managers/PreviewManager.h"
#include "Managers/AIControllerManager.h"
#include "Managers/MatchManager.h"

AMPGMGameplay::AMPGMGameplay()
{
	LobbyManager = NewObject<ULobbyManager>(this);
	if (LobbyManager)
	{
		LobbyManager->Initialize(this);
	}
	PreviewManager = NewObject<UPreviewManager>(this);
	if (PreviewManager)
	{
		PreviewManager->Initialize(this);
	}
	MatchManager = NewObject<UMatchManager>(this);
    if (MatchManager)
    {
        MatchManager->Initialize(this);
	}
	AIControllerManager = NewObject<UAIControllerManager>(this);
	if (AIControllerManager)
	{
		AIControllerManager->Initialize(this);
	}
}

AMPGMGameplay::~AMPGMGameplay()
{
	// Clean up all timers to prevent memory leaks
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(readyTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(restartLobbyTimerHandle);
	}
}

// gameplay common

/* PostLogin
* called when player join the lobby
* no need to have body, just attach the client lobby HUD
*/
void AMPGMGameplay::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);

	UMPLogManager::LogInfo(TEXT("One player log in"), TEXT("MPGMGameplay"));
	
	SetupFactoryInstances();
	AMPControllerPlayer* curPlayer = Cast<AMPControllerPlayer>(newPlayer);
	
	if (curPlayer)
	{
		curPlayer->InitializePS(allPlayersControllers.Num());

		// Assign preview slot and spawn preview character
		PreviewManager->AssignPreviewSlot(curPlayer);
		int32 slot = curPlayer->PreviewSlotIndex;
		ETeam team = ETeam::ENone;
		AMPPlayerState* curState = Cast<AMPPlayerState>(curPlayer->PlayerState);
		if (curState) {
			team = curState->playerTeam;
		}
		// Default to human preview if no team yet
		if (team == ETeam::ENone) team = ETeam::EHuman;
		PreviewManager->SpawnOrReplacePreviewCharacter(curPlayer, team, slot, 0, 0, 0);

		if (singlePlayerDebugMode)
		{
			// Single player debug mode: Force team assignment and start game immediately
			AMPPlayerState* curState = Cast<AMPPlayerState>(curPlayer->PlayerState);
			if (curState)
			{
				curState->playerTeam = debugPlayer;
				curState->isPlayerReady = true;
			}

			// Instantly start gameplay once the first (and only) player joins
			if (allPlayersControllers.Num() == 0)
			{
				allPlayersControllers.Add(curPlayer);
				StartGame();
			}
		}
		else if (multiplayerDebugMode)
		{
			// Multiplayer debug mode: Allow lobby but with relaxed requirements
			allPlayersControllers.Add(curPlayer);
			
			// If lobby is already active, assign team immediately
			if (theGameState && theGameState->curGameplayStatus == EGPStatus::ELobby)
			{
				LobbyManager->AutoAssignTeams();
				curPlayer->AttachHUD(EHUDType::ELobby, 0);
				curPlayer->LobbyStartUpdate();
			}
			else
			{
				// Lobby not started yet, just attach HUD
				curPlayer->AttachHUD(EHUDType::ELobby, 0);
			}
		}
		else
		{
			// Normal flow: add to lobby and handle team assignment
			allPlayersControllers.Add(curPlayer);
			
			// If lobby is already active, assign team immediately
			if (theGameState && theGameState->curGameplayStatus == EGPStatus::ELobby)
			{
				LobbyManager->AutoAssignTeams();
				curPlayer->AttachHUD(EHUDType::ELobby, 0);
				curPlayer->LobbyStartUpdate();
			}
			else
			{
				// Lobby not started yet, just attach HUD
				curPlayer->AttachHUD(EHUDType::ELobby, 0);
			}
		}
	}
}

void AMPGMGameplay::Logout(AController* exiting)
{
	Super::Logout(exiting);

	UMPLogManager::LogInfo(TEXT("One player log out"), TEXT("MPGMGameplay"));

	AMPControllerPlayer* curPlayer = Cast<AMPControllerPlayer>(exiting);
	if (curPlayer)
	{
		PreviewManager->FreePreviewSlot(curPlayer);
		// Check if the exiting player was a cat player
		AMPPlayerState* playerState = Cast<AMPPlayerState>(curPlayer->PlayerState);
		if (playerState && playerState->playerTeam == ETeam::ECat)
		{
			// Check if this cat was being held by any human
			AMPCharacterCat* catCharacter = Cast<AMPCharacterCat>(curPlayer->GetPawn());
			if (catCharacter)
			{
				// Find any human holding this cat and release them
				for (AMPControllerPlayer* eachPlayer : allPlayersControllers)
				{
					AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
					if (eachState && eachState->playerTeam == ETeam::EHuman)
					{
						AMPCharacterHuman* humanChar = Cast<AMPCharacterHuman>(eachPlayer->GetPawn());
						if (humanChar && humanChar->IsHoldingCat() && humanChar->GetHeldCat() == catCharacter)
						{
							humanChar->ForceReleaseCat();
							UMPLogManager::LogWarning(TEXT("Cat player disconnected while being held! Human released."), TEXT("MPGMGameplay"));
							break;
						}
					}
				}
			}
			
			// Update human team objective - reduce total cat players
			if (theGameState && theGameState->totalCatPlayers > 0)
			{
				theGameState->totalCatPlayers--;
				UMPLogManager::LogInfo(FString::Printf(TEXT("Cat player disconnected! Human objective updated: %d cat players remaining"), theGameState->totalCatPlayers), TEXT("MPGMGameplay"));
				
				// Recalculate percentage
				if (theGameState->totalCatPlayers > 0)
				{
					theGameState->caughtCatsPercentage = (float)theGameState->caughtCats / (float)theGameState->totalCatPlayers;
				}
				else
				{
					theGameState->caughtCatsPercentage = 1.0f; // All cats disconnected, human team wins
				}
			}
		}
		
		allPlayersControllers.Remove(curPlayer);
		RemoveControlledCharacters(curPlayer);
	}

	bool isGameEnd = MatchManager->CheckIfGameEnd();
}

void AMPGMGameplay::BeginPlay()
{
	Super::BeginPlay();

	InitializeGameState();
	
	if (PreviewManager)
	{
		PreviewManager->SetPreviewTransforms(characterPreviewLocations, characterPreviewRotations);
	}

	// Single player debug mode skips lobby entirely
	if (singlePlayerDebugMode)
	{
		UMPLogManager::LogInfo(TEXT("Single Player Debug Mode - Skipping lobby"), TEXT("MPGMGameplay"));
	}
	else
	{
		LobbyManager->StartLobby();
	}
	
	SetupFactoryInstances();
}

void AMPGMGameplay::RemoveControlledCharacters(AMPControllerPlayer* aPlayer)
{
	if (aPlayer)
	{
		TArray<AMPCharacter*> toRemoveCharacters;
		for (AMPCharacter* eachCharacter : allPlayerCharacters)
		{
			AMPControllerPlayer* eachMPController = Cast<AMPControllerPlayer>(eachCharacter->GetController());

			if (eachMPController && eachMPController == aPlayer)
			{
				toRemoveCharacters.Add(eachCharacter);
			}
		}

		for (AMPCharacter* eachToRemoveCharacter : toRemoveCharacters)
		{
			allPlayerCharacters.Remove(eachToRemoveCharacter);
		}
	}
}

// game state initialize
void AMPGMGameplay::InitializeGameState() 
{
	theGameState = Cast<AMPGS>(GameState);
}

// factories
void AMPGMGameplay::SetupFactoryInstances()
{
	if (!aiControllerFactoryInstance && aiControllerFactoryClass)
    {
        aiControllerFactoryInstance = NewObject<UFactoryAIController>(this, aiControllerFactoryClass);
        aiControllerFactoryInstance->SetGameWorld(GetWorld());
    }
	if (!humanFactoryInstance && humanFactoryClass)
    {
        humanFactoryInstance = NewObject<UFactoryHuman>(this, humanFactoryClass);
        humanFactoryInstance->SetGameWorld(GetWorld());
    }
    if (!catFactoryInstance && catFactoryClass)
    {
        catFactoryInstance = NewObject<UFactoryCat>(this, catFactoryClass);
        catFactoryInstance->SetGameWorld(GetWorld());
    }
    if (!environmentFactoryInstance && environmentFactoryClass)
    {
        environmentFactoryInstance = NewObject<UFactoryEnvironment>(this, environmentFactoryClass);
        environmentFactoryInstance->SetGameWorld(GetWorld());
    }
    if (!itemFactoryInstance && itemFactoryClass)
    {
        itemFactoryInstance = NewObject<UFactoryItem>(this, itemFactoryClass);
        itemFactoryInstance->SetGameWorld(GetWorld());
    }
    if (!abilityFactoryInstance && abilityFactoryClass)
    {
        abilityFactoryInstance = NewObject<UFactoryAbility>(this, abilityFactoryClass);
        abilityFactoryInstance->SetGameWorld(GetWorld());
    }
}

	// factory spawn
AMPEnvActorComp* AMPGMGameplay::SpawnEnvironment(EEnvActor envTag, FVector spawnLocation, FRotator spawnRotation)
{
	if (environmentFactoryInstance)
	{
		AActor* spawnActor = environmentFactoryInstance->SpawnMPActor(static_cast<int>(envTag), spawnLocation, spawnRotation);

		if (spawnActor)
		{
			AMPEnvActorComp* envSpawn = Cast<AMPEnvActorComp>(spawnActor);
			if (envSpawn)
			{
				return envSpawn;
			}
		}
	}
	return nullptr;
}
AMPItem* AMPGMGameplay::SpawnItem(EItem itemTag, FVector spawnLocation, FRotator spawnRotation)
{
	if (itemFactoryInstance)
	{
		AActor* spawnActor = itemFactoryInstance->SpawnMPActor(static_cast<int>(itemTag), spawnLocation, spawnRotation);

		if (spawnActor)
		{
			AMPItem* itemSpawn = Cast<AMPItem>(spawnActor);
			if (itemSpawn)
			{
				return itemSpawn;
			}
		}
	}
	return nullptr;
}
UMPAbility* AMPGMGameplay::SpawnAbility(AActor* abilityOwner, EAbility abilityTag)
{
    if (abilityFactoryInstance)
    {
		AActor* spawnActor = abilityFactoryInstance->SpawnMPActor(static_cast<int>(abilityTag), abilityOwner->GetActorLocation(), abilityOwner->GetActorRotation());

		if (spawnActor)
		{
			UMPAbility* abilitySpawn = Cast<UMPAbility>(spawnActor);
			if (abilitySpawn)
			{
				return abilitySpawn;
			}
		}
	}
	return nullptr;
}

// game process
	// lobby
// All lobby logic is now in ULobbyManager

	// character custom
void AMPGMGameplay::StartCustomizeCharacter()
{
	if(MatchManager) MatchManager->StartCustomizeCharacter();
}
void AMPGMGameplay::CountdownCustomizeCharacter()
{
	if(MatchManager) MatchManager->CountdownCustomizeCharacter();
}
void AMPGMGameplay::EndCustomizeCharacter()
{
	if(MatchManager) MatchManager->EndCustomizeCharacter();
}

	// start game
void AMPGMGameplay::StartGame()
{
	if(MatchManager) MatchManager->StartGame();
}

bool AMPGMGameplay::CheckIfGameEnd()
{
	if (MatchManager) return MatchManager->CheckIfGameEnd();
		return false;
}
void AMPGMGameplay::EndGameplayTime()
{
    if(MatchManager) MatchManager->EndGameplayTime();
}

void AMPGMGameplay::RemoveGameplayHUD()
{
	// This functionality is now in MatchManager
}

void AMPGMGameplay::RegisterPlayerDeath(AMPControllerPlayer* diedPlayer,
	FVector diedPlayerLocation, FRotator diedPlayerRotation)
{
	if(MatchManager) MatchManager->RegisterPlayerDeath(diedPlayer, diedPlayerLocation, diedPlayerRotation);
}

void AMPGMGameplay::DisplayProgressionStatus()
{
	if(MatchManager) MatchManager->DisplayProgressionStatus();
}

// setters and getters
FString AMPGMGameplay::GetDebugModeStatus() const
{
	if (singlePlayerDebugMode)
	{
		return FString::Printf(TEXT("Single Player Debug Mode - Team: %s"), 
			debugPlayer == ETeam::EHuman ? TEXT("Human") : TEXT("Cat"));
	}
	else if (multiplayerDebugMode)
	{
		return TEXT("Multiplayer Debug Mode - Relaxed Requirements");
	}
	else
	{
		return TEXT("Normal Mode - Full Requirements");
	}
}

void AMPGMGameplay::RestartLobby()
{
	// Reset game state
	if (theGameState)
	{
		theGameState->curGameplayStatus = EGPStatus::ELobby;
		theGameState->isMostPlayerReady = false;
		theGameState->curReadyTime = 0;
		theGameState->curCustomCharacterTime = 0;
		theGameState->curPrepareTime = 0;
		theGameState->curGameplayTime = 0;
	}
	
	// Clear all timers
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(readyTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(restartLobbyTimerHandle);
	}
	
	// Reset all players
	for (AMPControllerPlayer* eachPlayer : allPlayersControllers)
	{
		if (eachPlayer)
		{
			AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
			if (eachState)
			{
				eachState->isPlayerReady = false;
				eachState->isPlayerDied = false;
			}
		}
	}
	
	// Start fresh lobby
	LobbyManager->StartLobby();
	
	UMPLogManager::LogInfo(TEXT("Lobby restarted successfully"), TEXT("MPGMGameplay"));
}

// =====================
// Bot Management Methods
// =====================

bool AMPGMGameplay::AddBot(ETeam team)
{
    if (AIControllerManager)
    {
        bool Result = AIControllerManager->AddBot(team);
        if (Result && LobbyManager)
        {
            LobbyManager->BroadcastPlayerListUpdate();
        }
        return Result;
    }
    return false;
}

bool AMPGMGameplay::RemoveBot(int32 playerIndex)
{
    if (AIControllerManager)
    {
        bool Result = AIControllerManager->RemoveBot(playerIndex);
        if (Result && LobbyManager)
        {
            LobbyManager->BroadcastPlayerListUpdate();
        }
        return Result;
    }
    return false;
}

// =====================
// Ready State Management
// =====================

// Team-specific player lists
TArray<AMPControllerPlayer*> AMPGMGameplay::GetHumanPlayers() const
{
	TArray<AMPControllerPlayer*> humanPlayers;
	
	for (AMPControllerPlayer* player : allPlayersControllers)
	{
		if (player)
		{
			AMPPlayerState* playerState = Cast<AMPPlayerState>(player->PlayerState);
			if (playerState && playerState->playerTeam == ETeam::EHuman)
			{
				humanPlayers.Add(player);
			}
		}
	}
	
	return humanPlayers;
}

TArray<AMPControllerPlayer*> AMPGMGameplay::GetCatPlayers() const
{
	TArray<AMPControllerPlayer*> catPlayers;
	
	for (AMPControllerPlayer* player : allPlayersControllers)
	{
		if (player)
		{
			AMPPlayerState* playerState = Cast<AMPPlayerState>(player->PlayerState);
			if (playerState && playerState->playerTeam == ETeam::ECat)
			{
				catPlayers.Add(player);
			}
		}
	}
	
	return catPlayers;
}

// Countdown update RPCs
void AMPGMGameplay::ClientUpdateReadyCountdown_Implementation(int32 secondsRemaining)
{
	if (LobbyManager)
	{
		LobbyManager->ClientUpdateReadyCountdown(secondsRemaining);
	}
}

void AMPGMGameplay::ClientUpdateCustomizationCountdown_Implementation(int32 secondsRemaining)
{
	UMPLogManager::LogDebug(FString::Printf(TEXT("Client received customization countdown update: %d"), secondsRemaining), TEXT("MPGMGameplay"));
	
	// Update countdown on all player HUDs on this client
	for (AMPControllerPlayer* eachPlayer : allPlayersControllers)
	{
		if (eachPlayer && eachPlayer->lobbyManagerHUD)
		{
			eachPlayer->lobbyManagerHUD->UpdateCountdownText(secondsRemaining);
		}
	}
}

int32 AMPGMGameplay::GetPlayerPreviewSlot(AMPControllerPlayer* Player) const
{
    if (PreviewManager)
    {
        return PreviewManager->GetPlayerPreviewSlot(Player);
    }
	return -1;
}

void AMPGMGameplay::RequestPreviewCharacterUpdate(AMPControllerPlayer* Player, ETeam Team, int CatRace, int HumanProfession, int Hat)
{
    if (PreviewManager)
{
        PreviewManager->RequestPreviewCharacterUpdate(Player, Team, CatRace, HumanProfession, Hat);
	}
}