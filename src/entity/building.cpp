#include "entity/building.h"
#include "entity/amplifier.h"
#include "entity/gem.h"

#include "constants/game_header.h"

#include <unordered_set>

Building::Building(int ix_, int iy_) : Amplifiable(ix_, iy_)
{
    x = ix_ + 0.5f * g_game.ingameBuildingSize;
    y = iy_ + 0.5f * g_game.ingameBuildingSize;

    pGem = NULL;
}

ShotData Building::transformShotDataAmplify(const ShotData& sd)
{
    // Shouldn't ever be possible, but sanity check anyway
    if (type == TILE_AMPLIFIER)
        return sd;

    if (amplifying.empty())
        return sd;

    ShotData amplifyEffectsSD;

    for (const Amplifiable* a : amplifying)
    {
        // For any building that isn't an Amplifier, a is an Amplifier
        const Amplifier* pAmplifier = reinterpret_cast<const Amplifier*>(a);

        if ((pAmplifier->pGem == NULL) || pAmplifier->pGem->isDragged)
            continue;

        double damageMultiplier;
        float rangeMultiplier;
        float speedMultiplier;
        double specialMultiplier;

        if (g_game.game == GC_LABYRINTH)
        {
            damageMultiplier = pAmplifier->getGCLAmplifyPower() * 2.0;
            rangeMultiplier = damageMultiplier * 0.16f;
            speedMultiplier = damageMultiplier * 0.27f;
            specialMultiplier = damageMultiplier * 0.2;
            if (type == TILE_TRAP)
            {
                damageMultiplier *= 0.3;
                specialMultiplier *= 1.8;
            }
        }
        else if (g_game.game == GC_CHASINGSHADOWS)
        {
            damageMultiplier = 0.2;
            rangeMultiplier = 0.1f;
            speedMultiplier = 0.1f;
            specialMultiplier = 0.15;
        }
        else
        {
            throw "Game Code Unavailable!";
        }

        ShotData ampSD = pAmplifier->pGem->getShotDataComponents();
        amplifyEffectsSD +=
            ampSD.multiply(damageMultiplier, rangeMultiplier, speedMultiplier, specialMultiplier);
    }

    return sd + amplifyEffectsSD.round();
}

void Building::recalculateAdjacentGCLAmplifiers()
{
    // Only GCL has amplifier power depend on how many targets are affected,
    // so we need to check all our amplifiers for any buildings they are affecting, to recalculate
    if (g_game.game == GC_LABYRINTH)
    {
        std::unordered_set<Amplifiable*> toRecalculate;

        for (Amplifiable* a : amplifying)
        {
            // For any building that isn't an Amplifier, a is an Amplifier
            Amplifier* pAmplifier = reinterpret_cast<Amplifier*>(a);

            if (pAmplifier->pGem == NULL)
                continue;

            for (Amplifiable* b : pAmplifier->amplifying)
            {
                if (b != this)
                    toRecalculate.insert(b);
            }
        }

        for (Amplifiable* a : toRecalculate)
            a->recalculateAmplifyEffects();
    }
}

void Building::recalculateAmplifyEffects()
{
    if (pGem != NULL)
        pGem->recalculateShotData();
}

void Building::insertGem(Gem* pGem_)
{
    pGem = pGem_;
    pGem_->pBuilding = this;
}

void Building::removeGem()
{
    pGem->pBuilding = NULL;
    pGem = NULL;
}
