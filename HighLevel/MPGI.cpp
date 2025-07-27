#include "MPGI.h"
#include "../CommonEnum.h"
#include "MPSave.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/ManagerLocalization.h"
#include "Managers/ManagerLog.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "../MPActor/Player/Widget/HUDSearchSession.h"
#include "../MPActor/Player/MPControllerPlayer.h"

#include "UObject/NoExportTypes.h"

UMPGI::UMPGI()
{
    createSessionsCompletedDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMPGI::HostSessionCompleted);
	searchForSessionsCompletedDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMPGI::SearchForSessionsCompleted);
    joinSessionCompletedDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMPGI::JoinSessionCompleted);
	endSessionCompletedDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &UMPGI::EndSessionCompleted);
	destroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMPGI::DestroySessionCompleted);
}

UMPGI::~UMPGI()
{
    // Clean up all delegate handles to prevent memory leaks
    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
    {
        if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
        {
            onlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionsCompletedHandle);
            onlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(searchForSessionsCompletedHandle);
            onlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompletedHandle);
            onlineSessionInterface->ClearOnEndSessionCompleteDelegate_Handle(endSessionCompletedHandle);
            onlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(destroySessionCompleteHandle);
        }
    }
}

// common game instance section 
void UMPGI::Init()
{
    Super::Init();
    
    // Initialize logging system first
    UManagerLog::InitializeLogging();
    
    LoadGame();
    InitializeLocalization();
    
    UManagerLog::LogInfo(TEXT("Game Instance initialized"), TEXT("MPGI"));
}

void UMPGI::Shutdown()
{
    Super::Shutdown();
    SaveGame();
    
    UManagerLog::LogInfo(TEXT("Game Instance shutting down"), TEXT("MPGI"));
}

// save file section
void UMPGI::CreateSaveFile()
{
    UMPSave* dataToSave = Cast<UMPSave>(UGameplayStatics::CreateSaveGameObject(UMPSave::StaticClass()));
    if (dataToSave)
    {
        curPlayerName = GenerateRandomName();
        dataToSave->playerNameSave = curPlayerName;

        UGameplayStatics::SaveGameToSlot(dataToSave, "Slot1", 0);
        UManagerLog::LogInfo(TEXT("Save file created"), TEXT("MPGI"));
    }
    else
    {
        UManagerLog::LogError(TEXT("Failed to create save file"), TEXT("MPGI"));
    }
}

void UMPGI::SaveGame()
{
    UMPSave* dataToSave = Cast<UMPSave>(UGameplayStatics::LoadGameFromSlot("Slot1", 0));
    if (dataToSave == nullptr)
    {
        dataToSave = Cast<UMPSave>(UGameplayStatics::CreateSaveGameObject(UMPSave::StaticClass()));
    }

    if (dataToSave)
    {
        dataToSave->playerNameSave = curPlayerName;
        UGameplayStatics::SaveGameToSlot(dataToSave, "Slot1", 0);
        UManagerLog::LogDebug(TEXT("Game saved successfully"), TEXT("MPGI"));
    }
    else
    {
        UManagerLog::LogError(TEXT("Failed to save game"), TEXT("MPGI"));
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
            UManagerLog::LogInfo(TEXT("Game loaded successfully"), TEXT("MPGI"));
        }
        else
        {
            UManagerLog::LogWarning(TEXT("Failed to load save data, creating new save"), TEXT("MPGI"));
            CreateSaveFile();
        }
    }
    else 
    {
        UManagerLog::LogInfo(TEXT("No save file found, creating new one"), TEXT("MPGI"));
        CreateSaveFile();
    }
}

// getter && setter
// Legacy methods are now inline in header for backward compatibility

ELanguage UMPGI::GetCurrentLanguage() const
{
	// Game Instance is the single source of truth
	return gameLanguage;
}

void UMPGI::SetCurrentLanguage(ELanguage newLanguage)
{
	// Update game instance language and sync localization manager
	gameLanguage = newLanguage;
	
	if (localizationManager)
	{
		localizationManager->SetCurrentLanguage(newLanguage);
	}
	
	// Save the change immediately
	SaveGame();
	
	UManagerLog::LogInfo(FString::Printf(TEXT("Language changed to %d and saved"), (int32)newLanguage), TEXT("MPGI"));
}

FString UMPGI::GetCurPlayerName()
{
    return curPlayerName;
}

// Steam Online Subsystem Session Control	
// session section
void UMPGI::HostSession(FName sessionName, int numPlayers)
{
    // Check if already in a session
    if (IsInSession())
    {
        UManagerLog::LogWarning(TEXT("Already in session, ending current session first"), TEXT("MPGI"));
        EndSession();
        return;
    }

    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			if (enableSessionLogging)
			{
				UManagerLog::LogInfo(TEXT("Attempting to host session"), TEXT("MPGI"));
			}
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
			sessionSettings.bAllowJoinInProgress = allowJoinInProgress;
			sessionSettings.bAllowInvites = true;
			sessionSettings.bAllowJoinViaPresence = true;
			sessionSettings.bAllowJoinViaPresenceFriendsOnly = allowFriendsOnlyJoin;
			
			// Store session metadata for search functionality
			sessionSettings.Set(FName("SESSION_NAME"), sessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
			sessionSettings.Set(FName("HOST_NAME"), curPlayerName, EOnlineDataAdvertisementType::ViaOnlineService);
			sessionSettings.Set(FName("MAX_PLAYERS"), numPlayers, EOnlineDataAdvertisementType::ViaOnlineService);
			sessionSettings.Set(FName("CURRENT_PLAYERS"), 1, EOnlineDataAdvertisementType::ViaOnlineService); // Host counts as 1 player
			
			// Store password protection info
			sessionSettings.Set(FName("USE_PASSWORD"), sessionUsePassword, EOnlineDataAdvertisementType::ViaOnlineService);
			if (sessionUsePassword && !sessionPassword.IsEmpty())
			{
				sessionSettings.Set(FName("PASSWORD"), sessionPassword, EOnlineDataAdvertisementType::ViaOnlineService);
			}

			const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			if (localPlayer && localPlayer->GetPreferredUniqueNetId().IsValid())
			{
				if (onlineSessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), sessionName, sessionSettings))
				{
					if (enableSessionLogging)
					{
						UManagerLog::LogInfo(TEXT("Session creation request sent"), TEXT("MPGI"));
					}
				}
				else
				{
					UManagerLog::LogError(TEXT("Failed to create session"), TEXT("MPGI"));
					// Don't call EndSession() here as curSessionName might not be valid yet
				}
			}
			else
			{
				UManagerLog::LogError(TEXT("Local player or UniqueNetId is invalid"), TEXT("MPGI"));
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
			if (enableSessionLogging)
			{
				UManagerLog::LogInfo(TEXT("Starting session search"), TEXT("MPGI"));
			}

			searchForSessionsCompletedHandle = onlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(searchForSessionsCompletedDelegate);
			
			searchSettings = MakeShareable(new FOnlineSessionSearch());
			searchSettings->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
			searchSettings->MaxSearchResults = maxSearchResults;
			searchSettings->PingBucketSize = 50;
			searchSettings->TimeoutInSeconds = searchTimeoutSeconds;

			const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			if (localPlayer && localPlayer->GetPreferredUniqueNetId().IsValid())
			{
				if (onlineSessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), searchSettings.ToSharedRef()))
				{
					if (enableSessionLogging)
					{
						UManagerLog::LogInfo(TEXT("Session search request sent"), TEXT("MPGI"));
					}
				}
				else
				{
					UManagerLog::LogError(TEXT("Failed to start session search"), TEXT("MPGI"));
				}
			}
			else
			{
				UManagerLog::LogError(TEXT("Local player or UniqueNetId is invalid for session search"), TEXT("MPGI"));
			}
		}
	}
}

void UMPGI::JoinSessions(int sessionIndex)
	{
			if (sessionList.IsValidIndex(sessionIndex) && searchSettings.IsValid())
			{
				const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
				if (localPlayer && localPlayer->GetPreferredUniqueNetId().IsValid())
				{
					joinSessionCompletedHandle = IOnlineSubsystem::Get()->GetSessionInterface()->AddOnJoinSessionCompleteDelegate_Handle(joinSessionCompletedDelegate);
					
					IOnlineSubsystem::Get()->GetSessionInterface()->JoinSession(*localPlayer->GetPreferredUniqueNetId(), FName(*sessionList[sessionIndex].sessionName), searchSettings->SearchResults[sessionIndex]);

					UManagerLog::LogInfo(FString::Printf(TEXT("Attempting to join session %d"), sessionIndex), TEXT("MPGI"));
				}
				else
				{
			UManagerLog::LogError(TEXT("Local player or UniqueNetId is invalid for joining session"), TEXT("MPGI"));
				}
			}
			else
			{
		UManagerLog::LogWarning(TEXT("Invalid session index or search settings"), TEXT("MPGI"));
	}
}

void UMPGI::EndSession()
{
    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
    {
        if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
        {
            endSessionCompletedHandle = onlineSessionInterface->AddOnEndSessionCompleteDelegate_Handle(endSessionCompletedDelegate);
			onlineSessionInterface->EndSession(curSessionName);
			
			UManagerLog::LogInfo(TEXT("Session end request sent"), TEXT("MPGI"));
            }
        }
    }

void UMPGI::DestroySession(FName sessionName)
{
    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			destroySessionCompleteHandle = onlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(destroySessionCompleteDelegate);
			onlineSessionInterface->DestroySession(sessionName);
			
			UManagerLog::LogInfo(TEXT("Session destroy request sent"), TEXT("MPGI"));
			}
		}
	}

// Session completion callbacks
void UMPGI::HostSessionCompleted(FName sessionName, bool hostCompleted)
{
    if (hostCompleted)
	{
		UManagerLog::LogInfo(TEXT("Session hosted successfully"), TEXT("MPGI"));
		
    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
			{
				onlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(createSessionsCompletedHandle);
			}
		}
		
		// Set current session name
		curSessionName = sessionName;
		
		// For multiplayer sessions, we need to travel to the session
		// This ensures proper network setup for the multiplayer session
		TravelToSession(sessionName);
	}
	else
	{
		UManagerLog::LogError(TEXT("Failed to host session"), TEXT("MPGI"));
		curSessionName = NAME_None;
				}
}

void UMPGI::SearchForSessionsCompleted(bool searchCompleted)
{
    if (searchCompleted)
	{
		UManagerLog::LogInfo(TEXT("Session search completed"), TEXT("MPGI"));
		
		if (searchSettings.IsValid())
		{
			sessionList.Empty();
			
			for (int32 i = 0; i < searchSettings->SearchResults.Num(); i++)
			{
				FSessionInfo sessionInfo;
				sessionInfo.sessionIndex = i;
				sessionInfo.sessionName = searchSettings->SearchResults[i].Session.SessionSettings.Settings.FindRef(FName("SESSION_NAME")).Data.ToString();
				sessionInfo.hostName = searchSettings->SearchResults[i].Session.SessionSettings.Settings.FindRef(FName("HOST_NAME")).Data.ToString();
				sessionInfo.maxPlayersNum = searchSettings->SearchResults[i].Session.SessionSettings.NumPublicConnections;
				sessionInfo.curPlayersNum = sessionInfo.maxPlayersNum - searchSettings->SearchResults[i].Session.NumOpenPublicConnections;
				sessionInfo.ping = searchSettings->SearchResults[i].PingInMs;
				// sessionInfo.usePassword = searchSettings->SearchResults[i].Session.SessionSettings.Settings.FindRef(FName("USE_PASSWORD")).Data.GetValue(bUsePassword)();
				searchSettings->SearchResults[i].Session.SessionSettings.Settings.FindRef(FName("USE_PASSWORD")).Data.GetValue(sessionInfo.usePassword);

				sessionList.Add(sessionInfo);
			}
			
					UManagerLog::LogInfo(FString::Printf(TEXT("Found %d sessions"), sessionList.Num()), TEXT("MPGI"));
	}
	
			// Notify the HUD that search is complete
		if (APlayerController* playerController = GetWorld()->GetFirstPlayerController())
		{
			// Cast to our custom player controller to access the search session HUD
			if (AMPControllerPlayer* mpPlayerController = Cast<AMPControllerPlayer>(playerController))
			{
				if (mpPlayerController->GetSearchSessionHUD())
				{
					mpPlayerController->GetSearchSessionHUD()->OnSearchCompleted(true);
				}
				else
				{
					UManagerLog::LogWarning(TEXT("Search session HUD is null"), TEXT("MPGI"));
				}
			}
			else
			{
				UManagerLog::LogWarning(TEXT("Could not cast to MPControllerPlayer"), TEXT("MPGI"));
			}
		}
    }
    else
    {
		UManagerLog::LogWarning(TEXT("Session search failed"), TEXT("MPGI"));
	
			// Notify the HUD that search failed
		if (APlayerController* playerController = GetWorld()->GetFirstPlayerController())
		{
			// Cast to our custom player controller to access the search session HUD
			if (AMPControllerPlayer* mpPlayerController = Cast<AMPControllerPlayer>(playerController))
			{
				if (mpPlayerController->GetSearchSessionHUD())
				{
					mpPlayerController->GetSearchSessionHUD()->OnSearchCompleted(false);
				}
				else
				{
					UManagerLog::LogWarning(TEXT("Search session HUD is null"), TEXT("MPGI"));
				}
			}
			else
			{
				UManagerLog::LogWarning(TEXT("Could not cast to MPControllerPlayer"), TEXT("MPGI"));
			}
		}
    }


    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
    {
        if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
        {
            onlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(searchForSessionsCompletedHandle);
        }
    }
}

void UMPGI::JoinSessionCompleted(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
    if (result == EOnJoinSessionCompleteResult::Success)
    {
        UManagerLog::LogInfo(TEXT("Session joined successfully"), TEXT("MPGI"));
    }
    else
    {
        UManagerLog::LogWarning(TEXT("Failed to join session"), TEXT("MPGI"));
    }
	
	if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			onlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(joinSessionCompletedHandle);
		}
			}
	
				curSessionName = sessionName;
	
	// Travel to the session
	TravelToSession(sessionName);
}

bool UMPGI::TravelToSession(FName sessionName)
{
	if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			FString joinAddress;
			if (onlineSessionInterface->GetResolvedConnectString(sessionName, joinAddress))
			{
				UManagerLog::LogInfo(TEXT("Traveling to session"), TEXT("MPGI"));
				GetWorld()->GetFirstPlayerController()->ClientTravel(joinAddress, ETravelType::TRAVEL_Absolute);
					return true;
				}
		}
	}
	
	UManagerLog::LogError(TEXT("Failed to travel to session"), TEXT("MPGI"));
	return false;
}

FSessionInfo UMPGI::GetSessionInfo(int32 index) const
{
	if (sessionList.IsValidIndex(index))
	{
		return sessionList[index];
	}
	
	UManagerLog::LogWarning(FString::Printf(TEXT("Invalid session index: %d"), index), TEXT("MPGI"));
	return FSessionInfo();
}

void UMPGI::EndSessionCompleted(FName sessionName, bool endCompleted)
{
    if (endCompleted)
	{
		UManagerLog::LogInfo(TEXT("Session ended successfully"), TEXT("MPGI"));
	}
	else
	{
		UManagerLog::LogWarning(TEXT("Failed to end session"), TEXT("MPGI"));
	}
	
    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			onlineSessionInterface->ClearOnEndSessionCompleteDelegate_Handle(endSessionCompletedHandle);
		}
	}
	
	curSessionName = NAME_None;
			}

void UMPGI::DestroySessionCompleted(FName sessionName, bool destroyCompleted)
{
    if (destroyCompleted)
	{
		UManagerLog::LogInfo(TEXT("Session destroyed successfully"), TEXT("MPGI"));
	}
	else
	{
		UManagerLog::LogWarning(TEXT("Failed to destroy session"), TEXT("MPGI"));
	}
	
    if (IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr onlineSessionInterface = onlineSubsystem->GetSessionInterface())
		{
			onlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(destroySessionCompleteHandle);
		}
	}
}

// Level management
void UMPGI::OpenLevel(EGameLevel levelType)
{
	FString levelName = GetLevelName(levelType);
	if (!levelName.IsEmpty())
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(*levelName));
		UManagerLog::LogInfo(FString::Printf(TEXT("Opening level: %s"), *levelName), TEXT("MPGI"));
	}
	else
	{
		UManagerLog::LogError(TEXT("Invalid level type"), TEXT("MPGI"));
	}
}

FString UMPGI::GetLevelName(EGameLevel levelType) const
{
	switch (levelType)
	{
	case EGameLevel::EInit:
		return initLevelName;
	case EGameLevel::EGameplay:
		return gameplayLevelName;
	default:
		return TEXT("");
	}
}

// Session creation helpers
void UMPGI::CreateSession(const FString& sessionName, const FString& hostName, bool usePassword, const FString& password)
{
	sessionUsePassword = usePassword;
	sessionPassword = password;
	curPlayerName = hostName;
	
	// Set the host player ID when creating a session
	SetHostPlayerID(hostName);
	
	HostSession(FName(*sessionName), maxPlayersPerSession);
}

void UMPGI::StartSinglePlayerGame()
{
    UManagerLog::LogInfo(TEXT("Starting single player game"), TEXT("MPGI"));
	OpenLevel(EGameLevel::EGameplay);
}

FString UMPGI::GetSteamUsername() const
{
    // Implementation depends on Steam integration
    return TEXT("SteamUser");
}

FString UMPGI::GenerateRandomName() const
{
    // Generate a random player name
    const TArray<FString> prefixes = { TEXT("Player"), TEXT("Gamer"), TEXT("User") };
    const TArray<FString> suffixes = { TEXT("123"), TEXT("456"), TEXT("789"), TEXT("ABC"), TEXT("XYZ") };
    
    FString prefix = prefixes[FMath::RandRange(0, prefixes.Num() - 1)];
    FString suffix = suffixes[FMath::RandRange(0, suffixes.Num() - 1)];
    
    return FString::Printf(TEXT("%s%s"), *prefix, *suffix);
}

// Localization
void UMPGI::InitializeLocalization()
{
    if (!localizationManager)
	{
		localizationManager = NewObject<UManagerLocalization>(this);
	}
	
	if (localizationManager)
	{
		localizationManager->InitializeLocalization();
		UManagerLog::LogInfo(TEXT("Localization system initialized"), TEXT("MPGI"));
	}
	else
	{
		UManagerLog::LogError(TEXT("Failed to create localization manager"), TEXT("MPGI"));
	}
}

// Host detection utility
bool UMPGI::IsHost() const
{
	// Check if current player is the host by comparing with stored host ID
	if (!hostPlayerID.IsEmpty())
				{
		// Get current player's unique ID (could be Steam ID, player name, etc.)
		FString currentPlayerID = curPlayerName;
		
		// For now, use player name as ID. In a real implementation, you'd use Steam ID or similar
		return (currentPlayerID == hostPlayerID);
}

	// If no host ID is set, assume the first player is host
	// This is a fallback for single player or when host ID isn't set
	return true;
}

void UMPGI::SetHostPlayerID(const FString& hostID)
{
	hostPlayerID = hostID;
	UManagerLog::LogInfo(FString::Printf(TEXT("Host player ID set to: %s"), *hostID), TEXT("MPGI"));
}