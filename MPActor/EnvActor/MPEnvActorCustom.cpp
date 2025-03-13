#include "MPEnvActorCustom.h"

#include "../../CommonStruct.h"

AMPEnvActorCustom::AMPEnvActorCustom()
{
	envActorTag = EEnvActor::ECustom;
	envActorType = EEnvActorType::EDurationInteract;
}

bool AMPEnvActorCustom::IsInteractable(AMPCharacter* targetActor)
{
	return true;
}

FText AMPEnvActorCustom::GetInteractHintText(AMPCharacter* targetActor)
{
	return envActorName;
}

void AMPEnvActorCustom::ApplyInteractEffectDirect(AMPCharacter* targetActor)
{
	CustomInteractEffectDirect();
}

void AMPEnvActorCustom::ApplyInteractEffectDurationEffect()
{
	CustomInteractEffectDuration();
}
