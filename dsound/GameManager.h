#pragma once

#include <basetsd.h>

struct GlobalVars;

struct GameManager
{
	static bool   is9BallGame(GlobalVars* globalVars);
	static bool   isPlayerTurn();
	static int    getGameMode(GlobalVars* globalVars);
	static int    getState(GlobalVars* globalVars), playerClassification(GlobalVars* globalVars, UINT32 player);
	static bool   isAnyPopupOpen(GlobalVars* globalVars);
	static UINT32 getTable(GlobalVars* globalVars);
	static UINT32 getPocketNominationMode(GlobalVars* globalVars);
	static UINT32 getNominatedPocket(GlobalVars* globalVars);
	static UINT32 getRules(GlobalVars* globalVars);
	static void   nominatePocket(GlobalVars* globalVars, int pocketIndex);

private:
	struct Offsets
	{
		static constexpr UINT32 MCMenuStateManager = 0x2A0UL;
		static constexpr UINT32 Rules              = 0x2A8UL;
		static constexpr UINT32 StateManager       = 0x300UL;
		static constexpr UINT32 Table              = 0x2ACUL;
		static constexpr UINT32 GameMode           = 0x370UL;
	};
};

