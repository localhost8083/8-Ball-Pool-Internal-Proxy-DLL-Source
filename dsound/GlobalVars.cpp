#include "globalVars.h"
#include "Offsets.h"
#include "Memory.h"

#include "Ball.h"
#include "TableInfo.h"

static GlobalVars globalVars;
GlobalVars*       gGlobalVars =&globalVars;

bool GlobalVars::init() 
{
	this->memory                      = new Memory();
	this->gameModuleBase = this->memory->getGameModuleBase2();
	if (this->gameModuleBase == 0UL)
		return false;

	this->sharedGameManager = memory->read<UINT32>(this->gameModuleBase + gOffsets::SharedGameManager);
	if (this->sharedGameManager == 0UL)
		return false;

	this->sharedMenuManager = memory->read<UINT32>(this->gameModuleBase + gOffsets::SharedMenuManager);
	if (this->sharedMenuManager == 0UL)
		return false;

	this->sharedUserInfo = memory->read<UINT32>(this->gameModuleBase + gOffsets::SharedUserInfo);
	if (this->sharedUserInfo == 0UL)
		return false;

	this->sharedUserSettings = memory->read<UINT32>(this->gameModuleBase + gOffsets::SharedUserSettings);
	if (this->sharedUserSettings == 0UL)
		return false;

	this->features.esp.ballCircleRadius        = static_cast<float>(Ball::getRadius() * 5.0);
	this->features.esp.ballCircleFilled        = true;
	this->features.esp.ballLineThickness       = 1.f;

	this->features.esp.shotStateCircleThickness = 5.f;
	this->features.esp.shotStateCircleRadius    = static_cast<float>(TableInfo::getPocketRadius() * 5.0);

	this->features.maxWinStreak                = 5;
	this->features.powerStepSize               = 0.1f;
	this->features.angleRotationStepSize       = static_cast<float>(gTimePerTick);
	this->features.delayBetweenAngleSteps      = 5;
	this->features.autoPlayMaxPower            = 100.f;
	this->features.autoPlayDelay               = 1.f;
	this->features.autoPlayDelayBounds[1]      = 5.f;
	this->features.esp.displayPercantage       = 100;

	return true;
}