#include "entity/tower.h"
#include "entity/gem.h"

#include "ingame/ingame_map.h"

#include <algorithm>

Tower::Tower(int ix_, int iy_)
    : Building(ix_, iy_)
{
    type = TILE_TOWER;
    shotCharge = 0.0f;
    cachedTargetsValid = false;
    cooldownTimer = 0;
}

void Tower::tick(IngameMap& map, int frames)
{
    if ((pGem != NULL) && !pGem->isDragged && (frames > 0))
    {
        shotCharge += frames * pGem->shotFinal.fireRate;
        if (shotCharge >= 30.0f)
        {
            Targetable* pTarget = NULL;
            cachedTargetsValid = false;

            int availableShots;
            // Just use estimate of 0.45 instead of randomizing here, it might occasionally
            // shift a shot back or forward a frame
            if (g_game.game == GC_LABYRINTH)
                availableShots = std::min<int>(frames, shotCharge / 30.0f);
            else
                availableShots = std::min<int>(frames * 2, shotCharge / 29.55f);

            while ((availableShots > 0) && (shotCharge >= 30.0f))
            {
                if ((pTarget == NULL) || pTarget->isKilled || pTarget->isKillingShotOnTheWay)
                {
                    pTarget = pickTarget(map);
                    if (pTarget == NULL)
                        break;
                }

                int shotsTaken = map.projectileController.createTowerShots(*this, pTarget, availableShots);
                availableShots -= shotsTaken;
                if (g_game.game == GC_LABYRINTH)
                    shotCharge -= shotsTaken * 30.0f;
                else
                    shotCharge -= shotsTaken * (30.0f - 0.9f * (rand() / float(RAND_MAX)));
            }
        }

        shotCharge = std::min<float>(30.0f, shotCharge);
        cachedTargets.clear();
    }
}

Targetable* Tower::pickTarget(IngameMap& map)
{
    const float rangeSq = pGem->shotFinal.rangeSq();

    if (!cachedTargetsValid)
    {
        cachedTargets = map.enemyController.getTowerTargetsWithinRangeSq(y, x, rangeSq, TARGET_ENEMY);

        std::sort(cachedTargets.begin(), cachedTargets.end(),
            [](const Targetable* a, const Targetable* b) { return a->distanceToOrb < b->distanceToOrb; });

        if (cachedTargets.empty())
            cachedTargets = map.structureController.getTargetableStructuresWithinRangeSq(y, x, rangeSq);

        cachedTargetsValid = true;
    }

    if (cachedTargets.empty())
        return NULL;

    for (Targetable* t : cachedTargets)
    {
        if (!t->isKillingShotOnTheWay)
            return t;
    }

    // If still no target, all cached targets have been killed, try structure targets here as well
    cachedTargets = map.structureController.getTargetableStructuresWithinRangeSq(y, x, rangeSq);

    if (!cachedTargets.empty())
        return cachedTargets[0];

    return NULL;
}

void Tower::insertGem(Gem* pGem_)
{
    Building::insertGem(pGem_);

    cooldownTimer = 10000;

    pGem->recalculateShotData();

    if (g_game.game == GC_LABYRINTH)
        recalculateAdjacentGCLAmplifiers();
}

void Tower::removeGem()
{
    Gem* pOldGem = pGem;

    Building::removeGem();

    pOldGem->recalculateShotData();

    if (g_game.game == GC_LABYRINTH)
        recalculateAdjacentGCLAmplifiers();
}

void Tower::updateGem()
{
    cooldownTimer = 10000;
}

void Tower::tickCooldown(int frames)
{
    cooldownTimer -= frames * 50;
    if (cooldownTimer <= 0)
    {
        shotCharge = (g_game.game == GC_LABYRINTH) ? 30.0f : 0.0f;
    }
}
