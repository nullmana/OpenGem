#pragma once

#include "entity/building.h"

#include <vector>

class IngameMap;
class Targetable;

class Tower : public Building
{
private:
    float shotCharge;
    int16_t cooldownTimer;

    bool cachedTargetsValid;
    std::vector<Targetable*> cachedTargets;

    Targetable* pickTarget(IngameMap& map);

public:
    Tower(int ix_, int iy_);

    void tick(IngameMap& map, int frames);

    virtual void insertGem(Gem* pGem_);
    virtual void removeGem();
    virtual void updateGem();

    void tickCooldown(int frames);
    bool isCoolingDown() const { return cooldownTimer > 0; }
    float getCooldown() const { return cooldownTimer * 0.0001f; }
};
