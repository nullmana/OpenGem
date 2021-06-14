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

static const uint32_t WAVE_TYPE_COLOR[WAVE_TYPE_COUNT] = {
    0x888888, // WAVE_REAVER
    0x88FF88, // WAVE_SWARMLING
    0x8888FF, // WAVE_GIANT
    0x8888BB, // WAVE_ARMORED
    0x88BB88, // WAVE_RUNNER

    0xFF8888, // WAVE_SPARK
};
