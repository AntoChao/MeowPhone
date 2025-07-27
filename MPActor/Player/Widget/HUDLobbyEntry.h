#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../../CommonEnum.h"
#include "HUDLobbyEntry.generated.h"

class AMPControllerPlayer;
class AMPAIController;

UCLASS()
class MEOWPHONE_API UHUDLobbyEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	UHUDLobbyEntry(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	// UI Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* playerNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* readyStatusText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* removeButton;

	// Player data
	UPROPERTY(BlueprintReadOnly, Category = "Player Data")
	FString playerName;

	UPROPERTY(BlueprintReadOnly, Category = "Player Data")
	bool isBot;

	UPROPERTY(BlueprintReadOnly, Category = "Player Data")
	bool isReady;

	UPROPERTY(BlueprintReadOnly, Category = "Player Data")
	ETeam playerTeam;

	UPROPERTY(BlueprintReadOnly, Category = "Player Data")
	int32 playerIndex;

	UPROPERTY(BlueprintReadOnly, Category = "Player Data")
	AMPControllerPlayer* playerController;

	UPROPERTY(BlueprintReadOnly, Category = "Player Data")
	AMPAIController* aiController;

	// Localization keys
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString readyTextKey = TEXT("READY");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString notReadyTextKey = TEXT("NOT_READY");

	// Callback for remove button
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoveBotClicked, int32, playerIndex);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRemoveBotClicked OnRemoveBotClicked;

public:
	// Initialize the entry with player data
	UFUNCTION(BlueprintCallable, Category = "Lobby Entry")
	void InitializeEntry(const FString& inPlayerName, bool inIsBot, bool inIsReady, 
		ETeam inPlayerTeam, int32 inPlayerIndex, AMPControllerPlayer* inPlayerController, 
		AMPAIController* inAIController);

	// Update ready status
	UFUNCTION(BlueprintCallable, Category = "Lobby Entry")
	void UpdateReadyStatus(bool inIsReady);

	// Update player name
	UFUNCTION(BlueprintCallable, Category = "Lobby Entry")
	void UpdatePlayerName(const FString& inPlayerName);

	// Update host visibility (show/hide remove button for bots)
	UFUNCTION(BlueprintCallable, Category = "Lobby Entry")
	void UpdateHostVisibility(bool isHost);

protected:
	// Button click handler
	UFUNCTION(BlueprintCallable, Category = "Lobby Entry")
	void OnRemoveButtonClicked();

	// Update UI based on current data
	UFUNCTION(BlueprintCallable, Category = "Lobby Entry")
	void UpdateUI();
}; 