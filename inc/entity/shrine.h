#pragma once

#include "constants/shrine_type.h"

#include "entity/building.h"
#include "entity/targetable.h"

#include <vector>

class IngameMap;

class Shrine : public Building
{
protected:
    std::vector<Targetable*> cachedTargets;
    SHRINE_TYPE shrineType;
    IngameMap& map;

    virtual std::vector<Targetable*> getTargets() = 0;

public:
    Shrine(IngameMap& map_, int ix_, int iy_) : Building(ix_, iy_), map(map_) {}

    virtual bool canBeDemolished() { return false; }
    virtual bool canBeAmplified() { return false; }

    virtual bool canActivate() = 0;
    virtual void activate(Gem* pGem) = 0;
};
