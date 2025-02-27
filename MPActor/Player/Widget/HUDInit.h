#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "HUDInit.generated.h"

class UButton;
class UTextBlock;
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
    // local Text
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
    FText startGameTXT;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
    FText optionTXT;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
    FText creditTXT;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Properties")
    FText quitTXT;

    // text components
    UTextBlock* startGameTXTBlock;
    UTextBlock* optionTXTBlock;
    UTextBlock* creditTXTBlock;
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
};