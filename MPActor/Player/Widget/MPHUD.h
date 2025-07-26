#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "MPHUD.generated.h"

class UMPGI;
class AMPControllerPlayer;
enum class EHUDType : uint8;
enum class EAbility : uint8;

UCLASS()
class UMPHUD : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // common HUD properties
protected:
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "HUD Properties")
    EHUDType curHUDType;

    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
    UMPGI* curGameInstance;

    UPROPERTY(BlueprintReadWrite, Category = "HUD Properties")
    AMPControllerPlayer* owner;

public:
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
    void SetOwner(AMPControllerPlayer* theOwner);
    
    // Localization functions - to be implemented by child classes
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
    virtual void UpdateTexts();
    
    virtual void OnLanguageChanged();
    
    // Helper function to get localized text
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
    FText GetLocalizedText(const FString& textKey) const;
    
    // Subscribe to language changes
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
    void SubscribeToLanguageChanges();
    
    // Unsubscribe from language changes
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
    void UnsubscribeFromLanguageChanges();
    
    // Validate root widget (to be implemented by child classes)
    UFUNCTION(BlueprintCallable, Category = "HUD Methods")
    virtual bool ValidateRootWidget();
    

};