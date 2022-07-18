#include "Ball.h"
#include "Memory.h"
#include "Offsets.h"
#include "GlobalVars.h"
#include "CodeManager.h"


Vector2D Ball::getPosition(GlobalVars* globalVars, UINT32 object)
{
    return globalVars->memory->read<Vector2D>(object + Offsets::Position);
}



int Ball::classification(GlobalVars* globalVars, UINT32 object)
{
    return globalVars->memory->read<int>(object + Offsets::classification);
}



int Ball::getState(GlobalVars* globalVars, UINT32 object)
{
    return globalVars->memory->read<int>(object + Offsets::State);
}


bool Ball::isOnTable(GlobalVars* globalVars, UINT32 object)
{
    int state = getState(globalVars, object);

    return ((state == 1) || (state == 2));
}