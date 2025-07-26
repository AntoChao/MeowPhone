#include "MPGS.h"
#include "../CommonEnum.h"

#include "../MPActor/Character/MPCharacterCat.h"
#include "../MPActor/Character/MPCharacterHuman.h"
#include "../MPActor/Item/MPItem.h"
#include "../MPActor/EnvActor/MPEnvActorComp.h"
#include "Net/UnrealNetwork.h"
#include "HighLevel/MPLogManager.h"

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
	DOREPLIFETIME(AMPGS, totalMPProgression);
	DOREPLIFETIME(AMPGS, curMPProgression);
	DOREPLIFETIME(AMPGS, curMPProgressionPercentage);
	DOREPLIFETIME(AMPGS, totalCatPlayers);
	DOREPLIFETIME(AMPGS, caughtCats);
	DOREPLIFETIME(AMPGS, caughtCatsPercentage);
	
}

void AMPGS::BeginPlay()
{
	Super::BeginPlay();
}

void AMPGS::UpdateMPProgression(int modifier)
{
	// Don't update if game has ended
	if (curGameplayStatus != EGPStatus::EGameplay)
	{
		return;
	}
	
	curMPProgression += modifier;
	
	// Prevent division by zero
	if (totalMPProgression > 0.0f)
	{
		curMPProgressionPercentage = curMPProgression / totalMPProgression;
		
		// Don't cap progression percentage - allow it to exceed 100% for win condition checking
		// The win condition will check against catWinProgressionPercentage, not 100%
	}
	else
	{
		curMPProgressionPercentage = 0.0f;
	}
}

void AMPGS::UpdateHumanProgression(int modifier)
{
	// Don't update if game has ended
	if (curGameplayStatus != EGPStatus::EGameplay)
	{
		return;
	}
	
	caughtCats += modifier;
	
	// Prevent division by zero
	if (totalCatPlayers > 0)
	{
		caughtCatsPercentage = (float)caughtCats / (float)totalCatPlayers;
		
		// Don't cap progression percentage - allow it to exceed 100% for win condition checking
		// The win condition will check against 100% (all cats caught)
	}
	else
	{
		caughtCatsPercentage = 0.0f;
	}
}

void AMPGS::OnRep_CurMPProgression()
{
	UMPLogManager::LogInfo(FString::Printf(TEXT("Client: Progression Updated to %f"), curMPProgression), TEXT("MPGS"));
}

void AMPGS::OnRep_CurMPProgressionPercentage()
{
	UMPLogManager::LogInfo(FString::Printf(TEXT("Client: Progression Percentage Updated to %.1f%%"), curMPProgressionPercentage * 100.0f), TEXT("MPGS"));
}

void AMPGS::OnRep_CaughtCats()
{
	UMPLogManager::LogInfo(FString::Printf(TEXT("Client: Caught Cats Updated to %d"), caughtCats), TEXT("MPGS"));
}

void AMPGS::OnRep_CaughtCatsPercentage()
{
	UMPLogManager::LogInfo(FString::Printf(TEXT("Client: Caught Cats Percentage Updated to %.1f%%"), caughtCatsPercentage * 100.0f), TEXT("MPGS"));
}