#include "MPCharacterCat.h"

#include "../../CommonEnum.h"
#include "../../CommonStruct.h"
#include "MPCharacterHuman.h"
#include "../Ability/MPAbility.h"

AMPCharacterCat::AMPCharacterCat()
{
    cameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Character SpringArm"));
    cameraSpringArm->SetupAttachment(RootComponent);
    cameraSpringArm->TargetArmLength = 300.0f; 
    cameraSpringArm->bUsePawnControlRotation = true;

    characterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Character Camera"));
    characterCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
}

void AMPCharacterCat::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeAllAbility();
}

// ability
void AMPCharacterCat::InitializeAllAbility()
{
	InitializeActiveAbility();
    InitializeAllPassiveAbility();
}

void AMPCharacterCat::InitializeActiveAbility()
{
	activeAbility = nullptr;
	AMPPlayerState* curPlayerState = Cast<AMPPlayerState>(GetPlayerState());
	AMPGameplay* curGameMode = UGameplayStatics::GetGameMode(GetWorld());
	
	if (curPlayerState && curGameMode)
	{
		activeAbility = curGameMode->SpawnAbility(this, curPlayerState->playerSelectedAbility)
		if (activeAbility)
		{
			activeAbility->BeInitialized(this);
		}
	}
}
void AMPCharacterCat::InitializeAllPassiveAbility()
{
	allPassiveAbilities.Empty;
	AMPGameplay* curGameMode = UGameplayStatics::GetGameMode(GetWorld());
	
	if (curGameMode)
	{
		int i = 0;
		for (EAbility eachAbilityTag : initPassiveAbilities)
		{
			allPassiveAbilities[i] = curGameMode->SpawnAbility(this, eachAbilityTag)
			if (allPassiveAbilities[i])
			{
				allPassiveAbilities[i]->BeInitialized(this);
				i ++;
			}
		}
	}
}

// interactable interface
bool AMPCharacterCat::IsInteractable(AMPCharacter* player)
{
	return !isBeingHolded;
}

FLocalizedText AMPCharacterCat::GetInteractHintText(AMPCharacter* player)
{
	if (IsInteractable(player))
	{
		switch(player->GetCharacterTeam())
		{
			case ETeam::EHuman :
			{
				return humanInteractHintText;
			}
			case ETeam::ECat :
			{
				return humanInteractHintText;
			}
		}
	}
	else
	{
		switch(player->GetCharacterTeam())
		{
			case ETeam::EHuman :
			{
				return uninteractableHumanHintText;
			}
			case ETeam::ECat :
			{
				return uninteractableCatHintText;
			}
		}
	}
}

void AMPCharacterCat::BeInteracted(AMPCharacter* player)
{
	if (IsInteractable(player))
	{
		switch(player->GetCharacterTeam())
		{
			case ETeam::EHuman :
			{
				StartedToBeHold(player);
				
				// notify humanPlayer
				humanHolding->StartHoldingCat(this);
			}
			case ETeam::ECat :
			{
				return;
				// what should cat interact with other cat
			}
		}
	}
}

bool AMPCharacterCat::CheckIfIsAbleToLook()
{	
	return true;
}
bool AMPCharacterCat::CheckIfIsAbleToMove()
{
	return !isBeingHolded && !isRubbing;
}
bool AMPCharacterCat::CheckIfIsAbleToInteract()
{
	return true;
}

void AMPCharacterCat::StartedToBeHold(AMPCharacter* humanPlayer)
{
	// update cat stats
	isBeingHolded = true;
	humanHolding = Cast<AMPCharacterHuman>(humanPlayer)
	curHoldTime = 0;
}
void AMPCharacterCat::EndToBeHold()
{
	humanHolding->StopHoldingCat();

	isBeingHolded = false;
	humanHolding = nullptr;
	curHoldTime = 0;
}
void AMPCharacterCat::Straggle()
{
	curHoldTime = curHoldTime + holdModifier;
	curHoldPercentage = curHoldTime / holdTotalTime;

	if (curHoldTime >= holdTotalTime)
	{
		EndToBeHold();
	}
}

void AMPCharacterCat::StartToRub(AMPCharacterHuman* humanToRub)
{
	isRubbing = true;
    humanRubbing = humanToRub;
}
void AMPCharacterCat::StopToRub()
{
	isRubbing = false;
    humanRubbing = nullptr;
}

// controller/ input reaction
/*	Interact() -> curCatAction
*		beingHold -> straggle
*		detectActor -> human -> interactHuman
* 		detectActor -> cat -> interactCat
* 		detectActor -> envActor -> envActor
* 		detectActor -> item -> pickup	
*/
void AMPCharacterCat::Interact()
{
	if (CheckIfIsAbleToInteract)
	{
		if (isRubbing)
		{
			StopToRub();
		}
		else if (isBeingHolded)
		{
			Straggle();
		}
		else if (detectInteractableActor)
		{
			detectInteractableActor->BeInteracted(this);
		}
	}
}