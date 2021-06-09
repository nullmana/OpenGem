#include "entity/gcl/shrine_charged_bolts.h"
#include "entity/gem.h"

#include "ingame/ingame_map.h"

#include <cmath>

ShrineChargedBolts::ShrineChargedBolts(IngameMap& map_, int ix_, int iy_) : Shrine(map_, ix_, iy_)
{
    type = TILE_SHRINE_CB;
    shrineType = SHRINE_CHARGED_BOLTS;
}

bool ShrineChargedBolts::canActivate()
{
    return map.enemyController.hasShrineTargetsWithinRangeSq(y, x, 4.9f * 4.9f);
}

void ShrineChargedBolts::activate(Gem* pGem)
{
    int numTargets = 0.5f + 0.5f * pow(5.0f + pGem->grade * 11.0f, 1.13f);

    cachedTargets = getTargets();

    for (size_t i = 0; (i < cachedTargets.size()) && (i < numTargets); ++i)
        cachedTargets[i]->receiveShotDamage();
}

std::vector<Targetable*> ShrineChargedBolts::getTargets()
{
    return map.enemyController.getShrineTargetsWithinRangeSq(y, x, 4.9f * 4.9f);
}
