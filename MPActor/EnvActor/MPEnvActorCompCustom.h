#pragma once

#include "CoreMinimal.h"
#include "MPEnvActorComp.h"

#include "MPEnvActorCompCustom.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AMPEnvActorCompCustom : public AMPEnvActorComp
{
    GENERATED_BODY()

public:
    AMPEnvActorCompCustom();

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