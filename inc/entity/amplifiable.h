#pragma once

#include "constants/tile_type.h"

#include <list>

class Amplifiable
{
public:
    Amplifiable(int ix_, int iy_) : ix(ix_), iy(iy_) {}

    virtual void recalculateAmplifyEffects() {}

    std::list<Amplifiable*> amplifying;

    int ix;
    int iy;
};
