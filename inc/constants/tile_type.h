#pragma once

#include <cstdint>

enum TILE_TYPE
{
    TILE_NONE = 0,
    TILE_PATH = 1,

    TILE_WALL = 2,
    TILE_WALL_PATH = 3,
    TILE_TOWER = 4,
    TILE_TOWER_PATH = 5,
    TILE_TRAP = 6,
    TILE_AMPLIFIER = 7,
    TILE_AMPLIFIER_PATH = 8,

    TILE_ORB = 9,

    TILE_TYPE_COUNT
};

inline static bool isPathable(TILE_TYPE t)
{
    return (t == TILE_PATH) || (t == TILE_TRAP);
}

const static uint32_t TILE_COLOR[TILE_TYPE_COUNT] = {
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
static const char* TILE_TYPE_NAME[TILE_TYPE_COUNT] = {
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
