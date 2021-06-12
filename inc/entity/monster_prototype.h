#pragma once

#include <cstdint>

struct MonsterPrototype
{
public:
    double hp;
    double armor;
    double mana;
    double banishmentCostMultiplier;
    uint8_t type;
};
