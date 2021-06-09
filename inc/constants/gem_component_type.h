#pragma once

enum GEM_COMPONENT_TYPE
{
    GEM_SLOW = 0,
    GEM_CHAIN = 1,
    GEM_POISON = 2,
    GEM_ARMOR = 3,
    GEM_SHOCK = 4,
    GEM_BLOODBOUND = 5,
    GEM_CRITICAL = 6,
    GEM_LEECH = 7,

    GEM_POOLBOUND = 8,
    GEM_SUPPRESSING = 9,

    GEM_COMPONENT_TYPE_COUNT
};

#ifdef DEBUG
static const char* GEM_COMPONENT_TYPE_NAME[GEM_COMPONENT_TYPE_COUNT] = {
    "Slow",
    "Chain Hit",
    "Poison",
    "Armor Tearing",
    "Shock",
    "Bloodbound",
    "Critical Hit",
    "Leeching",

    "Poolbound",
    "Suppressing",
};
#endif
