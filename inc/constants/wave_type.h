#pragma once

#include <cstdint>

enum WAVE_TYPE
{
    WAVE_REAVER = 0,
    WAVE_SWARMLING = 1,
    WAVE_GIANT = 2,
    WAVE_ARMORED = 3,
    WAVE_RUNNER = 4,

    WAVE_SPARK = 5,

    WAVE_TYPE_COUNT
};

enum WAVE_FORMATION
{
    FORMATION_NORMAL = 0,
    FORMATION_2 = 1,
    FORMATION_3 = 2,
    FORMATION_6 = 3,
    FORMATION_TIGHT_LOOSE = 4,
    FORMATION_LOOSE_TIGHT = 5,
    FORMATION_RANDOM_LOOSE = 6,
    FORMATION_RANDOM_TIGHT = 7,
    FORMATION_RANDOM = 8,
    FORMATION_TIGHT = 9,
    FORMATION_LOOSE = 10,

    WAVE_FORMATION_COUNT
};

static const uint32_t WAVE_TYPE_COLOR[WAVE_TYPE_COUNT] = {
    0x888888, // WAVE_REAVER
    0x88EE88, // WAVE_SWARMLING
    0x8888EE, // WAVE_GIANT
    0x8888AA, // WAVE_ARMORED
    0x88AA88, // WAVE_RUNNER

    0xEE8888, // WAVE_SPARK
};
