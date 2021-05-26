#pragma once

#include "entity/targetable.h"
#include "entity/tower_shot.h"

#include "interface/window.h"

#include <list>
#include <unordered_set>

class Tower;
struct _fbg;

class IngameProjectileController
{
private:
	std::list<TowerShot> shots;
public:
	IngameProjectileController();

	void createTowerShots(const Tower &tower, Targetable *pTarget, int num);

	/*!
	 * @brief Monsters which reach the orb are immediately hit by all shots heading towards them
	 * @param[in] pTarget Target which reached the orb
	 */
	void warpShotsToTarget(Targetable *pTarget);
	/*!
	 * @brief Clear targets on any killed monsters
	 * @param[in] targets Set of INVALIDATED pointers for comparison, should NOT be dereferenced
	 */
	void clearShotsFromTarget(const std::unordered_set<Targetable*> &invalidatedTargets);

	void tickProjectiles(int frames);

	void render(struct _fbg *pFbg, const Window &window)const;
};
