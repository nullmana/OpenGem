#include "ingame/ingame_mana_pool.h"

#include "constants/game_header.h"

#include "wrapfbg.h"

#include <algorithm>
#include <cmath>

#include <cstdio>

IngameManaPool::IngameManaPool()
{
    mana = 1000.0;
    manaPoolCap = 2000.0;

    manaReplenishMultiplier = 1.0;
    manaGainMultiplier = 1.0;

    manaPoolLevel = 1;

    initialManaPoolCap = manaPoolCap;
    initialManaPoolCost = currentManaPoolCost = 240.0;

    // Only GCL has pool as a manual spell, default off
    autopool = (g_game.game != GC_LABYRINTH);
}

STATUS IngameManaPool::render(struct _fbg* pFbg, const Window& window) const
{
    double fill = std::max(0.0, mana / manaPoolCap);
    bool isFull = mana == manaPoolCap;

    if (isFull)
    {
        fbg_rect(pFbg, window.x, window.y, window.width, window.height, 0x80, 0x80, 0xD0);
    }
    else
    {
        fbg_rect(pFbg, window.x, window.y, fill * window.width, window.height, 0x20, 0x20, 0xA0);
    }

    return STATUS_OK;
}

double IngameManaPool::addMana(double delta, bool applyGainMultipliers)
{
    if (applyGainMultipliers)
    {
        delta *= manaGainMultiplier;
    }

    mana += delta;

    return delta;
}

void IngameManaPool::expandManaPoolOnceGCL()
{
    manaReplenishMultiplier += 0.05;
    manaGainMultiplier += 0.05;
    manaPoolCap += floor(0.5 * initialManaPoolCap);
    mana -= currentManaPoolCost;
    currentManaPoolCost = std::min<double>(0.95 * manaPoolCap,
        10 * round(initialManaPoolCost * 0.02 + currentManaPoolCost * 0.005) + currentManaPoolCost);
}

void IngameManaPool::expandManaPool()
{
    if (g_game.game == GC_LABYRINTH)
    {
        const double poolIncrement = floor(0.5 * initialManaPoolCap);

        while ((currentManaPoolCost < 0.95 * manaPoolCap) && (mana > manaPoolCap))
        {
            manaReplenishMultiplier += 0.05;
            manaGainMultiplier += 0.05;
            manaPoolCap += poolIncrement;
            mana -= currentManaPoolCost;
            currentManaPoolCost = std::min<double>(0.95 * manaPoolCap,
                10 * round(initialManaPoolCost * 0.02 + currentManaPoolCost * 0.005) + currentManaPoolCost);
        }

        if (mana > manaPoolCap)
        {
            // Solved quadratic to get number of expansions necessary with 0.95 pool cost
            double numExpansions = floor((sqrt(pow(0.95 * manaPoolCap + 0.525 * poolIncrement, 2.0) +
                                               1.9 * poolIncrement * (mana - manaPoolCap)) -
                                             (0.95 * manaPoolCap + 0.525 * poolIncrement)) /
                                         (0.95 * poolIncrement));
            double manaSpent = 0.475 * poolIncrement * numExpansions * numExpansions +
                               (0.95 * manaPoolCap - 0.475 * poolIncrement) * numExpansions;

            manaReplenishMultiplier += 0.05 * numExpansions;
            manaGainMultiplier += 0.05 * numExpansions;
            manaPoolCap += poolIncrement * numExpansions;
            mana -= manaSpent;
            currentManaPoolCost = 0.95 * manaPoolCap;

            // Should only ever need to expand once afterwards to correct for the rounding down, but loop just in case
            while (mana > manaPoolCap)
                expandManaPoolOnceGCL();
        }
    }
    else if (g_game.game == GC_CHASINGSHADOWS)
    {
        while (mana > manaPoolCap)
        {
            manaReplenishMultiplier += 0.272;
            manaGainMultiplier += 0.04;
            manaPoolCap *= 1.41 + 0.035 * (++manaPoolLevel);
        }
    }
    else
    {
        throw "Game Code Unavailable!";
    }
}

bool IngameManaPool::castExpandManaPool()
{
    if (mana >= currentManaPoolCost)
    {
        if (g_game.game == GC_LABYRINTH)
            expandManaPoolOnceGCL();
        else
            expandManaPool();
        return true;
    }

    return false;
}

void IngameManaPool::checkMaximumMana()
{
    if (autopool)
    {
        bool expanded = mana > manaPoolCap;
        if (mana > manaPoolCap)
            expandManaPool();
    }
    else
    {
        if (mana > manaPoolCap)
            mana = manaPoolCap;
    }
}
