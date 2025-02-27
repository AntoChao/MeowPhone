#include "MPGMGameplay.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

#include "../CommonEnum.h"

#include "MPGI.h"

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
#include "../MPActor/AI/MPControllerAI.h"
#include "../MPActor/Ability/MPAbility.h"

AMPGMGameplay::AMPGMGameplay()
{
	return;
}

// gameplay common

/* PostLogin
* called when player join the lobby
* no need to have body, just attach the client lobby HUD
*/
void AMPGMGameplay::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);

	UE_LOG(LogTemp, Warning, TEXT("GM: One player log in"));
	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Yellow, TEXT("GM: One player log in"));
	
	SetupFactoryInstances();
	AMPControllerPlayer* curPlayer = Cast<AMPControllerPlayer>(newPlayer);
	
	if (curPlayer)
	{
		curPlayer->InitializePS(allPlayersControllers.Num());

		allPlayersControllers.Add(curPlayer);
		
		// should attach lobby hud at beginning
		curPlayer->AttachHUD(EHUDType::ELobby, 0); // hud enum + zorder

		if (isSinglePlayerTesting)
		{
			StartGame();
		}
	}
}

void AMPGMGameplay::Logout(AController* exiting)
{
	Super::Logout(exiting);

	UE_LOG(LogTemp, Warning, TEXT("GM: One player log out"));

	AMPControllerPlayer* curPlayer = Cast<AMPControllerPlayer>(exiting);
	if (curPlayer)
	{
		allPlayersControllers.Remove(curPlayer);
		RemoveControlledCharacters(curPlayer);
	}

	bool isGameEnd = CheckIfGameEnd();
}

void AMPGMGameplay::BeginPlay()
{
	Super::BeginPlay();

	StartLobby();
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
AMPEnvActor* AMPGMGameplay::SpawnEnvironment(EEnvActor envTag, FVector spawnLocation, FRotator spawnRotation)
{
	if (environmentFactoryInstance)
	{
		AActor* spawnActor = environmentFactoryInstance->SpawnMPActor(static_cast<int>(envTag), spawnLocation, spawnRotation);

		if (spawnActor)
		{
			AMPEnvActor* envSpawn = Cast<AMPEnvActor>(spawnActor);
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
		UObject* spawnObject = itemFactoryInstance->SpawnMPObject(abilityOwner, static_cast<int>(abilityTag));

		if (spawnObject)
		{
			UMPAbility* abilitySpawn = Cast<UMPAbility>(spawnObject);
			if (abilitySpawn)
			{
				return abilitySpawn;
			}
		}
	}
	return nullptr;
}

// game process

void AMPGMGameplay::StartLobby()
{
	curGameplayStatus = EGPStatus::ELobby;

	for (AMPControllerPlayer* eachPlayer : allPlayersControllers)
	{
		eachPlayer->AttachHUD(EHUDType::ELobby, 0);
		eachPlayer->LobbyStartUpdate();
	}
}
/* Got ready
* called by when a player is ready
*/
void AMPGMGameplay::GotReady(AMPControllerPlayer* aPlayer)
{
	if (CheckReadyToStartGame())
	{
		isReady = true;
		curReadyTime = readyTotalTime;
		CountdownReadyGame();
	}
	else 
	{
		isReady = false; // represents if a player cancel ready
	}
}
bool AMPGMGameplay::CheckReadyToStartGame()
{
	return CheckBothTeamHasPlayers() && CheckHalfPlayersAreReady();
}

bool AMPGMGameplay::CheckBothTeamHasPlayers()
{
	int humanPlayers = 0;
	int catPlayers = 0;
	for (AMPControllerPlayer* eachPlayer : allPlayersControllers)
	{
		AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
		if (eachState)
		{
			switch(eachState->playerTeam)
			{
				case ETeam::EHuman:
				{
					humanPlayers ++;
					break;
				}
				case ETeam::ECat:
				{
					catPlayers ++;
					break;
				}
			}
		}
	}

	return humanPlayers > 0 && catPlayers > 0;
}
bool AMPGMGameplay::CheckHalfPlayersAreReady()
{
	int numReadyPlayers = 0;
	for (AMPControllerPlayer* eachPlayer : allPlayersControllers)
	{
		AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
		if (eachState)
		{
			if (eachState->isReady)
			{
				numReadyPlayers ++;
			}
		}
	}
	return numReadyPlayers >= allPlayersControllers.Num();
}

void AMPGMGameplay::CountdownReadyGame()
{
	if (isReady)
	{
		if (curReadyTime > 0)
		{
			UWorld* serverWorld = GetWorld();
			if (serverWorld)
			{
				curReadyTime -= 1;

				serverWorld->GetTimerManager().ClearTimer(readyTimerHandle);
				FTimerDelegate readyTimerDel;
				readyTimerDel.BindUFunction(this, FName("CountdownReadyGame"));
				serverWorld->GetTimerManager().SetTimer(readyTimerHandle, readyTimerDel, 1, false);
			}
		}
		else 
		{
			EndReadyTime();
		}
	}
}
void AMPGMGameplay::EndReadyTime()
{
	// remove all lobby hud
	for (AMPControllerPlayer* eachPlayer : allPlayersControllers)
	{
		if (eachPlayer)
		{
			eachPlayer->RemoveHUD(EHUDType::ELobby);
		}
	}

	StartGame();
}

void AMPGMGameplay::StartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("GM: Starting Game"));
	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Yellow, TEXT("GM: Starting Game"));

	SetupGame();
	StartPrepareTime();
}

void AMPGMGameplay::SetupGame()
{
	SetupMap();
	SetupHumans();
	SetupAllCats();
	SetupGameplayHUD();
}

/* SetupMap()
* Manage all items, it get all actors in the world, 
* but eliminate some of them to be randomness
*/
void AMPGMGameplay::SetupMap()
{
	TArray<AActor*> allActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMPItem::StaticClass(), allActors);

    for (AActor* eachActor : allActors)
    {
        AMPItem* eachItem = Cast<AMPItem>(eachActor);
        if (eachItem)
        {
			int32 randomNumber = FMath::RandRange(1, 100);
			if (randomNumber > itemRemainPercentage)
			{
				eachItem->GetEliminated();
			}
        }
    }
}
void AMPGMGameplay::SetupHumans()
{
	int i = 0;
	for (AMPControllerPlayer* eachPlayer : allPlayersControllers)
	{
		AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
		if (eachState)
		{
			if (eachState->playerTeam == ETeam::EHuman)
			{
				if (humanFactoryInstance)
				{
					int professionInt = static_cast<int>(eachState->humanProfession);
					AActor* humanBody = humanFactoryInstance->SpawnMPActor(professionInt, allHumanSpawnLocations[i], allHumanSpawnRotations[i]);

					if (humanBody)
					{
						AMPCharacter* humanMPBody = Cast<AMPCharacter>(humanBody);
						if (humanMPBody)
						{
							eachPlayer->Possess(humanMPBody);
							allPlayerCharacters.Add(humanMPBody);
							i ++;

							UE_LOG(LogTemp, Warning, TEXT("GM: One human player created sucessfully"));
							GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Yellow, TEXT("GM: One human player created sucessfully"));

						}
					}
				}
			}
		}
	}
}
void AMPGMGameplay::SetupAllCats()
{
	playerCatIndeces.Empty();
	SetupPlayerCats();
	// SetupAICats();
}
void AMPGMGameplay::SetupPlayerCats()
{
	for (AMPControllerPlayer* eachPlayer : allPlayersControllers)
	{
		int i = -1;
		AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
		if (eachState)
		{
			if (eachState->playerTeam == ETeam::ECat)
			{
				if (catFactoryInstance)
				{
					while (i < 0 || i >= numOfCats || playerCatIndeces.Contains(i))
					{
						i = FMath::RandRange(0, numOfCats);
					}

					int catInt = static_cast<int>(eachState->catRace);
					AActor* catBody = catFactoryInstance->SpawnMPActor(catInt, allCatSpawnLocations[i], allCatSpawnRotations[i]);

					if (catBody)
					{
						AMPCharacter* catMPBody = Cast<AMPCharacter>(catBody);
						if (catMPBody)
						{
							eachPlayer->Possess(catMPBody);
							allPlayerCharacters.Add(catMPBody);

							UE_LOG(LogTemp, Warning, TEXT("GM: One cat player created sucessfully"));
							GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Yellow, TEXT("GM: One cat player created sucessfully"));
						}
					}
				}
			}
		}
	}
}
void AMPGMGameplay::SetupAICats()
{
	for (int i = 0; i < numOfCats; i++)
	{
		if (!playerCatIndeces.Contains(i))
		{
			if (catFactoryInstance && aiControllerFactoryInstance)
			{
				int catInt = FMath::RandRange(0, totalCatRacesNum);
				AActor* catBody = catFactoryInstance->SpawnMPActor(catInt, allHumanSpawnLocations[i], allHumanSpawnRotations[i]);

				int aiInt = FMath::RandRange(0, totalAITypesNum);
				AActor* aiController = aiControllerFactoryInstance->SpawnMPActor(aiInt, FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
				
				if (catBody && aiController)
				{
					AMPCharacter* catAIMPBody = Cast<AMPCharacter>(catBody);
					AMPControllerAI* aiMPController = Cast<AMPControllerAI>(aiController);
					if (catAIMPBody && aiMPController)
					{
						aiMPController->Possess(catAIMPBody);
						allAICharactersCats.Add(catAIMPBody);
					}
				}
			}
		}
	}
}
void AMPGMGameplay::SetupGameplayHUD()
{
	for (AMPControllerPlayer* eachPlayer : allPlayersControllers)
	{
		if (eachPlayer)
		{
			AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
			if (eachState)
			{
				switch (eachState->playerTeam)
				{
				case ETeam::EHuman:
				{
					eachPlayer->AttachHUD(EHUDType::EGameplayHuman, 0);
					break;
				}
				case ETeam::ECat:
				{
					eachPlayer->AttachHUD(EHUDType::EGameplayCat, 0);
					break;
				}
				default:
					break;
				}
			}
		}
	}
}

void AMPGMGameplay::StartPrepareTime()
{
	curGameplayStatus = EGPStatus::EPrepare;
	curPrepareTime = prepareTotalTime;
	CountdownPrepareGame();

	for (AMPControllerPlayer* eachPlayer : allPlayersControllers)
	{
		eachPlayer->PrepareStartUpdate();
	}
}
void AMPGMGameplay::CountdownPrepareGame()
{
	if (curPrepareTime > 0)
	{
		UWorld* serverWorld = GetWorld();
		if (serverWorld)
		{
			curPrepareTime -= 1;

			serverWorld->GetTimerManager().ClearTimer(prepareTimerHandle);
			FTimerDelegate prepareTimerDel;
			prepareTimerDel.BindUFunction(this, FName("CountdownPrepareGame"));
			serverWorld->GetTimerManager().SetTimer(prepareTimerHandle, prepareTimerDel, 1, false);
		}
	}
	else 
	{
		EndPrepareTime();
	}
}
void AMPGMGameplay::EndPrepareTime()
{
	StartGameplayTime();
}

void AMPGMGameplay::StartGameplayTime()
{
	curGameplayStatus = EGPStatus::EGameplay;
	curGameplayTime = gameplayTotalTime;
	CountdownGameplayGame();

	for (AMPControllerPlayer* eachPlayer : allPlayersControllers)
	{
		eachPlayer->GameplayStartUpdate();
	}
}
void AMPGMGameplay::CountdownGameplayGame()
{
	if (curGameplayTime > 0)
	{
		UWorld* serverWorld = GetWorld();
		if (serverWorld)
		{
			curGameplayTime -= 1;

			serverWorld->GetTimerManager().ClearTimer(gameplayTimerHandle);
			FTimerDelegate gameplayTimerDel;
			gameplayTimerDel.BindUFunction(this, FName("CountdownGameplayGame"));
			serverWorld->GetTimerManager().SetTimer(gameplayTimerHandle, gameplayTimerDel, 1, false);
		}
	}
	else 
	{
		EndGameplayTime();
	}
}

bool AMPGMGameplay::CheckIfGameEnd()
{
	bool isGameEnd = false;
	// check if game is ended
	if (isGameEnd)
	{
		EndGameplayTime();
		return true;
	}
	else
	{
		return false;
	}
}
void AMPGMGameplay::EndGameplayTime()
{
	RemoveGameplayHUD();
	StartLobby();
}

void AMPGMGameplay::RemoveGameplayHUD()
{
	for (AMPControllerPlayer* eachPlayer : allPlayersControllers)
	{
		if (eachPlayer) 
		{
			AMPPlayerState* eachState = Cast<AMPPlayerState>(eachPlayer->PlayerState);
			if (eachState)
			{
				switch (eachState->playerTeam)
				{
				case ETeam::EHuman:
				{
					eachPlayer->RemoveHUD(EHUDType::EGameplayHuman);
					break;
				}
				case ETeam::ECat:
				{
					eachPlayer->RemoveHUD(EHUDType::EGameplayCat);
					break;
				}
				default:
					break;
				}
			}
		}
	}
}

void AMPGMGameplay::RegisterPlayerDeath(AMPControllerPlayer* diedPlayer,
	FVector diedPlayerLocation, FRotator diedPlayerRotation)
{
	if (!CheckIfGameEnd() && diedPlayer)
	{
		RemoveControlledCharacters(diedPlayer);

		// let the player possess the spectator body
		AMPPlayerState* diedPlayerState = Cast<AMPPlayerState>(diedPlayer->PlayerState);
		if (diedPlayerState)
		{
			AActor* diedBody = nullptr;
			if (diedPlayerState->playerTeam == ETeam::EHuman)
			{
				if (humanFactoryInstance)
				{
					diedBody = humanFactoryInstance->SpawnMPActor(static_cast<int>(EHumanProfession::EDiedHuman), diedPlayerLocation, diedPlayerRotation);
				}
			}
			else if (diedPlayerState->playerTeam == ETeam::ECat)
			{
				if (catFactoryInstance)
				{
					diedBody = catFactoryInstance->SpawnMPActor(static_cast<int>(ECatRace::EDiedCat), diedPlayerLocation, diedPlayerRotation);
				}
			}

			if (diedBody)
			{
				AMPCharacter* mpDiedBody = Cast<AMPCharacter>(diedBody);
				if (mpDiedBody)
				{
					diedPlayer->Possess(mpDiedBody);
					allPlayerCharacters.Add(mpDiedBody);
				}
			}
		}
	}
}