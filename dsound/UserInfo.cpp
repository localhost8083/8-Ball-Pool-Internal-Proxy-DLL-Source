#include "UserInfo.h"

#include "Memory.h"
#include "GlobalVars.h"
#include "CodeManager.h"



int UserInfo::getWinStreak(GlobalVars* globalVars)
{
	return globalVars->memory->read<int>(globalVars->sharedUserInfo + Offsets::winStreak);
}



