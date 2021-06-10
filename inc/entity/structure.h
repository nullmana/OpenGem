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

    virtual void receiveShotDamage(ShotData& shot, double damage, Gem* pSourceGem);
    virtual void receiveShrineDamage(double damage) {}
    virtual double calculateIncomingDamage(double damage);

    virtual bool canBeTargeted() { return !isIndestructible; }
    virtual float getShotVariance() { return 0.5f * width; }
};
