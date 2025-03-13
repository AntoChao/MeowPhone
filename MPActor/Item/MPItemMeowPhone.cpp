#include "MPItem.h"

#include "../../CommonStruct.h"

#include "../../HighLevel/MPGS.h"

AMPItemMeowPhone::AMPItemMeowPhone()
{
	
}

void AMPItemMeowPhone::ApplyUsageEffectDirect(AActor* targetActor)
{
	if (GetWorld())
	{
		AMPGS* curGameState = Cast<AMPGS>(GetWorld()->GetGameState());

		if (curGameState)
		{
			curGameState->UpdateMPProgression(theMPModifierNum);
		}
	}
}
