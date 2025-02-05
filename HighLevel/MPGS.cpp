#include "MPGS.h"
#include "../CommonEnum.h"

#include "../MPActor/Character/MPCharacterCat.h"
#include "../MPActor/Character/MPCharacterHuman.h"
#include "../MPActor/Item/MPItem.h"
#include "../MPActor/EnvActor/MPEnvActor.h"

AMPGS::AMPGS()
{
	return;
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