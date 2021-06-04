#pragma once

#include "entity/building.h"

class Amplifier : public Building
{
public:
    Amplifier(int ix_, int iy_) : Building(ix_, iy_) { type = TILE_AMPLIFIER; }
};
