#pragma once

#include "constants/tile_type.h"

#include "entity/amplifiable.h"

#include <cstddef>
#include <list>

class Gem;

class Building : public Amplifiable
{
public:
    Building(int ix_, int iy_);

    virtual bool canBeDemolished() { return pGem == NULL; }
    virtual bool canBeAmplified() { return true; }

    TILE_TYPE type;

    Gem* pGem;

    float x;
    float y;
};
