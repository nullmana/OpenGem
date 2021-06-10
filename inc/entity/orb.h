#pragma once

#include "entity/amplifiable.h"
#include "entity/monster_node.h"

class Orb : public MonsterNode, public Amplifiable
{
    bool broken;
    double banishmentCostReductionAmplifiersRaw;

public:
    Orb(int ix, int iy);

    bool isBroken() const { return broken; }
    void breakOrb() { broken = true; }

    virtual void recalculateAmplifyEffects();
};
