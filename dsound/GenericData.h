#pragma once

#include "const.h"
#include <basetsd.h>

#ifdef DumpCode
#include "const.h"
#endif

struct GlobalVars;

struct VisualGuide
{

	static vec_t getAimAngle(GlobalVars* globalVars, UINT32 visualGuide);
	static float getPlayerTimer(GlobalVars* globalVars, UINT32 visualGuide);
	static void  setAimAngle(GlobalVars* globalVars, UINT32 visualGuide, const vec_t& angle);
	static bool  initVisualGuide(GlobalVars* globalVars, UINT32* out);



private:
	struct Offsets
	{
		static constexpr UINT32 AimAngle    = 0x18UL;
		static constexpr UINT32 Timer       = 0x28UL;
		static constexpr UINT32 VisualGuide = 0x27CUL;
		static constexpr UINT32 VisualCue   = 0x2D8UL;
	};
};

