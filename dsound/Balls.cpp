#include "Balls.h"
#include "Memory.h"
#include "Offsets.h"
#include "globalVars.h"
#include "GameManager.h"
#include "CodeManager.h"


bool Balls::initBallList(GlobalVars* globalVars, UINT32& ballList, int& ballCount)
{
    UINT32 table, balls;

    table     = GameManager::getTable(globalVars);
    balls     = globalVars->memory->read<UINT32>(table + Offsets::Balls);
    ballList  = globalVars->memory->read<UINT32>(balls + Offsets::Entry);
    ballCount = globalVars->memory->read<int>(balls + Offsets::Count);

    return true;
}



UINT32 Balls::getBall(GlobalVars* globalVars, UINT32 index, const UINT32& ballList)
{
    return globalVars->memory->read<UINT32>(ballList + index * 4UL);
}