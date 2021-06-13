#pragma once

#include "entity/building.h"

#include <vector>

class IngameMap;
class Targetable;

class Tower : public Building
{
private:
    float shotCharge;

    bool cachedTargetsValid;
    std::vector<Targetable*> cachedTargets;

    Targetable* pickTarget(IngameMap& map);

public:
    Tower(int ix_, int iy_);

    void tick(IngameMap& map, int frames);

    virtual void insertGem(Gem* pGem_);
    virtual void removeGem();
};
