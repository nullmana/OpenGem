#pragma once

enum SHRINE_TYPE
{
    // GCL
    SHRINE_CHARGED_BOLTS = 0,
    SHRINE_LIGHTNING = 1,

    // GCCS
    SHRINE_ENERGY = 2,
    SHRINE_BLADES = 3,
    SHRINE_WISDOM = 4,
    SHRINE_FOCUS = 5,
    SHRINE_VENOM = 6,
    SHRINE_INFECTION = 7,
    SHRINE_ENCHANTMENT = 8,
    SHRINE_TIME = 9,
    SHRINE_CORROSION = 10,

    SHRINE_TYPE_COUNT
};

#ifdef DEBUG
static const char* SHRINE_TYPE_NAME[SHRINE_TYPE_COUNT] = {
    "ChargedBolts",
    "Lightning",

    "Energy",
    "Blades",
    "Wisdom",
    "Focus",
    "Venom",
    "Infection",
    "Enchantment",
    "Time",
    "Corrosion",
};
#endif
