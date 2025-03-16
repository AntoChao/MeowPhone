#include "MPEnvActorCompCustom.h"

#include "../../CommonStruct.h"

AMPEnvActorCompCustom::AMPEnvActorCompCustom()
{
	envActorTag = EEnvActor::ECustom;
	envActorType = EEnvActorType::EDurationInteract;
}

bool AMPEnvActorCompCustom::IsInteractable(AMPCharacter* targetActor)
{
	return true;
}

FText AMPEnvActorCompCustom::GetInteractHintText(AMPCharacter* targetActor)
{
	return envActorName;
}

void AMPEnvActorCompCustom::ApplyInteractEffectDirect(AMPCharacter* targetActor)
{
	CustomInteractEffectDirect();
}

void AMPEnvActorCompCustom::ApplyInteractEffectDurationEffect()
{
	CustomInteractEffectDuration();
}
