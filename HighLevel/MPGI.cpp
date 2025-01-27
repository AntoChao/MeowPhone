#include "MPGI.h"
#include "../CommonEnum.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "UObject/NoExportTypes.h"

UMPGI::UMPGI()
{
    return;
}

// common game instance section 
void UMPGI::Init()
{
    return;
}

void UMPGI::Shutdown()
{
    return;
}

// save file section
void UMPGI::CreateSaveFile()
{
    return;
}

void UMPGI::SaveGame()
{
    return;
}


void UMPGI::LoadGame()
{
    return;
}


// session section
void UMPGI::HostSession(FName sessionName, int numPlayers)
{
    return;
}


void UMPGI::SearchForSessions()
{
    return;
}


void UMPGI::JoinSession(int sessionIndex)
{
    return;
}


void UMPGI::EndSession()
{
    return;
}


void UMPGI::DestroySession(FName sessionName)
{
    return;
}



void UMPGI::HostSessionCompleted(FName sessionName, bool hostCompleted)
{
    return;
}


void UMPGI::SearchForSessionsCompleted(bool searchCompleted)
{
    return;
}


void UMPGI::JoinSessionCompleted(FName sessionName)
{
    return;
}


void UMPGI::EndSessionCompleted(FName sessionName, bool endCompleted)
{
    return;
}


void UMPGI::DestroySessionCompleted(FName sessionName, bool destroyCompleted)
{
    return;
}


// getter && setter
ELanguage UMPGI::getGameLanguage()
{
    return;
}

