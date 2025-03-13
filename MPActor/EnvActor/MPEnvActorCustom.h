#pragma once

#include "CoreMinimal.h"
#include "MPEnvActor.h"

#include "MPEnvActorCustom.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMPEnvActorCustom : public AMPEnvActor
{
    GENERATED_BODY()

public:
    AMPEnvActorCustom();

    // interactable interface
public:
    virtual bool IsInteractable(AMPCharacter* targetActor) override;

    virtual FText GetInteractHintText(AMPCharacter* targetActor) override;

    // interact specific
protected:
    virtual void ApplyInteractEffectDirect(AMPCharacter* targetActor) override;

    virtual void ApplyInteractEffectDurationEffect() override;

    // implement in blueprint cuz the variety
    UFUNCTION(BlueprintImplementableEvent, Category = "Custom Interact Function")
    void CustomInteractEffectDirect();

    UFUNCTION(BlueprintImplementableEvent, Category = "Custom Interact Function")
    void CustomInteractEffectDuration();

};