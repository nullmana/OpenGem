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

    double armor;

    bool isIndestructible;

    virtual uint32_t receiveShotDamage(ShotData& shot, uint32_t numShots, double damage, double crit,
        Gem* pSourceGem, bool isKillingShot);
    virtual void receiveShrineDamage(double damage) {}
    virtual double calculateIncomingDamage(double damage, double crit);

    virtual bool canBeTargeted() { return !isIndestructible; }
    virtual float getShotVariance() { return 0.125f * width; }
    virtual bool canLeech() { return false; }
};
