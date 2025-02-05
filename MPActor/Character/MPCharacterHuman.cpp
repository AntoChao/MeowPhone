#include "MPCharacterCat.h"

#include "../../CommonEnum.h"
#include "../../CommonStruct.h"
#include "MPCharacterCat.h"

AMPCharacterHuman::AMPCharacterHuman()
{
	// Create the first-person camera
	characterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Character Camara"));
	characterCamera->SetupAttachment(RootComponent);
	characterCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	characterCamera->bUsePawnControlRotation = true;
}

// interactable interface
bool AMPCharacterHuman::IsInteractable(AMPCharacter* )
{
	// only cat can interact with human?
	return player->GetCharacterTeam == ETeam::ECat; 
}

FLocalizedText AMPCharacterHuman::GetInteractHintText(AMPCharacter* player)
{
	if (IsInteractable(player))
	{
		return catInteractHintText;
	}
	else 
	{
		return uninteractableCatHintText;
	}
}

void AMPCharacterHuman::BeInteracted(AMPCharacter* player)
{
	if (IsInteractable(player))
	{
		switch(player->GetCharacterTeam())
		{
			case ETeam::ECat :
			{
				AMPCharacterCat* cat = Cast<AMPCharacterCat>(player);
				if (cat)
				{
					StartToBeRubbed(rubbedCat);
					cat->StartToRub(this);
				}
			}
		}
	}
}

bool AMPCharacterHuman::CheckIfIsAbleToLook()
{
	return true;
}
bool AMPCharacterHuman::CheckIfIsAbleToMove() 
{
	return !isRubbedByCat;
}
bool AMPCharacterHuman::CheckIfIsAbleToInteract() 
{
	return !isHoldingCat;
}

void AMPCharacterHuman::StartHoldingCat(AMPCharacterCat* catToHold)
{
	isHoldingCat = true;
	catHolding = catToHold;
}

void AMPCharacterHuman::StopHoldingCat()
{
	isHoldingCat = false;
	catHolding = nullptr;
}

void AMPCharacterHuman::StartToBeRubbed(AMPCharacterCat* rubbedCat)
{
	isRubbedByCat = true;
	catRubbing = rubbedCat;
}
void AMPCharacterHuman::StopToBeRubbed()
{
	isRubbedByCat = false;
	catRubbing = nullptr;
}

// controller/ input reaction
/*	Interact() -> curHumanAction
*		holdingCat -> drop cat
*		detectActor -> human -> interactHuman
* 		detectActor -> cat -> interactCat
* 		detectActor -> envActor -> envActor
* 		detectActor -> item -> pickup	
*/
void AMPCharacterHuman::Interact()
{
	if (isRubbedByCat)
	{
		StopToBeRubbed();
	}
	else 
	{
		if (CheckIfIsAbleToInteract())
		{
			if (detectInteractableActor)
			{
				detectInteractableActor->BeInteracted(this);
			}
		}
	}
}
