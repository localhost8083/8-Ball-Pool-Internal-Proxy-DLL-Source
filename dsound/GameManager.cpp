#include "GameManager.h"

#include "OneTimeAssignment.h"
#include "GlobalVars.h"
#include "Memory.h"
#include "Offsets.h"

UINT32 GameManager::getRules(GlobalVars* globalVars)
{
	return globalVars->memory->read<UINT32>(globalVars->sharedGameManager + Offsets::Rules);
}

bool GameManager::is9BallGame(GlobalVars* globalVars)
{
	int    result;
	UINT32 rules = GameManager::getRules(globalVars);

	result = globalVars->memory->read<int>(rules);

	return (result & 64) == 64;
}

bool GameManager::isPlayerTurn()
{
	return (GameManager::getState(gGlobalVars) == 4);
}

int GameManager::getGameMode(GlobalVars* globalVars)
{
	return globalVars->memory->read<int>(globalVars->sharedGameManager + Offsets::GameMode);
}

int GameManager::getState(GlobalVars* globalVars)
{
	UINT32 stateManager, objectEntry, lastObject, objectCount, buffer, stateAddress;

	stateManager = globalVars->memory->read<UINT32>(globalVars->sharedGameManager + Offsets::StateManager);
	buffer = globalVars->memory->read<UINT32>(stateManager + 0x4UL);
	objectCount = globalVars->memory->read<UINT32>(buffer + 0x4UL);
	objectEntry = globalVars->memory->read<UINT32>(buffer + 0xCUL);
	lastObject = globalVars->memory->read<UINT32>(objectEntry + objectCount * 0x4UL - 0x4UL);
	stateAddress = lastObject;
	if (stateAddress) {
		stateAddress += 0xCUL;
		return globalVars->memory->read<int>(stateAddress);
	}

	return 0;
}

bool GameManager::isAnyPopupOpen(GlobalVars* globalVars)
{
	UINT32 mcMenuStateManager, objectCount, menuStateStack;

	mcMenuStateManager = globalVars->memory->read<UINT32>(globalVars->sharedGameManager + Offsets::MCMenuStateManager);
	menuStateStack     = globalVars->memory->read<UINT32>(mcMenuStateManager + 0x8UL);
	objectCount        = globalVars->memory->read<UINT32>(menuStateStack + 0x4UL);

	return (objectCount > 1);
}

int GameManager::playerClassification(GlobalVars* globalVars, UINT32 player)
{
	UINT32 rules, classificationsAddress;

	rules                  = getRules(globalVars);
	classificationsAddress = globalVars->memory->read<UINT32>(rules + 0x5CUL);

	return globalVars->memory->read<int>(classificationsAddress + player * 4UL); // 0 * 4 for local player 1 * 4 for oppenent 
}

UINT32 GameManager::getTable(GlobalVars* globalVars)
{
	return globalVars->memory->read<UINT32>(globalVars->sharedGameManager + Offsets::Table);
}

UINT32 GameManager::getPocketNominationMode(GlobalVars* globalVars)
{
	UINT32 rules = GameManager::getRules(globalVars);

	UINT32 unknown = globalVars->memory->read<UINT32>(rules + 0x38);

	return (unknown - 1UL);
}

UINT32 GameManager::getNominatedPocket(GlobalVars* globalVars)
{
	UINT32 rules = GameManager::getRules(globalVars);

	return globalVars->memory->read<UINT32>(rules + 0x94);
}

void GameManager::nominatePocket(GlobalVars* globalVars, int pocketIndex)
{
	UINT32 rules = GameManager::getRules(globalVars);

	globalVars->memory->write(rules + 0x94, pocketIndex);
}