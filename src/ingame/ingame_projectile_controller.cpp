#include "ingame/ingame_projectile_controller.h"
#include "ingame/ingame_level_definition.h"
#include "ingame/ingame_mana_pool.h"

#include "wrapfbg.h"

IngameProjectileController::IngameProjectileController(IngameManaPool& mp_) : manaPool(mp_) {}

void IngameProjectileController::createTowerShots(const Tower& tower, Targetable* pTarget, int num)
{
    double damage = 0.0;
    for (int i = 0; i < num; ++i)
    {
        shots.emplace_back(tower, pTarget);
        damage += pTarget->calculateIncomingDamage(shots.back().damage, shots.back().crit);
    }
    pTarget->incomingShots += num;
    pTarget->incomingDamage += damage;
    if (pTarget->hp <= pTarget->incomingDamage)
    {
        pTarget->setKillingShot();
    }
}

void IngameProjectileController::shotHitsTarget(TowerShot* pShot)
{
    Targetable* pTarget = pShot->pTarget;
    if (pTarget != NULL)
    {
        --pTarget->incomingShots;
        pTarget->incomingDamage -= pTarget->calculateIncomingDamage(pShot->damage, pShot->crit);
        pTarget->receiveShotDamage(pShot->shot, pShot->damage, pShot->crit, pShot->pSourceGem);
    }

    // TODO chain hit
    if (pShot->shot.component[COMPONENT_LEECH] > 0.0)
    {
        if (pTarget->canLeech())
            manaPool.addMana(pShot->shot.component[COMPONENT_LEECH], true);
    }
}

void IngameProjectileController::warpShotsToTarget(Targetable* pTarget)
{
    for (std::list<TowerShot>::iterator it = shots.begin(); it != shots.end();)
    {
        if (it->pTarget == pTarget)
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

void IngameProjectileController::clearShotsFromTarget(
    const std::unordered_set<Targetable*>& invalidatedTargets)
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
            fbg_pixel(pFbg, scale * s.x + window.x, scale * s.y + window.y, s.RGB >> 16, s.RGB >> 8,
                s.RGB);
        }
    }
}
