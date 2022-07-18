#include "AdsManager.h"

#include "Memory.h"
#include "GlobalVars.h"

#include "OneTimeAssignment.h"
#include "CodeManager.h"


void AdsManager::disableAdBreakScreen(GlobalVars* globalVars)
{
	UINT32 adManager, interstitialsController;
	UCHAR  playedMatchesWithoutAd, playedMatchesWithoutAdAddr;

	adManager = globalVars->memory->read<UINT32>(globalVars->gameModuleBase + Offsets::SharedAdsManager);
	if (adManager == 0UL)
		return;

	interstitialsController = globalVars->memory->read<UINT32>(adManager + Offsets::InterstitialsController);
	if (interstitialsController == 0UL)
		return;

	playedMatchesWithoutAdAddr = globalVars->memory->read<UINT32>(interstitialsController + 0xCUL);
	if (playedMatchesWithoutAdAddr) {
		playedMatchesWithoutAdAddr += 0x20UL;

		playedMatchesWithoutAd = globalVars->memory->read<UCHAR>(playedMatchesWithoutAdAddr);
		if (playedMatchesWithoutAd > 0)
			globalVars->memory->write(playedMatchesWithoutAdAddr, 0Ui8);
	}
}

