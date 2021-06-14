#pragma once

#include "entity/building.h"

class IngameMap;

class Trap : public Building
{
private:
    float shotCharge;
    int16_t cooldownTimer;

public:
    Trap(int ix_, int iy_);

    void tick(IngameMap& map, int frames);

    virtual ShotData transformShotDataBuilding(const ShotData& sd);

    virtual void insertGem(Gem* pGem_);
    virtual void removeGem();
    virtual void updateGem();

    void tickCooldown(int frames);
    bool isCoolingDown() const { return cooldownTimer > 0; }
    float getCooldown() const { return cooldownTimer * 0.0001f; }
};
