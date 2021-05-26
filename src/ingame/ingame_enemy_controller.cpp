#include "ingame/ingame_enemy_controller.h"
#include "ingame/ingame_map.h"
#include "ingame/ingame_pathfinder.h"

#include "wrapfbg.h"

#include <cstdio>
#include <unordered_set>

IngameEnemyController::IngameEnemyController()
    : monstersOnTile(g_game.ingameMapHeight, g_game.ingameMapWidth)
{
}

void IngameEnemyController::spawnMonsters(const IngamePathfinder& pathfinder, int num)
{
    static int nodeOffset                            = 0;
    const std::vector<const MonsterSpawnNode*> nodes = pathfinder.getMonsterSpawnNodes();
    for (int i = 0; i < num; ++i)
    {
        // Multiply by large primes to make node order less consistent
        monsters.emplace_back(
            nodes[(797 * nodeOffset + 337 * i) % nodes.size()], pathfinder.getOrbNode());
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
        if ((m.x > 0.0f) && (m.x < g_game.ingameMapWidth) && (m.y > 0.0f) &&
            (m.y < g_game.ingameMapHeight))
        {
            fbg_pixel(pFbg, scale * m.x + window.x, scale * m.y + window.y, (m.color >> 4) & 0xFF,
                (m.color >> 2) & 0xFF, m.color & 0xFF);
        }
    }
}

std::vector<Monster*> IngameEnemyController::getLiveTargetsWithinRangeSq(
    float y, float x, float rangeSq)
{
    std::vector<Monster*> targets;

    for (Monster& m : monsters)
    {
        if (!m.isKilled && !m.isKillingShotOnTheWay &&
            ((m.y - y) * (m.y - y) + (m.x - x) * (m.x - x) <= rangeSq))
            targets.push_back(&m);
    }

    return targets;
}
