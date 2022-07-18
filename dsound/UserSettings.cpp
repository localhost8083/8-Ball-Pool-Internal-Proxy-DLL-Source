#include "UserSettings.h"
#include "GlobalVars.h"
#include "Offsets.h"
#include "Memory.h"

#include "CodeManager.h"


void UserSettings::setWideGuideLine(GlobalVars* globalVars, unsigned char enable)
{
	globalVars->memory->write(globalVars->sharedUserSettings + 0xE, enable);
}


void UserSettings::disablepopup(GlobalVars* globalVars, unsigned char enable)
{

	globalVars->memory->write(0x40600000 + 0x28FF74B, 0x00);
	globalVars->memory->write(0x40600000 + 0xAABCD0, 0x00);
	globalVars->memory->write(0x40600000 + 0xAB2F04, 0x00);
	globalVars->memory->write(0x40600000 + 0x13F1600, 0x00);
}
