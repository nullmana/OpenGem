#pragma once

#include "constants/status.h"

#include "interface/window.h"

struct _fbg;

class IngameManaPool
{
private:
    double mana;
    double manaPoolCap;

    double manaReplenishMultiplier;
    double manaGainMultiplier;

    int manaPoolLevel;

    double initialManaPoolCap;
    double initialManaPoolCost;
    double currentManaPoolCost;

    bool autopool;

    void expandManaPoolOnceGCL();
    void expandManaPool();

public:
    IngameManaPool();

    STATUS render(struct _fbg* pFbg, const Window& window) const;

    double getMana() const { return mana; }
    double addMana(double delta, bool applyGainMultipliers);
    bool toggleAutopool() { return (autopool = !autopool); }

    bool castExpandManaPool();
    void checkMaximumMana();
    bool hasPoolMana() const { return mana >= currentManaPoolCost; }
};
