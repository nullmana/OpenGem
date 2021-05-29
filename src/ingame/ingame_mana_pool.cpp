#include "ingame/ingame_mana_pool.h"

#include "constants/game_header.h"

#include "wrapfbg.h"

#include <algorithm>
#include <cmath>

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

void IngameManaPool::addMana(double delta, bool applyGainMultipliers)
{
    if (applyGainMultipliers)
    {
        delta *= manaGainMultiplier;
    }

    mana += delta;
}

void IngameManaPool::expandManaPool()
{
    if (g_game.game == GC_LABYRINTH)
    {
        manaReplenishMultiplier += 0.05;
        manaGainMultiplier += 0.05;
        mana -= currentManaPoolCost;
        manaPoolCap += floor(0.5 * initialManaPoolCap);
        currentManaPoolCost = std::min<double>(0.95 * manaPoolCap,
            10 * round(initialManaPoolCost * 0.02 + currentManaPoolCost * 0.005) +
                currentManaPoolCost);
    }
    else if (g_game.game == GC_CHASINGSHADOWS)
    {
        manaReplenishMultiplier += 0.272;
        manaGainMultiplier += 0.04;
        manaPoolCap *= 1.41 + 0.035 * (++manaPoolLevel);
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
        while (mana > manaPoolCap)
        {
            expandManaPool();
        }
    }
    else
    {
        if (mana > manaPoolCap)
            mana = manaPoolCap;
    }
}
