#include "entity/trap.h"
#include "entity/gem.h"

#include "ingame/ingame_map.h"

#include <algorithm>
#include <cmath>

void Trap::tick(IngameMap& map, int frames)
{
    if ((pGem != NULL) && !pGem->isDragged)
    {
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
                    if ((x - m->x) * (x - m->x) + (y - m->y) * (y - m->y) < rangeSq)
                        targetsInRange.push_back(m);
                }
            }
        }

        int shots = frames;
        for (size_t i = 0; (shots > 0) && (i < targetsInRange.size()); ++i)
        {
            if (!targetsInRange[i]->isKilled)
            {
                targetsInRange[i]->receiveShotDamage(
                    pGem->shotFinal, pGem->shotFinal.rollDamage(), pGem);
                --shots;
            }
        }
    }
}

ShotData Trap::transformShotDataBuilding(const ShotData& sd)
{
    double damageMultiplier = 0.2;
    float speedMultiplier = 1.2f;

    ShotData out = sd;
    out.damageMin = std::max(1.0, std::round(sd.damageMin * damageMultiplier));
    out.damageMax = std::max(2.0, std::round(sd.damageMax * damageMultiplier));
    if (g_game.game != GC_LABYRINTH)
        out.fireRate = sd.fireRate * speedMultiplier;

    return out;
}

void Trap::insertGem(Gem* pGem_)
{
    Building::insertGem(pGem_);

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
