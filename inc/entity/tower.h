#pragma once

#include "entity/building.h"

class IngameMap;

class Tower : public Building
{
public:
    Tower(int ix_, int iy_) : Building(ix_, iy_) { type = TILE_TOWER; }

    void tick(IngameMap& map, int frames);

    virtual void insertGem(Gem* pGem_);
    virtual void removeGem();
};
