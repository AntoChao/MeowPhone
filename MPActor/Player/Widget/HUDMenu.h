#pragma once

#include "MPHUD.h"
#include "HUDMenu.generated.h"

class UButton;
class UTextBlock;
class AMPControllerPlayer;

UCLASS()
class UHUDMenu : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDMenu();

protected:
    virtual void NativeConstruct() override;

public:
    // Buttons and TextBlocks
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* continueButton;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* continueText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* optionButton;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* optionText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UButton* quitButton;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UTextBlock* quitText;

    // Localization keys
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString continueTextKey = TEXT("continue");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString optionTextKey = TEXT("option");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
    FString quitTextKey = TEXT("quit");

    // Update button texts from localization
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateButtonTexts();

    // Button handlers
    UFUNCTION()
    void OnContinueClicked();
    UFUNCTION()
    void OnOptionClicked();
    UFUNCTION()
    void OnQuitClicked();

    // Called by OptionHUD to return to menu if previousHUD is EMenu
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ReturnFromOptionHUD();
};