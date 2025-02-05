#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"

#include "MPHUDInit.generated.h"

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
class UMPHUDInit : public UMPHUD
{
	GENERATED_BODY()

public:
    UMPHUDInit();

// common Init HUD 
protected :
    // local Text
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        FLocalizedText startGameTXT;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        FLocalizedText optionTXT;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        FLocalizedText creditTXT;
    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
        FLocalizedText quitTXT;

    // text components
    UTextBlock* startGameTXTBlock;
    UTextBlock* optionTXTBlock;
    UTextBlock* creditTXTBlock;
    UTextBlock* quitTXTBlock;

    // buttons
    UPROPERTY(meta = (BindWidget))
        UButton* generalSessionButton
    UPROPERTY(meta = (BindWidget))
        UButton* optionButton
    UPROPERTY(meta = (BindWidget))
        UButton* creditButton
    UPROPERTY(meta = (BindWidget))
        UButton* quitButton
    
    // bind functions
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
	    void GoSessionGeneralWidget();
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
	    void GoOptionWidget();
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
	    void GoCreditWidget();
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
	    void QuitGame();
}