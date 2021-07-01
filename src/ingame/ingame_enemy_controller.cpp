#include "ingame/ingame_enemy_controller.h"
#include "ingame/ingame_mana_pool.h"
#include "ingame/ingame_map.h"
#include "ingame/ingame_pathfinder.h"

#include "wrapfbg.h"

#include <algorithm>
#include <unordered_set>

#ifdef DEBUG
#include <cstdio>
#endif

IngameEnemyController::IngameEnemyController(IngameManaPool& mp_)
    : manaPool(mp_), monstersOnTile(g_game.ingameMapHeight, g_game.ingameMapWidth)
{
    pendingMonsterClock = 0;
}

void IngameEnemyController::spawnMonsters(const IngamePathfinder& pathfinder, int num)
{
    static int nodeOffset = 0;
    const std::vector<const MonsterSpawnNode*> nodes = pathfinder.getMonsterSpawnNodes();

    MonsterPrototype mp;
    mp.hp = 10.0;
    mp.armor = 2.0;
    mp.mana = 8.0;
    mp.banishmentCostMultiplier = 1.0;
    mp.speed = 0.4f + (rand() / float(RAND_MAX));
    mp.type = TARGET_REAVER;

    for (int i = 0; i < num; ++i)
    {
        // Multiply by large primes to make node order less consistent
        monsters.emplace_back(nodes[(797 * nodeOffset + 337 * i) % nodes.size()], pathfinder.getOrbNode(), mp);
    }
    ++nodeOffset;

#ifdef DEBUG
    printf("Monsters: %lu\n", monsters.size());
#endif
}

void IngameEnemyController::tickMonsters(IngameMap& map, int frames)
{
    std::unordered_set<Targetable*> invalidatedWithShots;

    for (std::list<Monster>::iterator it = monsters.begin(); it != monsters.end();)
    {
        if (it->tick(map, frames))
        {
            if (it->manaBindTimer <= 0)
                manaPool.addMana(it->mana, true);

            if (it->incomingShots > 0)
                invalidatedWithShots.insert(&(*it));

            if (it->isSelectedTarget)
                map.setSelectedTarget(NULL);

            monsters.erase(it++);
        }
        else
        {
            ++it;
        }
    }

    map.projectileController.clearShotsFromTarget(invalidatedWithShots);

    if (frames > 0)
    {
        // Check if pending monsters should enter
        pendingMonsterClock += frames;
        auto it = pendingMonsters.begin();

        while (it != pendingMonsters.end())
        {
            if (it->first <= pendingMonsterClock)
            {
                monsters.emplace_back(map.pathfinder.pickMonsterSpawnNode(), map.pathfinder.getOrbNode(), it->second);
                it = pendingMonsters.erase(it);
            }
            else
            {
                break;
            }
        }
    }

    monstersOnTile.clear();
    for (Monster& m : monsters)
    {
        int ix = m.x;
        int iy = m.y;

        // Nudge monsters slightly off-screen to make sure they are checked by traps on the border
        if (ix == -1)
            ix = 0;
        else if (ix == g_game.ingameMapWidth)
            ix = g_game.ingameMapWidth - 1;
        if (iy == -1)
            iy = 0;
        else if (iy == g_game.ingameMapHeight)
            iy = g_game.ingameMapHeight - 1;

        if ((ix >= 0) && (ix < g_game.ingameMapWidth) && (iy >= 0) && (iy < g_game.ingameMapHeight))
            monstersOnTile.at(iy, ix).push_back(&m);
    }
}

void IngameEnemyController::addPendingMonsters(const MonsterPrototype& mp, const std::vector<int>& times)
{
    for (size_t i = 0; i < times.size(); ++i)
        pendingMonsters.emplace(pendingMonsterClock + times[i], mp);

#ifdef DEBUG
    printf("Pending Monsters: %lu\n", pendingMonsters.size());
#endif
}

void IngameEnemyController::forceRepath(int x, int y, int w, int h)
{
    for (Monster& m : monsters)
    {
        if ((m.nextX >= x) && (m.nextX < x + w) && (m.nextY >= y) && (m.nextY < y + h))
            m.pickNextTarget();
    }
}

void IngameEnemyController::render(struct _fbg* pFbg, const Window& window) const
{
    float scale = window.width / float(g_game.ingameMapWidth);

    for (const Monster& m : monsters)
    {
        float x = m.x;
        float y = m.y;
        if ((g_game.game == GC_LABYRINTH) && (m.shockTimer > 0))
        {
            x = m.shockX;
            y = m.shockY;
        }
        if ((x > 0.0f) && (x < g_game.ingameMapWidth) && (y > 0.0f) && (y < g_game.ingameMapHeight))
        {
            const float ss = scale * m.scale;
            float vx = cos(m.motionAngle) * ss;
            float vy = sin(m.motionAngle) * ss;

            fbgx_tri(pFbg, scale * x + vx + window.x, scale * y + vy + window.y,
                scale * x - vx + vy * 0.4f + window.x, scale * y - vy - vx * 0.4f + window.y,
                scale * x - vx - vy * 0.4f + window.x, scale * y - vy + vx * 0.4f + window.y,
                m.color >> 16, m.color >> 8, m.color);
        }
    }

    for (const Monster& m : monsters)
    {
        float y = m.y - 1.2f;
        float x1 = m.x - 0.6f;
        float x2 = m.x + 0.6f;

        if ((m.healthBarTimer > 0) &&
            (m.x > 0.0f) && (m.x < g_game.ingameMapWidth) &&
            (y > 0.0f) && (m.y < g_game.ingameMapHeight))
        {
            float x3 = (x2 - x1) * (m.hp / m.hpMax) + x1;
            if (x1 < 0.0f)
                x1 = 0.0f;
            if (x2 > g_game.ingameMapWidth)
                x2 = g_game.ingameMapWidth;

            y = y * scale + window.y;

            if (x3 > 0.0f)
            {
                fbg_line(pFbg, x1 * scale + window.x, y, x3 * scale + window.x, y,
                    m.hp < m.hpMax * 0.67 ? 0xFF : 0, m.hp > m.hpMax * 0.33 ? 0xFF : 0, 0x00);
            }
            if (x3 < g_game.ingameMapWidth)
            {
                fbg_line(
                    pFbg, x3 * scale + window.x, y, x2 * scale + window.x, y, 0x10, 0x10, 0x10);
            }
        }
    }
}

std::vector<Targetable*>& IngameEnemyController::getTargetsWithinRangeSq(std::vector<Targetable*>& targets,
    float y, float x, float rangeSq, uint32_t typeMask, bool ignoreKillingShot)
{
    const double fullMapRangeSq = g_game.ingameMapWidth * g_game.ingameMapWidth +
                                  g_game.ingameMapHeight + g_game.ingameMapHeight;

    if (!!(typeMask & TARGET_MONSTER))
    {
        if (rangeSq <= fullMapRangeSq / 2)
        {
            int range = ceil(sqrt(rangeSq));
            int ix = x;
            int iy = y;

            for (int j = std::max(0, iy - range); j <= std::min(g_game.ingameMapHeight - 1, iy + range); ++j)
            {
                for (int i = std::max(0, ix - range); i <= std::min(g_game.ingameMapWidth - 1, ix + range); ++i)
                {
                    // Add some tolerance to avoid leaking to rounding errors, shouldn't be common or noticable
                    if (((j - iy) * (j - iy) + (i - ix) * (i - ix)) < rangeSq + 2)
                    {
                        std::vector<Monster*>& mt = monstersOnTile.at(j, i);
                        for (Monster* m : mt)
                        {
                            if (m->canBeTargeted() && (ignoreKillingShot || !m->isKillingShotOnTheWay) &&
                                (typeMask & m->type) &&
                                isTargetWithinRangeSq(m, y, x, rangeSq))
                            {
                                targets.push_back(m);
                            }
                        }
                    }
                }
            }
        }
        else if (rangeSq >= fullMapRangeSq)
        {
            for (Monster& m : monsters)
            {
                if (m.canBeTargeted() && (ignoreKillingShot || !m.isKillingShotOnTheWay) && (typeMask & m.type))
                {
                    targets.push_back(&m);
                }
            }
        }
        else
        {
            for (Monster& m : monsters)
            {
                if (m.canBeTargeted() && (ignoreKillingShot || !m.isKillingShotOnTheWay) &&
                    (typeMask & m.type) &&
                    isTargetWithinRangeSq(&m, y, x, rangeSq))
                {
                    targets.push_back(&m);
                }
            }
        }
    }

    return targets;
}

bool IngameEnemyController::hasTargetsWithinRangeSq(float y, float x, float rangeSq, uint32_t typeMask, bool ignoreKillingShot) const
{
    if (!!(typeMask & TARGET_MONSTER))
    {
        for (const Monster& m : monsters)
        {
            if (m.canBeTargeted() && (ignoreKillingShot || !m.isKillingShotOnTheWay) && (typeMask & m.type) &&
                isTargetWithinRangeSq(&m, y, x, rangeSq))
            {
                return true;
            }
        }
    }
    return false;
}

Targetable* IngameEnemyController::getTargetAtPosition(float y, float x)
{
    int ix = x;
    int iy = y;
    Monster* pNearest = NULL;
    float minDistanceSq = 1000.0f;

    for (int j = std::max(0, iy - 1); j <= std::min(g_game.ingameMapHeight - 1, iy + 1); ++j)
    {
        for (int i = std::max(0, ix - 1); i <= std::min(g_game.ingameMapWidth - 1, ix + 1); ++i)
        {
            std::vector<Monster*>& mt = monstersOnTile.at(j, i);
            for (Monster* m : mt)
            {
                float distanceSq = (m->y - y) * (m->y - y) + (m->x - x) * (m->x - x);
                if ((distanceSq <= (m->scale * m->scale)) && (distanceSq <= minDistanceSq))
                {
                    pNearest = m;
                    minDistanceSq = distanceSq;
                }
            }
        }
    }

    return pNearest;
}

Targetable* IngameEnemyController::getNextTarget(Targetable* pTarget, int increment)
{
    std::vector<Targetable*> targets;

    for (Monster& m : monsters)
        targets.push_back(&m);

    if (targets.empty())
        return NULL;
    else if ((targets.size() == 1) || (increment == 0))
        return pTarget;

    std::sort(targets.begin(), targets.end(),
        [](const Targetable* a, const Targetable* b) { return a->y > b->y; });

    std::vector<Targetable*>::iterator it = std::find(targets.begin(), targets.end(), pTarget);
    if (it != targets.end())
    {
        if (increment > 0)
        {
            if (it + 1 == targets.end())
                return targets.front();
            else
                return *(it + 1);
        }
        else
        {
            if (it == targets.begin())
                return targets.back();
            else
                return *(it - 1);
        }
    }
    return NULL;
}
