#include "entity/tower.h"

#include "ingame/ingame_map.h"

#include <algorithm>

void Tower::tick(IngameMap &map, int frames)
{
	std::vector<Monster*> targetsInRange = map.enemyController.getLiveTargetsWithinRangeSq(y, x, 100.0f);

	if (!targetsInRange.empty())
	{
		std::sort(targetsInRange.begin(), targetsInRange.end(),
				[](const Monster *a, const Monster *b) { return a->distanceToOrb < b->distanceToOrb; });
	}

	int shots = frames;
	for (size_t i = 0; (shots > 0) && (i < targetsInRange.size()); ++i)
	{
		Targetable *pTarget = targetsInRange[i];
		map.projectileController.createTowerShots(*this, pTarget, 1);
		--shots;
	}
}
