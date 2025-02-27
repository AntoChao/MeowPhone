#include "MPGS.h"
#include "../CommonEnum.h"

#include "../MPActor/Character/MPCharacterCat.h"
#include "../MPActor/Character/MPCharacterHuman.h"
#include "../MPActor/Item/MPItem.h"
#include "../MPActor/EnvActor/MPEnvActor.h"
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