#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDInit.generated.h"

class UButton;
class UTextBlock;
class UCanvasPanel;
struct FLocalizedText;

/* Init widget
* 4 buttoms
* start game
* options
* creadit
* quit
*/

UCLASS()
class UHUDInit : public UMPHUD
{
    GENERATED_BODY()

// common Init HUD 
protected:
    // Root widget (required for all UserWidgets)
    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* rootCanvas;
    
    // Text keys for localization
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
    FString startGameTextKey = TEXT("HUD_INIT_START_GAME");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
    FString optionTextKey = TEXT("HUD_INIT_OPTIONS");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
    FString creditTextKey = TEXT("HUD_INIT_CREDITS");
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
    FString quitTextKey = TEXT("HUD_INIT_QUIT");

    // text components
    UPROPERTY(meta = (BindWidget))
    UTextBlock* startGameTXTBlock;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* optionTXTBlock;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* creditTXTBlock;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* quitTXTBlock;

    // buttons
    UPROPERTY(meta = (BindWidget))
    UButton* generalSessionButton;
    UPROPERTY(meta = (BindWidget))
    UButton* optionButton;
    UPROPERTY(meta = (BindWidget))
    UButton* creditButton;
    UPROPERTY(meta = (BindWidget))
    UButton* quitButton;

    // bind functions
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
    void GoSessionGeneralWidget();
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
    void GoOptionWidget();
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
    void GoCreditWidget();
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
    void QuitGame();
    
    // Override parent functions
    virtual void UpdateTexts() override;
    virtual bool ValidateRootWidget() override;
    virtual void NativeDestruct() override;
};