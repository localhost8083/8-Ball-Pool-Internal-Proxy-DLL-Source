#pragma once

#include "Vector.h"
#include <basetsd.h>

#ifdef DumpCode
#include "const.h"
#endif

struct GlobalVars;

struct Ball
{

	static Vector2D getPosition(GlobalVars* globalVars, UINT32 object);
	static int      getState(GlobalVars* globalVars, UINT32 object);
	static int      classification(GlobalVars* globalVars, UINT32 object);

	static bool            isOnTable(GlobalVars* globalVars, UINT32 object);
	static constexpr vec_t getRadius() { return 3.800475; }

private:
	struct Offsets
	{
		static constexpr UINT32 Spin           = 0x10UL;
		static constexpr UINT32 Position       = 0x28UL;
		static constexpr UINT32 Velocity       = 0x38UL;
		static constexpr UINT32 Radius         = 0x50UL;
		static constexpr UINT32 classification = 0x78UL;
		static constexpr UINT32 State          = 0x7CUL; // 1 or 2 = On Table
	};
};