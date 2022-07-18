#pragma once

#include <basetsd.h>

#ifdef DumpCode
#include "const.h"
#endif

struct GlobalVars;

struct UserInfo
{

	static int getWinStreak(GlobalVars* globalVars);

private:
	struct Offsets
	{
		static constexpr UINT32 winStreak = 0x158UL;
	};
};