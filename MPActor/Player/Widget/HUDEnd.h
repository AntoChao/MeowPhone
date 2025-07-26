#pragma once

#include "MPHUD.h"

#include "HUDEnd.generated.h"

class UTextBlock;
class UButton;
class UProgressBar;

UCLASS()
class UHUDEnd : public UMPHUD
{
    GENERATED_BODY()

public:
    UHUDEnd();

protected:
    virtual void NativeConstruct() override;

    // Win/Lose text
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    class UTextBlock* resultText;

    // Progression bars for both teams
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    class UProgressBar* humanProgressionBar;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    class UProgressBar* catProgressionBar;

    // Return to lobby button
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    class UButton* returnLobbyButton;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "HUD")
    class UTextBlock* returnLobbyButtonText;

public:
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetResultText(const FString& resultKey);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetHumanProgression(float percent);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetCatProgression(float percent);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetReturnLobbyButtonText(const FString& key);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void OnReturnLobbyClicked();
};