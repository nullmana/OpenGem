#pragma once

#include "entity/building.h"

class Amplifier : public Building
{
public:
    Amplifier(int ix_, int iy_)
        : Building(ix_, iy_) { type = TILE_AMPLIFIER; }

    virtual void insertGem(Gem* pGem_);
    virtual void removeGem();
    virtual void updateGem();

    // GCL only, amplifier power is reduced for every building it is amplifying
    double getGCLAmplifyPower() const;
};
