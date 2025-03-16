#include "MPGS.h"
#include "../CommonEnum.h"

#include "../MPActor/Character/MPCharacterCat.h"
#include "../MPActor/Character/MPCharacterHuman.h"
#include "../MPActor/Item/MPItem.h"
#include "../MPActor/EnvActor/MPEnvActorComp.h"
#include "Net/UnrealNetwork.h"

AMPGS::AMPGS()
{
	return;
}

void AMPGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMPGS, allHumans);
	DOREPLIFETIME(AMPGS, allCats);
	DOREPLIFETIME(AMPGS, allItems);
	DOREPLIFETIME(AMPGS, allEnvActors);

	DOREPLIFETIME(AMPGS, isMostPlayerReady);

	DOREPLIFETIME(AMPGS, curReadyTime);
	DOREPLIFETIME(AMPGS, curPrepareTime);
	DOREPLIFETIME(AMPGS, curGameplayTime);
	DOREPLIFETIME(AMPGS, curMPProgression);
	DOREPLIFETIME(AMPGS, curMPProgressionPercentage);
	
}

void AMPGS::BeginPlay()
{
	Super::BeginPlay();
}

void AMPGS::UpdateMPProgression(int modifier)
{
	curMPProgression += modifier;
	curMPProgressionPercentage = curMPProgression / totalMPProgression;
}