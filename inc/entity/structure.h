#pragma once

#include "entity/targetable.h"

class Structure : public Targetable
{
public:
    Structure(int ix_, int iy_, int width_, int height_);

    int ix;
    int iy;
    int width;
    int height;

    bool isIndestructible;

    virtual uint32_t receiveShotDamage(ShotData& shot, uint32_t numShots, double damage, double crit,
        Gem* pSourceGem, bool isKillingShot);
    virtual void receiveShrineDamage(double damage) {}
    virtual void receiveBombDamage(const ShotData& shot, double damage);
    virtual double calculateIncomingDamage(double damage, double crit);

    virtual bool canBeTargeted() const { return !isIndestructible; }
    virtual bool canBeSecondaryTarget() const { return true; }
    virtual float getShotVariance() const { return 0.125f * width; }
    virtual bool canLeech() const { return false; }
};
