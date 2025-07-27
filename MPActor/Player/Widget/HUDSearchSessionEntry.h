#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../../CommonEnum.h"
#include "HUDSearchSessionEntry.generated.h"

UCLASS()
class MEOWPHONE_API UHUDSearchSessionEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	UHUDSearchSessionEntry(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	// Session data
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* sessionNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* hostNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* currentPlayersNumberText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* maxPlayersNumberText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* pingText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* joinButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* joinButtonText;

	// Session information
	UPROPERTY(BlueprintReadOnly)
	FString sessionName;

	UPROPERTY(BlueprintReadOnly)
	FString hostName;

	UPROPERTY(BlueprintReadOnly)
	int32 currentPlayersNumber;

	UPROPERTY(BlueprintReadOnly)
	int32 maxPlayersNumber;

	UPROPERTY(BlueprintReadOnly)
	int32 ping;

	UPROPERTY(BlueprintReadOnly)
	bool availableToJoin;

	UPROPERTY(BlueprintReadOnly)
	int32 sessionIndex;

	// Callback for join button
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoinSessionClicked, int32, sessionIndex);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnJoinSessionClicked OnJoinSessionClicked;

public:
	// Initialize the entry with session data
	UFUNCTION(BlueprintCallable, Category = "Session Entry")
	void InitializeEntry(const FString& inSessionName, const FString& inHostName, 
		int32 inCurrentPlayersNumber, int32 inMaxPlayersNumber, int32 inPing, bool inAvailableToJoin, int32 inSessionIndex);

	// Update availability
	UFUNCTION(BlueprintCallable, Category = "Session Entry")
	void UpdateAvailability(bool inAvailableToJoin);

	// Update ping
	UFUNCTION(BlueprintCallable, Category = "Session Entry")
	void UpdatePing(int32 inPing);

	// Update player count
	UFUNCTION(BlueprintCallable, Category = "Session Entry")
	void UpdatePlayerCount(int32 inCurrentPlayersNumber);

protected:
	// Button click handler
	UFUNCTION(BlueprintCallable, Category = "Session Entry")
	void OnJoinButtonClicked();

	// Update button state based on availability
	UFUNCTION(BlueprintCallable, Category = "Session Entry")
	void UpdateButtonState();
}; 