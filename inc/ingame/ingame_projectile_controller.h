#pragma once

#include "entity/targetable.h"
#include "entity/tower_shot.h"

#include "interface/window.h"

#include <list>
#include <unordered_set>

class Tower;
class IngameMap;
struct _fbg;

class IngameProjectileController
{
private:
    IngameMap& map;
    std::list<TowerShot> shots;

    void shotHitsTarget(TowerShot*);

public:
    IngameProjectileController(IngameMap& map_);

    /*!
     * @brief Creates up to `num` shots targeting `pTarget`, or however many are required to
     *        mark target for death.
     * @param[in] tower   Tower from which shot originates
     * @param[in] pTarget Target which is being shot at
     * @param[in] numShots Maximum number of shots to fire
     * @return Number of shots created
     */
    int createTowerShots(const Tower& tower, Targetable* pTarget, int numShots);

    /*!
     * @brief Monsters which reach the orb are immediately hit by all shots heading towards them
     * @param[in] pTarget Target which reached the orb
     */
    void warpShotsToTarget(Targetable* pTarget);
    /*!
     * @brief Clear targets on any killed monsters
     * @param[in] targets Set of INVALIDATED pointers for comparison, should NOT be dereferenced
     */
    void clearShotsFromTarget(const std::unordered_set<Targetable*>& invalidatedTargets);
    /*!
     * @brief Clear source gem any projectiles from this gem
     * @param[in] pGem Gem to be removed
     */
    void clearShotsFromGem(const Gem* pGem);

    void tickProjectiles(int frames);

    void render(struct _fbg* pFbg, const Window& window) const;
};
