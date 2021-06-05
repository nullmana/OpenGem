#pragma once

#include "entity/shrine.h"

class ShrineLightning : public Shrine
{
protected:
    virtual std::vector<Targetable*> getTargets();

public:
    ShrineLightning(IngameMap& map_, int ix_, int iy_);

    virtual bool canBeDemolished() { return false; }
    virtual bool canBeAmplified() { return false; }

    virtual bool canActivate();
    virtual void activate(Gem* pGem);
};
