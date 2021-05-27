#pragma once

#include <cstdint>

enum BUILDING_TYPE
{
    BUILDING_NONE = 0,
    BUILDING_PATH = 1,

    BUILDING_WALL = 2,
    BUILDING_WALL_PATH = 3,
    BUILDING_TOWER = 4,
    BUILDING_TOWER_PATH = 5,
    BUILDING_TRAP = 6,
    BUILDING_AMPLIFIER = 7,
    BUILDING_AMPLIFIER_PATH = 8,

    BUILDING_ORB = 9,

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
    0x4444BB, // BUILDING_TOWER
    0x2222BB, // BUILDING_TOWER_PATH
    0x884433, // BUILDING_TRAP
    0xBB4444, // BUILDING_AMPLIFIER
    0xBB2222, // BUILDING_AMPLIFIER_PATH

    0x4477FF, // BUILDING_ORB
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
    "Amplifier",
    "AmplifierPath",

    "Orb",
};
#endif
