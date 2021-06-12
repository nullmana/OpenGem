#pragma once

#include "entity/building.h"

class IngameMap;

class Trap : public Building
{
public:
    Trap(int ix_, int iy_)
        : Building(ix_, iy_) { type = TILE_TRAP; }

    void tick(IngameMap& map, int frames);

    virtual ShotData transformShotDataBuilding(const ShotData& sd);

    virtual void insertGem(Gem* pGem_);
    virtual void removeGem();
};
