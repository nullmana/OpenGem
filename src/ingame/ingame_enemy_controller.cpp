#include "ingame/ingame_enemy_controller.h"
#include "ingame/ingame_mana_pool.h"
#include "ingame/ingame_map.h"
#include "ingame/ingame_pathfinder.h"

#include "wrapfbg.h"

#include <cstdio>
#include <unordered_set>

IngameEnemyController::IngameEnemyController(IngameManaPool& mp_)
    : manaPool(mp_), monstersOnTile(g_game.ingameMapHeight, g_game.ingameMapWidth)
{
}

void IngameEnemyController::spawnMonsters(const IngamePathfinder& pathfinder, int num)
{
    static int nodeOffset = 0;
    const std::vector<const MonsterSpawnNode*> nodes = pathfinder.getMonsterSpawnNodes();

    MonsterPrototype mp;
    mp.hp = 50.0;
    mp.armor = 2.0;
    mp.mana = 8.0;
    mp.banishmentCostMultiplier = 1.0;

    for (int i = 0; i < num; ++i)
    {
        // Multiply by large primes to make node order less consistent
        monsters.emplace_back(
            nodes[(797 * nodeOffset + 337 * i) % nodes.size()], pathfinder.getOrbNode(), mp);
    }
    ++nodeOffset;

    printf("Monsters: %lu\n", monsters.size());
}

void IngameEnemyController::tickMonsters(IngameMap& map, int frames)
{
    std::unordered_set<Targetable*> invalidatedWithShots;

    for (std::list<Monster>::iterator it = monsters.begin(); it != monsters.end();)
    {
        if (it->tick(map, frames))
        {
            manaPool.addMana(it->mana, true);
            if (it->incomingShots > 0)
                invalidatedWithShots.insert(&(*it));
            monsters.erase(it++);
        }
        else
        {
            ++it;
        }
    }

    map.projectileController.clearShotsFromTarget(invalidatedWithShots);

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
            fbg_pixel(pFbg, scale * x + window.x, scale * y + window.y, (m.color >> 16) & 0xFF,
                (m.color >> 8) & 0xFF, m.color & 0xFF);
        }
    }

    for (const Monster& m : monsters)
    {
        float y = m.y - 1.2f;
        float x1 = m.x - 0.6f;
        float x2 = m.x + 0.6f;

        if ((m.healthBarTimer > 0) && (m.x > 0.0f) && (m.x < g_game.ingameMapWidth) && (y > 0.0f) &&
            (m.y < g_game.ingameMapHeight))
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

std::vector<Targetable*> IngameEnemyController::getTargetsWithinRangeSq(
    float y, float x, float rangeSq, bool ignoreKillingShot)
{
    std::vector<Targetable*> targets;

    for (Monster& m : monsters)
    {
        if (!m.isKilled && (ignoreKillingShot || !m.isKillingShotOnTheWay) &&
            ((m.y - y) * (m.y - y) + (m.x - x) * (m.x - x) <= rangeSq))
        {
            targets.push_back(&m);
        }
    }

    return targets;
}

bool IngameEnemyController::hasTargetsWithinRangeSq(
    float y, float x, float rangeSq, bool ignoreKillingShot) const
{
    for (const Monster& m : monsters)
    {
        if (!m.isKilled && (ignoreKillingShot || !m.isKillingShotOnTheWay) &&
            ((m.y - y) * (m.y - y) + (m.x - x) * (m.x - x) <= rangeSq))
        {
            return true;
        }
    }
    return false;
}
