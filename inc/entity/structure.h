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

    virtual void receiveShotDamage()
    {
        if (!isIndestructible)
            isKilled = true;
    }
    virtual float canBeTargeted() { return !isIndestructible; }
    virtual float getShotVariance() { return 0.5f * width; }
};
