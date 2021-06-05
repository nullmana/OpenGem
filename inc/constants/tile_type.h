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

    TILE_SHRINE_CB = 9,
    TILE_SHRINE_LI = 10,

    TILE_ORB = 11,

    TILE_TYPE_COUNT
};

inline static bool isPathable(TILE_TYPE t)
{
    return (t == TILE_PATH) || (t == TILE_TRAP);
}

const static uint32_t TILE_COLOR[TILE_TYPE_COUNT] = {
    0x222222, // TILE_NONE
    0xFFFFCC, // TILE_PATH

    0x9999AA, // TILE_WALL
    0x8888AA, // TILE_WALL_PATH
    0x4444BB, // TILE_TOWER
    0x2222BB, // TILE_TOWER_PATH
    0x884433, // TILE_TRAP
    0xBB4444, // TILE_AMPLIFIER
    0xBB2222, // TILE_AMPLIFIER_PATH

    0x226848, // TILE_SHRINE_CB
    0x224868, // TILE_SHRINE_LI

    0x4477FF, // TILE_ORB
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

    "ShrineCB",
    "ShrineLI",

    "Orb",
};
#endif
