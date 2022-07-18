#include "TableInfo.h"
#include "GameManager.h"
#include "GlobalVars.h"
#include "Memory.h"
#include <array>

#include "OneTimeAssignment.h"

static Vector2D                                   size{};
static Vector4D                                   position{};
static OneTimeAssignment<std::vector<Vector2D>>   tableShape{};
static OneTimeAssignment<std::array<Vector2D, 6>> pocketsPositions{};

void TableInfo::Position::set(vec_t x, vec_t y, vec_t z, vec_t w)
{
    position.x = x;
    position.y = y;
    position.z = z;
    position.w = w;
}

void TableInfo::setSize(vec_t x, vec_t y)
{
    size.x = x;
    size.y = y;
}

vec_t TableInfo::Position::left()
{
    return position.x;
}

vec_t TableInfo::Position::top()
{
    return position.y;
}

vec_t TableInfo::Position::right()
{
    return position.z;
}

vec_t TableInfo::Position::bottom()
{
    return position.w;
}

vec_t TableInfo::getWidth()
{
    return size.x;
}

vec_t TableInfo::getHeight()
{
    return size.y;
}

BYTE TableInfo::areTableCushionsActive()
{
    UINT32 table;

    table = GameManager::getTable(gGlobalVars);

    return gGlobalVars->memory->read<BYTE>(table + Offsets::areTableCushionsActive);
}

UINT32 TableInfo::getTableCushions()
{
    UINT32 table;

    table = GameManager::getTable(gGlobalVars);

    return gGlobalVars->memory->read<UINT32>(table + Offsets::TableCushions);
}

UINT32 TableInfo::getTablePockets()
{
    UINT32 table;

    table = GameManager::getTable(gGlobalVars);

    return gGlobalVars->memory->read<UINT32>(table + Offsets::TablePockets);
}

const std::array<Vector2D, 6>& TableInfo::getPocketsPositions()
{
    PVOID  buffer;
    UINT32 table, tableProperties, pockets, bufferSize;

    if (pocketsPositions.canAssign()) {
        table           = GameManager::getTable(gGlobalVars);
        tableProperties = gGlobalVars->memory->read<UINT32>(table + 0x284UL);
        if (tableProperties) {
            pockets = gGlobalVars->memory->read<UINT32>(tableProperties + 0x34UL);
            if (pockets) {
                bufferSize = sizeof(Vector2D) * maxPocketCount;
                buffer = malloc(bufferSize);
                if (buffer) {
                    if (gGlobalVars->memory->read(pockets, buffer, bufferSize)) {
                        memcpy(&pocketsPositions.data[0].x, buffer, bufferSize);
                        pocketsPositions.postAssignment();
                    }

                    free(buffer);
                }
            }
        }
    }

    return pocketsPositions.get();
}

const std::vector<Vector2D>& TableInfo::getTableShape()
{
    PVOID  buffer;
    UINT32 table, vecStartAddr, vecEndAddr, vecSize;

    if (tableShape.canAssign()) {
        table        = GameManager::getTable(gGlobalVars);
        vecStartAddr = gGlobalVars->memory->read<UINT32>(table + 0x2F8UL);
        vecEndAddr   = gGlobalVars->memory->read<UINT32>(table + 0x2F8UL + 4UL);
        vecSize      = vecEndAddr - vecStartAddr;
        if (vecStartAddr && vecEndAddr) {
            buffer = malloc(vecSize);
            if (buffer) {
                if (gGlobalVars->memory->read(vecStartAddr, buffer, vecSize)) {
                    tableShape.data.reserve(vecSize >> 4);
                    tableShape.data.resize(vecSize >> 4);
                    memcpy(&tableShape.data[0].x, buffer, vecSize);
                    tableShape.postAssignment();
                }

                free(buffer);
            }
        }
    }

    return tableShape.get();
}

constexpr std::array<std::pair<UINT16, UINT16>, 36> tableCushions{
    std::pair { 5, 6 },
    { 6, 6 },
    { 6, 7 },
    { 7, 7 },
    { 7, 8 },
    { 8, 0 },
    { 14, 15 },
    { 15, 15 },
    { 15, 16 },
    { 16, 16 },
    { 16, 17 },
    { 17, 0 },
    { 21, 22 },
    { 22, 22 },
    { 22, 23 },
    { 23, 23 },
    { 23, 24 },
    { 24, 0 },
    { 28, 29 },
    { 29, 29 },
    { 29, 30 },
    { 30, 30 },
    { 30, 31 },
    { 31, 0 },
    { 37, 38 },
    { 38, 38 },
    { 38, 39 },
    { 39, 39 },
    { 39, 40 },
    { 40, 0 },
    { 44, 45 },
    { 45, 45 },
    { 45, 0 },
    { 0, 0 },
    { 0, 1 },
    { 1, 0 }
};

const std::array<std::pair<UINT16, UINT16>, 36>& TableInfo::getCushions()
{
    return tableCushions;
}