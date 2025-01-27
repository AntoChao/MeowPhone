#include "MPGM.h"
#include "MPGI.h"

AMPGM:AMPGM()
{

}

AMPGM:BeginPlay()
{
    Super::BeginPlay();
    mpGameInstance = Cast<UMPGI>(GetGameInstance());
}