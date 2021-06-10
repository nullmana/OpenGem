#include "entity/tower.h"
#include "entity/gem.h"

#include "ingame/ingame_map.h"

#include <algorithm>

void Tower::tick(IngameMap& map, int frames)
{
    if ((pGem != NULL) && !pGem->isDragged)
    {
        const float rangeSq = pGem->shotFinal.rangeSq();
        std::vector<Targetable*> targetsInRange =
            map.enemyController.getTowerTargetsWithinRangeSq(y, x, rangeSq);

        if (!targetsInRange.empty())
        {
            std::sort(targetsInRange.begin(), targetsInRange.end(),
                [](const Targetable* a, const Targetable* b) {
                    return a->distanceToOrb < b->distanceToOrb;
                });
        }
        else
        {
            targetsInRange =
                map.structureController.getTargetableStructuresWithinRangeSq(y, x, rangeSq);
        }

        int shots = frames;
        for (size_t i = 0; (shots > 0) && (i < targetsInRange.size()); ++i)
        {
            Targetable* pTarget = targetsInRange[i];
            map.projectileController.createTowerShots(*this, pTarget, 1);
            --shots;
        }
    }
}

void Tower::insertGem(Gem* pGem_)
{
    Building::insertGem(pGem_);

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
