#pragma once

// [Meow-Phone Project]
//
// This is the base class for nearly all UI Widget Blueprints in the project. Its primary
// purpose is to provide common functionality and references that every UI screen needs,
// such as a pointer to the Game Instance and the owning Player Controller. It also
// contains the core logic for handling language changes.
//
// How to utilize in Blueprint:
// - You should not create a widget directly from this class. Instead, all other `UHUD...` classes inherit from it.
// - Any child Blueprint that inherits from a class derived from `UMPHUD` will gain access to the `curGameInstance` and `owner` (Player Controller) variables automatically.
// - It provides the `GetLocalizedText` helper function, which simplifies getting text from the localization manager for all child classes.
// - In `NativeConstruct`, it automatically subscribes to language change events. When the language is changed in the options menu, the `OnLanguageChanged` function is called on every active widget that inherits from this class, which in turn calls `UpdateTexts`.
//
// Necessary things to define:
// - Child classes are expected to override the `UpdateTexts` function to implement their specific logic for refreshing text when the language changes.
//
// How it interacts with other classes:
// - UUserWidget: The base class for all UMG widgets.
// - AMPControllerPlayer: It holds a reference to the `owner` player controller that created it.
// - UMPGI (Game Instance): It holds a reference to the game instance for easy access to global systems.
// - UManagerLocalization: It interacts directly with the localization manager to handle language change subscriptions and to provide the `GetLocalizedText` helper.
// - All other `UHUD...` classes: Every other UI class in the project inherits from this one to gain its foundational features.

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
    virtual void SetOwner(AMPControllerPlayer* theOwner);
    
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