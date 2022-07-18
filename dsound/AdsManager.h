#pragma once

#include <basetsd.h>

#ifdef DumpCode
#include "const.h"
#endif

struct GlobalVars;

struct AdsManager
{

	static void disableAdBreakScreen(GlobalVars* globalVars);

private:
	struct Offsets
	{
		static constexpr SIZE_T SharedAdsManager        = 0x35DB9D4UL;
		static constexpr SIZE_T InterstitialsController = 0x84UL;
	};
};