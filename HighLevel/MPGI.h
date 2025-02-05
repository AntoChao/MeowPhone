#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MPGI.generated.h"

enum class ELanguage : uint8;
struct FSessionInfo;
class FOnlineSessionSearch;
class OnlineSessionPtr;

class FOnCreateSessionsCompleteDelegate;
class FOnFindSessionsCompleteDelegate;
class FOnJoinSessionCompleteDelegate;
class FOnEndSessionCompleteDelegate;
class FOnDestroySessionCompleteDelegate;

class FDelegateHandle;

UCLASS(minimalapi)
class MEOWPHONE_API UMPGI : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UMPGI();

// common game instance section 
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
		ELanguage gameLanguage;

	UPROPERTY(BlueprintReadWrite, Category = "Common Properties")
		FName curPlayerName;

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
		FString GenerateRandomUserString();
		
	UFUNCTION(BlueprintCallable, Category = "SaveFile Methods")
		void SaveGame();
	
	UFUNCTION(BlueprintCallable, Category = "SaveFile Methods")
		void LoadGame();
		
// session section
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Session Properties")
		TSharedPtr<FOnlineSessionSearch> searchSettings;

	UPROPERTY(BlueprintReadWrite, Category = "Session Properties")
		OnlineSessionPtr sessionInterface;

	UPROPERTY(BlueprintReadWrite, Category = "Session Properties")
		bool foundSuccessed;
	
	UPROPERTY(BlueprintReadWrite, Category = "Session Properties")
		TArray<FSessionInfo> sessionList;
		
	UPROPERTY(BlueprintReadWrite, Category = "Session Properties")
		FName curSessionName;

public:
	UFUNCTION(BlueprintCallable, Category = "Session Methods")
		void HostSession(FName sessionName, int numPlayers);

	UFUNCTION(BlueprintCallable, Category = "Session Methods")
		void SearchForSessions();

	UFUNCTION(BlueprintCallable, Category = "Session Methods")
		void JoinSession(int sessionIndex);

	UFUNCTION(BlueprintCallable, Category = "Session Methods")
		void EndSession();

	UFUNCTION(BlueprintCallable, Category = "Session Methods")
		void DestroySession(FName sessionName);

// session complete section
protected:
	FOnCreateSessionsCompleteDelegate createSessionsCompletedDelegate;
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

	UFUNCTION(BlueprintCallable, Category = "Session Complete Methods")
		void JoinSessionCompleted(FName sessionName);

	UFUNCTION(BlueprintCallable, Category = "Session Complete Methods")
		bool TravelToSession(FName sessionName)
	
	UFUNCTION(BlueprintCallable, Category = "Session Complete Methods")
		void EndSessionCompleted(FName sessionName, bool endCompleted);

	UFUNCTION(BlueprintCallable, Category = "Session Complete Methods")
		void DestroySessionCompleted(FName sessionName, bool destroyCompleted);

// getter && setter
public:
	UFUNCTION(BlueprintCallable, Category = " getter && setter")
		ELanguage getGameLanguage();

	UFUNCTION(BlueprintCallable, Category = " getter && setter")
		FName getCurPlayerName();
};
