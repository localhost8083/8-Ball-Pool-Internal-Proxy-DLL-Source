#pragma once

struct GlobalVars;
#include "const.h"

struct UserSettings
{

	static void setWideGuideLine(GlobalVars* globalVars, unsigned char enable);

	static void disablepopup(GlobalVars* globalVars, unsigned char enable);

};

