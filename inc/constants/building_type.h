#pragma once

#include <cstdint>

enum BUILDING_TYPE
{
    BUILDING_NONE = 0,
    BUILDING_PATH = 1,

    BUILDING_WALL       = 2,
    BUILDING_WALL_PATH  = 3,
    BUILDING_TOWER      = 4,
    BUILDING_TOWER_PATH = 5,
    BUILDING_TRAP       = 6,

    BUILDING_ORB = 7,

    BUILDING_TYPE_COUNT
};

inline static bool isPathable(BUILDING_TYPE t)
{
    return (t == BUILDING_PATH) || (t == BUILDING_TRAP);
}

const static uint32_t TILE_COLOR[BUILDING_TYPE_COUNT] = {
    0x222222, // BUILDING_NONE
    0xFFFFCC, // BUILDING_PATH

    0x9999AA, // BUILDING_WALL
    0x8888AA, // BUILDING_WALL_PATH
    0x3333EE, // BUILDING_TOWER
    0x2222EE, // BUILDING_TOWER_PATH
    0xDD4433, // BUILDING_TRAP

    0x1133FF, // BUILDING_ORB
};

#ifdef DEBUG
static const char* BUILDING_TYPE_NAME[BUILDING_TYPE_COUNT] = {
    "None",
    "Path",

    "Wall",
    "WallPath",
    "Tower",
    "TowerPath",
    "Trap",

    "Orb",
};
#endif
