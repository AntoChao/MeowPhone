#include "MPHUD.h"
#include "../../HighLevel/MPGI.h"

UMPHUD::UMPHUD()
{

}

void UMPHUD::NativeConstruct()
{
    Super::NativeConstruct();

    curGameInstance = Cast<UMPGI>(GetWorld()->GetGameInstance());
}

void UMPHUD::SetOwner(AMPControllerPlayer* theOwner)
{
    owner = theOwner;
}