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
        cachedTargets.clear();
        switch (pGem->targetPriority)
        {
            case PRIORITY_NEAREST:
                map.enemyController.getTowerTargetsWithinRangeSq(cachedTargets, y, x, rangeSq, TARGET_ENEMY);

                std::sort(cachedTargets.begin(), cachedTargets.end(),
                    [](const Targetable* a, const Targetable* b) { return a->distanceToOrb < b->distanceToOrb; });

                map.structureController.getTargetableStructuresWithinRangeSq(cachedTargets, y, x, rangeSq);
                break;
            case PRIORITY_SWARMLINGS:
                map.enemyController.getTowerTargetsWithinRangeSq(cachedTargets, y, x, rangeSq,
                    TARGET_SWARMLING | TARGET_SPAWNLING);
                map.enemyController.getTowerTargetsWithinRangeSq(cachedTargets, y, x, rangeSq,
                    TARGET_ENEMY & ~(TARGET_SWARMLING | TARGET_SPAWNLING));
                map.structureController.getTargetableStructuresWithinRangeSq(cachedTargets, y, x, rangeSq);
                break;
            case PRIORITY_GIANTS:
                map.enemyController.getTowerTargetsWithinRangeSq(cachedTargets, y, x, rangeSq, TARGET_GIANT);
                map.enemyController.getTowerTargetsWithinRangeSq(cachedTargets, y, x, rangeSq,
                    TARGET_ENEMY & ~TARGET_GIANT);
                map.structureController.getTargetableStructuresWithinRangeSq(cachedTargets, y, x, rangeSq);
                break;
            case PRIORITY_RANDOM:
                map.enemyController.getTowerTargetsWithinRangeSq(cachedTargets, y, x, rangeSq, TARGET_ENEMY);
                map.structureController.getTargetableStructuresWithinRangeSq(cachedTargets, y, x, rangeSq);
                break;
            case PRIORITY_STRUCTURE:
                map.structureController.getTargetableStructuresWithinRangeSq(cachedTargets, y, x, rangeSq);
                map.enemyController.getTowerTargetsWithinRangeSq(cachedTargets, y, x, rangeSq, TARGET_ENEMY);
                break;
            case PRIORITY_SPECIAL:
                map.enemyController.getTowerTargetsWithinRangeSq(cachedTargets, y, x, rangeSq, TARGET_ENEMY);

                std::sort(cachedTargets.begin(), cachedTargets.end(),
                    [](const Targetable* a, const Targetable* b) { return a->sortBanishmentCost < b->sortBanishmentCost; });

                map.structureController.getTargetableStructuresWithinRangeSq(cachedTargets, y, x, rangeSq);
                break;
            case PRIORITY_ARMOR:
                map.enemyController.getTowerTargetsWithinRangeSq(cachedTargets, y, x, rangeSq, TARGET_ENEMY);

                std::sort(cachedTargets.begin(), cachedTargets.end(),
                    [](const Targetable* a, const Targetable* b) { return a->armor < b->armor; });

                map.structureController.getTargetableStructuresWithinRangeSq(cachedTargets, y, x, rangeSq);
                break;
            case PRIORITY_LEASTHP:
                map.enemyController.getTowerTargetsWithinRangeSq(cachedTargets, y, x, rangeSq, TARGET_ENEMY);

                std::sort(cachedTargets.begin(), cachedTargets.end(),
                    [](const Targetable* a, const Targetable* b) { return a->hp < b->hp; });

                map.structureController.getTargetableStructuresWithinRangeSq(cachedTargets, y, x, rangeSq);
                break;
        }
        map.enemyController.getTowerTargetsWithinRangeSq(cachedTargets, y, x, rangeSq, TARGET_ENEMY);

        std::sort(cachedTargets.begin(), cachedTargets.end(),
            [](const Targetable* a, const Targetable* b) { return a->distanceToOrb < b->distanceToOrb; });

        map.structureController.getTargetableStructuresWithinRangeSq(cachedTargets, y, x, rangeSq);

        cachedTargetsValid = true;
    }

    if (cachedTargets.empty())
        return NULL;

    for (Targetable* t : cachedTargets)
    {
        if (!t->isKillingShotOnTheWay)
            return t;
    }

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
