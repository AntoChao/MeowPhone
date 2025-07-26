#include "MPPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "../HighLevel/MPLogManager.h"
#include "../MPActor/Player/MPControllerPlayer.h"

AMPPlayerState::AMPPlayerState()
{
	// Initialize default values
	playerName = TEXT("Player");
	playerIndex = 0;
	playerTeam = ETeam::ENone;
	catRace = ECatRace::ECat1;
	playerSelectedAbility = EAbility::ENone;
	humanProfession = EHumanProfession::EHuman1;
	playerSelectedHat = EHat::ENone;
	isPlayerReady = false;
	isPlayerDied = false;
}

void AMPPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Register replicated properties
	DOREPLIFETIME(AMPPlayerState, playerName);
	DOREPLIFETIME(AMPPlayerState, playerIndex);
	DOREPLIFETIME(AMPPlayerState, playerTeam);
	DOREPLIFETIME(AMPPlayerState, catRace);
	DOREPLIFETIME(AMPPlayerState, playerSelectedAbility);
	DOREPLIFETIME(AMPPlayerState, humanProfession);
	DOREPLIFETIME(AMPPlayerState, playerSelectedHat);
	DOREPLIFETIME(AMPPlayerState, isPlayerReady);
	DOREPLIFETIME(AMPPlayerState, isPlayerDied);
}

void AMPPlayerState::OnRep_PlayerTeam()
{
	UMPLogManager::LogDebug(FString::Printf(TEXT("Player %s team replicated: %d"), 
		*playerName, (int32)playerTeam), TEXT("MPPlayerState"));
	
	// Update HUD if this is the local player
	AMPControllerPlayer* playerController = Cast<AMPControllerPlayer>(GetOwner());
	if (playerController && playerController->IsLocalPlayerController() && playerController->lobbyManagerHUD && playerController->lobbyManagerHUD->lobbyHUD)
	{
		// Only update team button visibility, full list update will be handled by GameMode RPC
		playerController->lobbyManagerHUD->lobbyHUD->UpdateTeamButtonVisibility();
	}
}

void AMPPlayerState::OnRep_IsPlayerReady()
{
	UMPLogManager::LogDebug(FString::Printf(TEXT("Player %s ready state replicated: %s"), 
		*playerName, isPlayerReady ? TEXT("Ready") : TEXT("Not Ready")), TEXT("MPPlayerState"));
	
	// Update HUD if this is the local player
	AMPControllerPlayer* playerController = Cast<AMPControllerPlayer>(GetOwner());
	if (playerController && playerController->IsLocalPlayerController() && playerController->lobbyManagerHUD)
	{
		// Update ready state in the manager, which will update the shared ready button
		playerController->lobbyManagerHUD->UpdateReadyState(isPlayerReady);
	}
}

void AMPPlayerState::OnRep_IsPlayerDied()
{
	UMPLogManager::LogDebug(FString::Printf(TEXT("Player %s death state replicated: %s"), 
		*playerName, isPlayerDied ? TEXT("Dead") : TEXT("Alive")), TEXT("MPPlayerState"));
}