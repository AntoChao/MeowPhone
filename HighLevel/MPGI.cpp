#include "MPGI.h"
#include "../CommonEnum.h"
#include "MPSave.h"
#include "Kismet/GameplayStatics.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "UObject/NoExportTypes.h"

UMPGI::UMPGI()
{
    createSessionsCompletedDelegate = FOnCreateSessionsCompleteDelegate::CreateUObject(this, &UMPGI::HostSessionCompleted);
	searchForSessionsCompletedDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMPGI::SearchForSessionsCompleted);;
    joinSessionCompletedDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMPGI::JoinSessionCompleted);;
	endSessionCompletedDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &UMPGI::EndSessionCompleted);;
	destroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMPGI::DestroySessionCompleted);;
}

// common game instance section 
void UMPGI::Init()
{
    Super::Init();
    LoadGame();
}

void UMPGI::Shutdown()
{
    Super::Shutdown();
    SaveGame();
}

// save file section
void UMPGI::CreateSaveFile()
{
    UMPSave* dataToSave = Cast<UMPSave>(UGameplayStatics::CreateSaveGameObject(UMPSave::StaticClass()));
    if (dataToSave)
    {
        curPlayerName = GenerateRandomUserString();
        dataToSave->playerNameSave = curPlayerName;

        UGameplayStatics::SaveGameToSlot(dataToSave, "Slot1", 0);
    }
}

FString UMPGI::GenerateRandomUserString()
{
    FString base = TEXT("user");

    for (int i = 0; i < 6; ++i)
    {
        int32 randomDigit = FMath::RnadRange(0, 9);
        base += FString::FromInt(randomDigit);
    }
    return base;
}

void UMPGI::SaveGame()
{
    UMPSave* dataToSave = Cast<UMPSave>(UGameplayStatics::LoadGameFromSLot("Slot1", 0));
    if (dataToSave == nullptr)
    {
        dataToSave = Cast<UMPSave>(UGameplayStatics::CreateSaveGameObject(UMPSave::StaticClass()));
    }

    if (dataToSave)
    {
        dataToSave->playerNameSave = curPlayerName;
        UGameplayStatics::SaveGameToSlot(dataToSave, "Slot1", 0);
    }
}


void UMPGI::LoadGame()
{
    if (UGameplayStatics::DoesSaveGameExist("Slot1", 0))
    {
        UMPSave* dataToLoad = Cast<UMPSave>(UGameplayStatics::LoadGameFromSlot("Slot1", 0));

        if (dataToLoad)
        {
            curPlayerName = dataToLoad->playerNameSave;
        }
    }
    else 
    {
        CreateSaveFile();
    }
}

// session section
void UMPGI::HostSession(FName sessionName, int numPlayers)
{
    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
            UE_LOG(LogTemp, Error, TEXT("GI: Try Hosting Session"));
			curSessionName = sessionName;

			createSessionsCompletedHandle = onlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(createSessionsCompletedDelegate);

			FOnlineSessionSettings sessionSettings;

			sessionSettings.bIsDedicated = false;
			sessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
			sessionSettings.bShouldAdvertise = true;
			sessionSettings.NumPublicConnections = numPlayers;
			sessionSize = numPlayers + 1;
			sessionSettings.NumPrivateConnections = 0;

			sessionSettings.bUsesPresence = true;
			sessionSettings.bAllowJoinInProgress = true;
			sessionSettings.bAllowInvites = true;
			sessionSettings.bAllowJoinViaPresence = true;
			sessionSettings.bAllowJoinViaPresenceFriendsOnly = true;
			sessionSettings.Set(FName("SESSION_NAME"), sessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService); // Storing session name

			const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			if (onlineSessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), sessionName, sessionSettings)) //*sessionSettings
			{
				UE_LOG(LogTemp, Error, TEXT("GI: A session has been created"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("GI: Session Interface has failed to be created"));
				endSession();
			}
		}
	}
}


void UMPGI::SearchForSessions()
{
    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			searchForSessionsCompletedHandle = onlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(searchForSessionsCompletedDelegate);

			searchSettings = MakeShareable(new FOnlineSessionSearch());
			searchSettings->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
			searchSettings->MaxSearchResults = 10000;
			searchSettings->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);
			// searchSettings->PingBucketSize = 60;
			searchSettings->TimeoutInSeconds = 30.0f;

			const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();

			if (onlineSessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), searchSettings.ToSharedRef()))
			{
				UE_LOG(LogTemp, Error, TEXT("search started"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("search unable to start"));
			}
		}
	}
}


void UMPGI::JoinSession(int sessionIndex)
{
    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			if (SessionList.IsValidIndex(SessionIndex))
			{
				// attempt to join the session
				const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
				joinSessionCompletedHandle = onlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(joinSessionCompletedDelegate);
				
				const FSessionInfo& SelectedSession = SessionList[SessionIndex];
				FName SessionName(*SelectedSession.SessionName);
				FOnlineSessionSearchResult SearchResult = searchSettings->SearchResults[SessionIndex];

				onlineSessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), SessionName, SearchResult);
			}
		}
	}
}


void UMPGI::EndSession()
{
    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
    {
        if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
        {
            APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
            if (PlayerController)
            {
                PlayerController->ClientTravel("Game/HighLevel/lobbylevel/LobbyLevel", TRAVEL_Absolute);
            }

            endSessionCompletedHandle = onlineSessionInterface->AddOnEndSessionCompleteDelegate_Handle(endSessionCompletedDelegate);
            bool ableToEnd = onlineSessionInterface->EndSession(curSessionName);
        }
    }
}


void UMPGI::DestroySession(FName sessionName)
{
    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			destroySessionCompletedHandle = onlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(destroySessionCompletedDelegate);
			onlineSessionInterface->DestroySession(sessionName);
		}
	}
}



void UMPGI::HostSessionCompleted(FName sessionName, bool hostCompleted)
{
    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			if (hostCompleted)
			{
				onlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionsCompletedHandle);

				// have to put the level into listen for listen erver
				GetWorld()->ServerTravel("/Game/HighLevel/GameplayLevel?listen");
				onlineSessionInterface->StartSession(sessionName);
			}
		}
	}
}


void UMPGI::SearchForSessionsCompleted(bool searchCompleted)
{
    foundSuccessed = searchCompleted;

	if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			onlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(searchForSessionsCompletedHandle);

			UE_LOG(LogTemp, Error, TEXT("search found %d sessions after completing search."), searchSettings->SearchResults.Num());

			TArray<FOnlineSessionSearchResult> SearchResults = searchSettings->SearchResults;

			if (SearchResults.Num() > 0)
			{
				SessionList.Empty();
				for (int32 i = 0; i < SearchResults.Num(); ++i)
				{
					FOnlineSessionSearchResult& SearchResult = SearchResults[i];
					FSessionInfo Info;
					FString SessionDisplayName;
					SearchResult.Session.SessionSettings.Get(FName("SESSION_NAME"), SessionDisplayName);  // Retrieving the session name

					Info.SessionIndex = i;
					Info.SessionName = !SessionDisplayName.IsEmpty() ? SessionDisplayName : TEXT("Default Server");  // Using a default if no name is found
					Info.CurrentPlayers = SearchResult.Session.SessionSettings.NumPublicConnections - SearchResult.Session.NumOpenPublicConnections;
					Info.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;

					SessionList.Add(Info);

					UE_LOG(LogTemp, Warning, TEXT("Session search info created for %s"), *Info.SessionName);
				}

                /*
				if (searchWidget)
				{
					searchWidget->refreshSearchResult(SessionList);
				}*/
			}
		}
	}
}


void UMPGI::JoinSessionCompleted(FName sessionName)
{
	if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			onlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompletedHandle);

			bool wasTravelSuccessful = travelToSession(sessionName);

			// curSessionName = sessionName;
		}
	}
}

bool UMPGI::TravelToSession(FName sessionName)
{
	if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			FString connectionInfo;
			if (onlineSessionInterface->GetResolvedConnectString(sessionName, connectionInfo))
			{
				// Travel the client to the server
				APlayerController* playerController = GetWorld()->GetFirstPlayerController();
				playerController->ClientTravel(connectionInfo, TRAVEL_Absolute);
				return true;
			}
			else
			{
				return false; // the connection infromation could not be obtained
			}
		}
	}

	// the client was unable to travel
	return false;
}

void UMPGI::EndSessionCompleted(FName sessionName, bool endCompleted)
{
    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			onlineSessionInterface->ClearOnEndSessionCompleteDelegate_Handle(endSessionCompletedHandle);

			destroySession(sessionName);
		}
	}
}


void UMPGI::DestroySessionCompleted(FName sessionName, bool destroyCompleted)
{
    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			onlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(destroySessionCompletedHandle);
		}
	}
}


// getter && setter
ELanguage UMPGI::getGameLanguage()
{
    return gameLanguage;
}

FName UMPGI::getCurPlayerName()
{
    return curPlayerName;
}
