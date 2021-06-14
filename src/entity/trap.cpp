#include "entity/trap.h"
#include "entity/gem.h"

#include "ingame/ingame_mana_pool.h"
#include "ingame/ingame_map.h"

#include <algorithm>
#include <cmath>

Trap::Trap(int ix_, int iy_)
    : Building(ix_, iy_)
{
    type = TILE_TRAP;
    shotCharge = 0.0f;
    cooldownTimer = 0;
}

void Trap::tick(IngameMap& map, int frames)
{
    if ((pGem != NULL) && !pGem->isDragged && (frames > 0))
    {
        shotCharge += frames * pGem->shotFinal.fireRate;
        if (shotCharge < 30.0f)
            return;

        std::vector<Targetable*> targetsInRange;
        float rangeSq = 1.0f;
        if (g_game.game == GC_LABYRINTH)
            rangeSq = 1.0f;
        else if (g_game.game == GC_CHASINGSHADOWS)
            rangeSq = (30.0f / 17.0f) * (30.0f / 17.0f);
        else
            throw "Game Code Unavailable!";

        for (int j = std::max(0, iy - 1);
             j <= std::min(g_game.ingameMapHeight - 1, iy + g_game.ingameBuildingSize); ++j)
        {
            for (int i = std::max(0, ix - 1);
                 i <= std::min(g_game.ingameMapWidth - 1, ix + g_game.ingameBuildingSize); ++i)
            {
                const std::vector<Monster*>& tile = map.enemyController.getMonstersOnTile(j, i);
                for (Monster* m : tile)
                {
                    if (!m->isKilled && ((x - m->x) * (x - m->x) + (y - m->y) * (y - m->y) < rangeSq))
                        targetsInRange.push_back(m);
                }
            }
        }

        if (targetsInRange.empty())
        {
            shotCharge = 30.0f;
            return;
        }

        uint64_t shots;
        if (g_game.game == GC_LABYRINTH)
            shots = std::min<int>(frames, shotCharge / 30.0f);
        else
            shots = std::min<int>(frames * 2, shotCharge / 30.0f);

        shotCharge = std::min<float>(30.0f, shotCharge - shots * 30.0f);

        uint64_t chain = 1 + pGem->shotFinal.rollChainLength();
        uint64_t targetsLeeched = 0;

        if (chain == 1)
        {
            if (shots > targetsInRange.size())
                shots = targetsInRange.size();

            targetsLeeched = shots;
            for (size_t ti = 0; ti < shots; ++ti)
            {
                // Roll individual hits without chain
                double damage = pGem->shotFinal.rollDamage();
                double crit = pGem->shotFinal.rollCritMultiplier();

                if (g_game.game == GC_LABYRINTH)
                {
                    if (pGem->shotFinal.component[COMPONENT_BLOODBOUND] > 0.0)
                        damage += pGem->shotFinal.component[COMPONENT_BLOODBOUND] * pGem->kills;
                }

                targetsInRange[ti]->receiveShotDamage(pGem->shotFinal, 1, damage, crit, pGem, false);
            }
        }
        else
        {
            // First plusHitPerTarget take hitsPerTarget+1, rest take hitsPerTarget
            uint64_t hitsPerTarget;
            uint64_t plusHitPerTarget;
            uint64_t shotsSpent = 0;

            // GCL allows hitting a target multiple times with the same chain
            if ((g_game.game != GC_LABYRINTH) && (chain >= targetsInRange.size()))
            {
                hitsPerTarget = shots;
                plusHitPerTarget = 0;
            }
            else
            {
                hitsPerTarget = (shots * chain) / targetsInRange.size();
                plusHitPerTarget = (shots * chain) % targetsInRange.size();
            }

            // Don't bother with individual rolls for chain, it's unbounded in GCL
            double damage = pGem->shotFinal.rollDamage();
            double crit = pGem->shotFinal.rollCritMultiplier();
            if (g_game.game == GC_LABYRINTH)
            {
                if (pGem->shotFinal.component[COMPONENT_BLOODBOUND] > 0.0)
                    damage += pGem->shotFinal.component[COMPONENT_BLOODBOUND] * pGem->kills;
            }

            for (size_t ti = 0; ti < targetsInRange.size(); ++ti)
            {
                Targetable* t = targetsInRange[ti];

                uint64_t hits = (ti < plusHitPerTarget) ? hitsPerTarget + 1 : hitsPerTarget;
                if (hits == 0)
                    break;

                shotsSpent += t->receiveShotDamage(pGem->shotFinal, hits, damage, crit, pGem, false);

                if (t->isKilled)
                {
                    uint64_t remainingTargets = targetsInRange.size() - ti;
                    // Reallocate unused shots to remaining targets
                    if (g_game.game == GC_LABYRINTH)
                    {
                        hitsPerTarget = (shots * chain - shotsSpent) / remainingTargets;
                        plusHitPerTarget = ((shots * chain - shotsSpent) % remainingTargets) + ti;
                    }
                    else if (hitsPerTarget < shots)
                    {
                        hitsPerTarget = std::min(shots, (shots * chain - shotsSpent) / remainingTargets);
                        plusHitPerTarget = ((shots * chain - shotsSpent) % remainingTargets) + ti;
                    }
                    break;
                }
            }
            targetsLeeched = shotsSpent;
        }

        if (pGem->shotFinal.component[COMPONENT_LEECH] > 0.0)
        {
            map.getManaPool().addMana(pGem->shotFinal.component[COMPONENT_LEECH] * targetsLeeched, true);
        }
    }
}

ShotData Trap::transformShotDataBuilding(const ShotData& sd)
{
    double damageMultiplier = 0.2;
    float speedMultiplier = 1.2f;
    double specialMultiplier = 1.8;

    ShotData out = sd;
    out.damageMin = std::max(1.0, std::round(sd.damageMin * damageMultiplier));
    out.damageMax = std::max(2.0, std::round(sd.damageMax * damageMultiplier));

    if (g_game.game == GC_LABYRINTH)
    {
        out.component[COMPONENT_POISON] *= specialMultiplier;
        out.component[COMPONENT_SLOW_POWER] *= specialMultiplier;
        out.component[COMPONENT_CHAIN] *= specialMultiplier;
        out.component[COMPONENT_LEECH] *= specialMultiplier;
        out.component[COMPONENT_ARMOR] *= specialMultiplier;
        out.component[COMPONENT_SHOCK] *= specialMultiplier;
        out.component[COMPONENT_CRITICAL_POWER] *= specialMultiplier;
        out.component[COMPONENT_BLOODBOUND] *= specialMultiplier;
    }
    else
    {
        out.fireRate = sd.fireRate * speedMultiplier;

        out.component[COMPONENT_POISON] *= specialMultiplier;
        out.component[COMPONENT_SLOW_DURATION] *= specialMultiplier;
        out.component[COMPONENT_LEECH] *= specialMultiplier;
        out.component[COMPONENT_ARMOR] *= specialMultiplier;
        out.component[COMPONENT_SUPPRESS] *= specialMultiplier;
        out.component[COMPONENT_CRITICAL_POWER] *= specialMultiplier;
    }

    return out;
}

void Trap::insertGem(Gem* pGem_)
{
    Building::insertGem(pGem_);

    cooldownTimer = 10000;

    pGem->recalculateShotData();

    if (g_game.game == GC_LABYRINTH)
        recalculateAdjacentGCLAmplifiers();
}

void Trap::removeGem()
{
    Gem* pOldGem = pGem;

    Building::removeGem();

    pOldGem->recalculateShotData();

    if (g_game.game == GC_LABYRINTH)
        recalculateAdjacentGCLAmplifiers();
}

void Trap::updateGem()
{
    cooldownTimer = 10000;
}

void Trap::tickCooldown(int frames)
{
    cooldownTimer -= frames * 1000 / 15;
    if (cooldownTimer <= 0)
    {
        shotCharge = (g_game.game == GC_LABYRINTH) ? 30.0f : 0.0f;
    }
}
