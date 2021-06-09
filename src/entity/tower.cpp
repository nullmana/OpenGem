#include "entity/tower.h"
#include "entity/gem.h"

#include "ingame/ingame_map.h"

#include <algorithm>

void Tower::tick(IngameMap& map, int frames)
{
    if (pGem != NULL)
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
