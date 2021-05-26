#include "ingame/ingame_projectile_controller.h"
#include "ingame/ingame_level_definition.h"

#include "wrapfbg.h"

#include <cstdio>

IngameProjectileController::IngameProjectileController() {}

void IngameProjectileController::createTowerShots(const Tower& tower, Targetable* pTarget, int num)
{
    for (int i = 0; i < num; ++i)
    {
        shots.emplace_back(tower, pTarget);
    }
    pTarget->incomingShots += num;
    pTarget->isKillingShotOnTheWay = true;
}

void IngameProjectileController::warpShotsToTarget(Targetable* pTarget)
{
    for (std::list<TowerShot>::iterator it = shots.begin(); it != shots.end();)
    {
        if (it->pTarget == pTarget)
        {
            --pTarget->incomingShots;
            pTarget->receiveShotDamage();
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

void IngameProjectileController::tickProjectiles(int frames)
{
    for (std::list<TowerShot>::iterator it = shots.begin(); it != shots.end();)
    {
        if (it->tick(frames))
        {
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
            fbg_pixel(pFbg, scale * s.x + window.x, scale * s.y + window.y, 0xDD, 0xDD, 0xEE);
        }
    }
}
