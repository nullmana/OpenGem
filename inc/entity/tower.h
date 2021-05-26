#pragma once

#include "entity/building.h"

class IngameMap;

class Tower : public Building
{
public:
    Tower(int ix_, int iy_) : Building(ix_, iy_) { type = BUILDING_TOWER; }

    void tick(IngameMap& map, int frames);
};
