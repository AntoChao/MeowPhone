#include "MPAIControllerHuman.h"
#include "MPAISystemManager.h"

AMPAIControllerHuman::AMPAIControllerHuman()
{
    
}

void AMPAIControllerHuman::BeginPlay()
{
    Super::BeginPlay();

}

void AMPAIControllerHuman::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

// setter and getter
void AMPAIControllerHuman::SetAISystem(AMPAISystemManager* aAIManager)
{
    theAIManager = aAIManager;
}