#pragma once

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