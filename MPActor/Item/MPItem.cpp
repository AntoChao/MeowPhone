#include "MPItem.h"

AMPItem::AMPItem()
{
	
}

// interactable interface
bool AMPItem::IsInteractable(AMPCharacter* player)
{
	return;
}

FLocalizedText AMPItem::GetInteractHintText() 
{
	return;
}

void AMPItem::BeInteracted(AMPCharacter* player)
{
	return;
}

// initialize / interact
void AMPItem::BeInitialized(AMPCharacter* player)
{
	return;
}

void AMPItem::BePickedUp(AMPCharacter* player)
{
	return;
}
void AMPItem::BeDroped(AMPCharacter* player)
{
	return;
}
// usage
void AMPItem::BeUsed(AMPCharacter* player, AActor* targetActor)
{
	return;
}
void AMPItem::ApplyUsageEffect(AMPCharacter* player, AActor* targetActor)
{
	return;
}
void AMPItem::ExpireUsageEffect(AMPCharacter* player, AActor* targetActor)
{
	return;
}
void AMPItem::StartCooldown(AMPCharacter* player, AActor* targetActor)
{
	return;
}
void AMPItem::EndCooldown(AMPCharacter* player, AActor* targetActor)
{
	return;
}