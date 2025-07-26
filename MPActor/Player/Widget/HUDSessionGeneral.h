#pragma once

#include "CoreMinimal.h"
#include "MPHUD.h"
#include "CommonEnum.h"
#include "HUDSessionGeneral.generated.h"

class UTextBlock;
class UButton;
class UCanvasPanel;

/**
 * HUD for session navigation - simple hub for session management
 * Features: Create session, search session, back to init
 */
UCLASS()
class MEOWPHONE_API UHUDSessionGeneral : public UMPHUD
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // Override parent functions
    virtual void UpdateTexts() override;
    virtual bool ValidateRootWidget() override;

private:
    // Root widget
    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* rootCanvas;

    // Main buttons
    UPROPERTY(meta = (BindWidget))
    UButton* createSessionButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* searchSessionButton;
    
    UPROPERTY(meta = (BindWidget))
    UButton* backButton;

    // Localization keys
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString createSessionTextKey = TEXT("SESSION_CREATE");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString searchSessionTextKey = TEXT("SESSION_SEARCH");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization Keys")
    FString backTextKey = TEXT("SESSION_BACK");

    // Button click functions
    UFUNCTION()
    void OnCreateSessionClicked();
    
    UFUNCTION()
    void OnSearchSessionClicked();
    
    UFUNCTION()
    void OnBackClicked();
};