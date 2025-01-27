#include "MPGMGameplay.h"
#include "../CommonEnum.h"

#include "MPGI"

AMPGMGameplay::AMPGMGameplay()
{
	return;
}

// gameplay common
void AMPGMGameplay::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);
}

void AMPGMGameplay::Logout(APlayerController* exiting)
{
	Super::Logout(exiting);
}

void AMPGMGameplay::BeginPlay()
{
	Super::BeginPlay();
}

// factories
void AMPGMGameplay::SetupFactoryInstances();

// game process
void AMPGMGameplay::CountDown()
{
	return;
}
bool AMPGMGameplay::IsEnableToStartGame()
{
	return;
}
void AMPGMGameplay::StartGame()
{
	return;
}

void AMPGMGameplay::CheckIfGameEnd()
{
	return;

}
void AMPGMGameplay::EndGame()
{
	return;
}

void AMPGMGameplay::SetupGame()
{
	return;
}
void AMPGMGameplay::SetupMap()
{
	return;
}
void AMPGMGameplay::SetupHumans()
{
	return;
}
void AMPGMGameplay::SetupCats()
{
	return;
}

void AMPGMGameplay::StartPrepareTime()
{
	return;
}
void AMPGMGameplay::EndPrepareTime()
{
	return;
}
void AMPGMGameplay::StartGameplayTime()
{
	return;
}
void AMPGMGameplay::EndGameplayTime()
{
	return;
}