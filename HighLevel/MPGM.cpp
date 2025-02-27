#include "MPGM.h"
#include "MPGI.h"

AMPGM::AMPGM()
{

}

void AMPGM::BeginPlay()
{
    Super::BeginPlay();
    mpGameInstance = Cast<UMPGI>(GetGameInstance());
}