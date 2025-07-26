#pragma once

#include "MPHUD.h"
#include "HUDLobby.generated.h"

class UScrollBox;
class UButton;
class UTextBlock;
class UHUDLobbyEntry;

UCLASS()
class MEOWPHONE_API UHUDLobby : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDLobby();

protected:
    virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	// UI Components
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UScrollBox* humanPlayersScrollBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UScrollBox* catPlayersScrollBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* humanJoinButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* humanJoinButtonText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* catJoinButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* catJoinButtonText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* humanAddButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* humanAddButtonText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* catAddButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* catAddButtonText;

	// Localization keys
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString joinHumanTextKey = TEXT("JOIN_HUMAN");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString joinCatTextKey = TEXT("JOIN_CAT");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString addHumanTextKey = TEXT("ADD_HUMAN");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	FString addCatTextKey = TEXT("ADD_CAT");

	// Lobby entry class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby List")
	TSubclassOf<UHUDLobbyEntry> lobbyEntryClass;

	// Lobby state
	UPROPERTY(BlueprintReadOnly, Category = "Lobby State")
	bool isReady;

	UPROPERTY(BlueprintReadOnly, Category = "Lobby State")
	bool isHost;

	UPROPERTY(BlueprintReadOnly, Category = "Lobby State")
	ETeam currentTeam;

	// Player lists
	UPROPERTY(BlueprintReadOnly, Category = "Player Lists")
	TArray<UHUDLobbyEntry*> humanPlayersList;

	UPROPERTY(BlueprintReadOnly, Category = "Player Lists")
	TArray<UHUDLobbyEntry*> catPlayersList;

public:
	// Update player lists
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void UpdatePlayerLists();

	// Clear player lists
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void ClearPlayerLists();

	// Update ready state
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void UpdateReadyState(bool inIsReady);

	// Update host visibility
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void UpdateHostVisibility();

	// Update team-specific button visibility
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void UpdateTeamButtonVisibility();

protected:
	// Button click handlers
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void OnHumanJoinButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void OnCatJoinButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void OnHumanAddButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void OnCatAddButtonClicked();

	// Entry event handlers
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void OnRemoveBotClicked(int32 playerIndex);

	// Update UI state
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	void UpdateUI();

	// Create lobby entry widget
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	UHUDLobbyEntry* CreateLobbyEntry(const FString& playerName, bool isBot, bool isReady, 
		ETeam team, int32 playerIndex, class AMPControllerPlayer* playerController, 
		class AMPAIController* aiController);

	// Get current team from player controller
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	ETeam GetCurrentTeam() const;

	// Check if current player is host
	UFUNCTION(BlueprintCallable, Category = "Lobby Management")
	bool IsHost() const;

	// Delegate for team changes
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamChanged, ETeam, NewTeam);
	UPROPERTY(BlueprintAssignable, Category = "Lobby Events")
	FOnTeamChanged OnTeamChanged;
};