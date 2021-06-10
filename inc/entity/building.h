#pragma once

#include "entity/amplifiable.h"
#include "entity/shot_data.h"

#include <cstddef>
#include <list>

class Gem;

class Building : public Amplifiable
{
public:
    Building(int ix_, int iy_);

    virtual bool canBeDemolished() { return pGem == NULL; }
    virtual bool canBeAmplified() { return true; }

    // Apply amplifier effects to sd
    ShotData transformShotDataAmplify(const ShotData& sd);
    // Apply building-specific stat changes to sd
    virtual ShotData transformShotDataBuilding(const ShotData& sd) { return sd; }

    virtual void recalculateAmplifyEffects();

    void recalculateAdjacentGCLAmplifiers();

    virtual void insertGem(Gem* pGem_);
    virtual void removeGem();
    virtual void updateGem() {}

    TILE_TYPE type;

    Gem* pGem;

    float x;
    float y;
};
