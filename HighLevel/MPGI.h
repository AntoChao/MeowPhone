#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Templates/SharedPointer.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MPGI.generated.h"

enum class ELanguage : uint8;
struct FSessionInfo;

class FDelegateHandle;

UCLASS(minimalapi)
class UMPGI : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UMPGI();

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
		FString GenerateRandomUserString();
		
	UFUNCTION(BlueprintCallable, Category = "SaveFile Methods")
		void SaveGame();
	
	UFUNCTION(BlueprintCallable, Category = "SaveFile Methods")
		void LoadGame();

// getter && setter
public:
	UFUNCTION(BlueprintCallable, Category = " getter && setter")
		ELanguage getGameLanguage();

	UFUNCTION(BlueprintCallable, Category = " getter && setter")
		void setGameLanguage(ELanguage newLanguage);

	UFUNCTION(BlueprintCallable, Category = " getter && setter")
		FString getCurPlayerName();
};

/* abandon logic, gonna use plugin online sybsystem instead

			
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
	

*/