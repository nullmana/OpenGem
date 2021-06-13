#include "ingame/ingame_projectile_controller.h"
#include "ingame/ingame_level_definition.h"
#include "ingame/ingame_mana_pool.h"
#include "ingame/ingame_map.h"

#include "wrapfbg.h"

IngameProjectileController::IngameProjectileController(IngameMap& map_)
    : map(map_) {}

int IngameProjectileController::createTowerShots(const Tower& tower, Targetable* pTarget, int numShots)
{
    int shotsTaken = 0;
    double damage = 0.0;
    while (shotsTaken < numShots)
    {
        ++shotsTaken;

        shots.emplace_back(tower, pTarget);

        pTarget->incomingDamage += pTarget->calculateIncomingDamage(shots.back().damage, shots.back().crit);
        if (pTarget->hp <= pTarget->incomingDamage)
        {
            pTarget->setKillingShot();
            shots.back().isKillingShot = true;
            break;
        }
    }

    pTarget->incomingShots += shotsTaken;

    return shotsTaken;
}

void IngameProjectileController::shotHitsTarget(TowerShot* pShot)
{
    Targetable* pTarget = pShot->pTarget;
    uint32_t targetsLeeched = 0;

    if ((pTarget != NULL) && !(pTarget->isKilled))
    {
        --pTarget->incomingShots;
        pTarget->incomingDamage -= pTarget->calculateIncomingDamage(pShot->damage, pShot->crit);
        pTarget->receiveShotDamage(pShot->shot, 1, pShot->damage, pShot->crit, pShot->pSourceGem, pShot->isKillingShot);

        if (pTarget->canLeech())
            ++targetsLeeched;
    }

    uint32_t chain = 0;

    // No chain hit if hitting something that isn't a monster
    if ((pTarget == NULL) || !!(pTarget->type & TARGET_MONSTER))
        chain = pShot->shot.rollChainLength();

    if (chain > 0)
    {
        const float chainRangeSq = (g_game.game == GC_LABYRINTH)
                                       ? ((50.0f / 33.0f) * (50.0f / 33.0f))
                                       : ((70.0f / 17.0f) * (70.0f / 17.0f));

        std::vector<Targetable*> chainTargets =
            map.enemyController.getTargetsWithinRangeSq(pShot->y, pShot->x, chainRangeSq, TARGET_MONSTER, true);

        if (chain > chainTargets.size())
            chain = chainTargets.size();

        for (uint32_t i = 0; i < chain; ++i)
        {
            if (chainTargets[i] == pTarget)
                continue;

            chainTargets[i]->receiveShotDamage(pShot->shot, 1, pShot->damage, pShot->crit, pShot->pSourceGem, false);
            if (chainTargets[i]->canLeech())
                ++targetsLeeched;
        }
    }

    if (pShot->shot.component[COMPONENT_LEECH] > 0.0)
    {
        map.getManaPool().addMana(pShot->shot.component[COMPONENT_LEECH] * targetsLeeched, true);
    }
}

void IngameProjectileController::warpShotsToTarget(Targetable* pTarget)
{
    for (std::list<TowerShot>::iterator it = shots.begin(); it != shots.end();)
    {
        if (it->pTarget == pTarget)
        {
            it->x = pTarget->x;
            it->y = pTarget->y;
            shotHitsTarget(&(*it));
            shots.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

void IngameProjectileController::clearShotsFromTarget(const std::unordered_set<Targetable*>& invalidatedTargets)
{
    for (TowerShot& t : shots)
    {
        if (invalidatedTargets.count(t.pTarget) > 0)
            t.pTarget = NULL;
    }
}

void IngameProjectileController::clearShotsFromGem(const Gem* pGem)
{
    for (TowerShot& t : shots)
    {
        if (t.pSourceGem == pGem)
            t.pSourceGem = NULL;
    }
}

void IngameProjectileController::tickProjectiles(int frames)
{
    for (std::list<TowerShot>::iterator it = shots.begin(); it != shots.end();)
    {
        if (it->tick(frames))
        {
            shotHitsTarget(&(*it));
            shots.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

void IngameProjectileController::render(struct _fbg* pFbg, const Window& window) const
{
    float scale = window.width / float(g_game.ingameMapWidth);

    for (const TowerShot& s : shots)
    {
        if ((s.x > 0.0f) && (s.x < g_game.ingameMapWidth) && (s.y > 0.0f) &&
            (s.y < g_game.ingameMapHeight))
        {
            fbg_pixel(pFbg, scale * s.x + window.x, scale * s.y + window.y, s.RGB >> 16, s.RGB >> 8, s.RGB);
        }
    }
}
