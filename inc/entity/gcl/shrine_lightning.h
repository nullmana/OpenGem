#pragma once

#include "entity/shrine.h"

class ShrineLightning : public Shrine
{
protected:
    virtual std::vector<Targetable*> getTargets();

public:
    ShrineLightning(IngameMap& map_, int ix_, int iy_);

    virtual bool canActivate();
    virtual void activate(Gem* pGem);
};
