#pragma once

#include "entity/building.h"

class Amplifier : public Building
{
private:
    int16_t cooldownTimer;

public:
    Amplifier(int ix_, int iy_);

    virtual void insertGem(Gem* pGem_);
    virtual void removeGem();
    virtual void updateGem();

    // GCL only, amplifier power is reduced for every building it is amplifying
    double getGCLAmplifyPower() const;

    void tickCooldown(int frames);
    bool isCoolingDown() const { return cooldownTimer > 0; }
    float getCooldown() const { return cooldownTimer * 0.0001f; }
};
