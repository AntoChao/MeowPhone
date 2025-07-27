#pragma once

// [Meow-Phone Project]
//
// This is the Game Instance class for the project. The Game Instance is a persistent object
// that exists for the entire duration of the game's execution. It is the central hub for
// managing systems that should not be tied to a specific level, such as online sessions,
// save/load operations, localization, and the player's identity.
//
// How to utilize in Blueprint:
// 1. This class, and its derived Blueprint, must be set as the default "Game Instance Class" in Project Settings.
// 2. From any Blueprint, you can get a reference to the Game Instance by using the "Get Game Instance" node and casting it to your `BP_MPGI` Blueprint.
// 3. UI is a heavy user of this class. For example:
//    - The Main Menu UI calls `CreateSession`, `SearchForSessions`, and `JoinSessions` to handle multiplayer logic.
//    - The Settings/Options UI calls `SetCurrentLanguage` to change the game's language.
//    - Any part of the game that needs the player's name will call `GetCurPlayerName`.
//    - `OpenLevel` is called to transition between the main menu and the gameplay level.
// 4. Before using any session-related features, the appropriate `...Completed` delegates must be bound in the calling Blueprint (e.g., in the Search Session widget) to handle the asynchronous results.
//
// Necessary things to define:
// - In the `BP_MPGI` Blueprint derived from this class, you must:
//   - Set the `initLevelName` and `gameplayLevelName` properties to the correct names of your map files.
//   - Assign a valid `UManagerLocalization` Blueprint to the `localizationManager` property. This is crucial for all in-game text to work.
//
// How it interacts with other classes:
// - UGameInstance: The base class from which it derives its persistent nature.
// - OnlineSubsystem / IOnlineSession: This class is the primary interface with Unreal's Online Subsystem, wrapping its complex, asynchronous functions (like creating, finding, and joining sessions) into more manageable Blueprint-callable events.
// - UManagerLocalization: It owns and initializes the localization manager, making it globally accessible.
// - MPSave: It creates, loads, and saves instances of the `UMPSave` class to handle game progress persistence.
// - FSessionInfo (Struct): A custom struct used to simplify and store the results from an online session search.
// - EGameLevel / ELanguage (Enums): Uses these enums to provide a more readable and less error-prone way of specifying levels and languages.
// - HUDs / UI Widgets: The UI is the main "client" of this class, driving most of its functionality based on user input.

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Templates/SharedPointer.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MPGI.generated.h"

enum class ELanguage : uint8;
enum class EGameLevel : uint8;
struct FSessionInfo;
class UManagerLocalization;

class FDelegateHandle;

UCLASS(minimalapi)
class UMPGI : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UMPGI();
	virtual ~UMPGI();

// common game instance section 
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
		ELanguage gameLanguage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common Properties")
		FString curPlayerName;

public:
	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "Common Methods")
		virtual void Shutdown() override;

// save file section
public:
	UFUNCTION(BlueprintCallable, Category = "SaveFile Methods")
		void CreateSaveFile();
		
	UFUNCTION(BlueprintCallable, Category = "SaveFile Methods")
		void SaveGame();
	
	UFUNCTION(BlueprintCallable, Category = "SaveFile Methods")
		void LoadGame();

// getter && setter
public:
	// Language management methods
	UFUNCTION(BlueprintCallable, Category = "Localization")
		ELanguage GetCurrentLanguage() const;
		
	UFUNCTION(BlueprintCallable, Category = "Localization")
		void SetCurrentLanguage(ELanguage newLanguage);
		
	// Legacy methods for backward compatibility
	UFUNCTION(BlueprintCallable, Category = "Localization")
		ELanguage getGameLanguage() { return GetCurrentLanguage(); }

	UFUNCTION(BlueprintCallable, Category = " getter && setter")
		FString GetCurPlayerName();

// Level management
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Management")
		FString initLevelName = TEXT("InitLevel");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Management")
		FString gameplayLevelName = TEXT("GameplayLevel");

	// Session configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Management")
		int32 maxPlayersPerSession = 8;

public:
	UFUNCTION(BlueprintCallable, Category = "Level Management")
		void OpenLevel(EGameLevel levelType);
	
	UFUNCTION(BlueprintCallable, Category = "Level Management")
		FString GetLevelName(EGameLevel levelType) const;

// Session creation
public:
	UFUNCTION(BlueprintCallable, Category = "Session Management")
		void CreateSession(const FString& sessionName, const FString& hostName, bool usePassword, const FString& password);
	
	UFUNCTION(BlueprintCallable, Category = "Session Management")
		void StartSinglePlayerGame();
	
	UFUNCTION(BlueprintCallable, Category = "Session Management")
		FString GetSteamUsername() const;
	
	UFUNCTION(BlueprintCallable, Category = "Session Management")
		FString GenerateRandomName() const;


// Steam Online Subsystem Session Control
// session section
protected:

	// UPROPERTY(BlueprintReadWrite, Category = "Session Properties")
	TSharedPtr<FOnlineSessionSearch> searchSettings;

	// Removed unused sessionInterface - each function gets it fresh from onlineSubsystem

	UPROPERTY(BlueprintReadWrite, Category = "Session Properties")
	bool foundSucceeded;

	UPROPERTY(BlueprintReadWrite, Category = "Session Properties")
	TArray<FSessionInfo> sessionList;

	UPROPERTY(BlueprintReadWrite, Category = "Session Properties")
	FName curSessionName;

	UPROPERTY(BlueprintReadWrite, Category = "Session Properties")
	FString hostPlayerID;

	UPROPERTY(BlueprintReadWrite, Category = "Session Properties")
	int32 sessionSize;

	// Session configuration constants
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Properties")
	int32 maxSearchResults = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Properties")
	float searchTimeoutSeconds = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Properties")
	bool allowFriendsOnlyJoin = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Properties")
	bool allowJoinInProgress = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session Properties")
	bool enableSessionLogging = true;

	// Password protection for sessions
	UPROPERTY()
	bool sessionUsePassword = false;

	UPROPERTY()
	FString sessionPassword = TEXT("");

public:
	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	void HostSession(FName sessionName, int numPlayers);

	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	void SearchForSessions();

	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	void JoinSessions(int sessionIndex);

	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	void EndSession();

	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	void DestroySession(FName sessionName);

	// session complete section
protected:
	FOnCreateSessionCompleteDelegate createSessionsCompletedDelegate;
	FOnFindSessionsCompleteDelegate searchForSessionsCompletedDelegate;
	FOnJoinSessionCompleteDelegate joinSessionCompletedDelegate;
	FOnEndSessionCompleteDelegate endSessionCompletedDelegate;
	FOnDestroySessionCompleteDelegate destroySessionCompleteDelegate;

	FDelegateHandle createSessionsCompletedHandle;
	FDelegateHandle searchForSessionsCompletedHandle;
	FDelegateHandle joinSessionCompletedHandle;
	FDelegateHandle endSessionCompletedHandle;
	FDelegateHandle destroySessionCompleteHandle;

	UFUNCTION(BlueprintCallable, Category = "Session Complete Methods")
	void HostSessionCompleted(FName sessionName, bool hostCompleted);

	UFUNCTION(BlueprintCallable, Category = "Session Complete Methods")
	void SearchForSessionsCompleted(bool searchCompleted);

	void JoinSessionCompleted(FName sessionName, EOnJoinSessionCompleteResult::Type result);

	UFUNCTION(BlueprintCallable, Category = "Session Complete Methods")
	bool TravelToSession(FName sessionName);

	UFUNCTION(BlueprintCallable, Category = "Session Complete Methods")
	void EndSessionCompleted(FName sessionName, bool endCompleted);

	UFUNCTION(BlueprintCallable, Category = "Session Complete Methods")
	void DestroySessionCompleted(FName sessionName, bool destroyCompleted);

	// Session utility methods
	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	bool IsInSession() const { return !curSessionName.IsNone(); }

	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	bool IsMultiplayerMode() const { return !curSessionName.IsNone(); }

	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	bool IsSinglePlayerMode() const { return curSessionName.IsNone(); }
	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	FName GetCurrentSessionName() const { return curSessionName; }
	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	void ClearSessionList() { sessionList.Empty(); }
	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	void ClearCurrentSession() { curSessionName = NAME_None; }

public:
	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	int32 GetSessionListCount() const { return sessionList.Num(); }
	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	FSessionInfo GetSessionInfo(int32 index) const;

	// Localization
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	UManagerLocalization* localizationManager;

	UFUNCTION(BlueprintCallable, Category = "Localization")
	void InitializeLocalization();

	UFUNCTION(BlueprintCallable, Category = "Localization")
	UManagerLocalization* GetLocalizationManager() const { return localizationManager; }

	// Host detection utility
	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	bool IsHost() const;

	// Set host player ID (called when session is created)
	UFUNCTION(BlueprintCallable, Category = "Session Methods")
	void SetHostPlayerID(const FString& hostID);

};