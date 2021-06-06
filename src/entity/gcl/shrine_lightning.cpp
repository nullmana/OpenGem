#include "entity/gcl/shrine_lightning.h"
#include "entity/gem.h"

#include "ingame/ingame_map.h"

#include <cmath>

ShrineLightning::ShrineLightning(IngameMap& map_, int ix_, int iy_) : Shrine(map_, ix_, iy_)
{
    type = TILE_SHRINE_LI;
    shrineType = SHRINE_LIGHTNING;
}

bool ShrineLightning::canActivate()
{
    for (int j = std::max(0, iy - 1); j <= std::min(g_game.ingameMapHeight - 1, iy + 1); ++j)
    {
        for (int i = 0; i < g_game.ingameMapWidth; ++i)
        {
            const std::vector<Monster*>& tile = map.enemyController.getMonstersOnTile(j, i);
            for (Monster* m : tile)
                if (!m->isKilled)
                    return true;
        }
    }

    for (int j = 0; j < iy - 1; ++j)
    {
        for (int i = std::max(0, ix - 1); i <= std::min(g_game.ingameMapWidth - 1, ix + 1); ++i)
        {
            const std::vector<Monster*>& tile = map.enemyController.getMonstersOnTile(j, i);
            for (Monster* m : tile)
                if (!m->isKilled)
                    return true;
        }
    }

    for (int j = iy + 2; j < g_game.ingameMapHeight; ++j)
    {
        for (int i = std::max(0, ix - 1); i <= std::min(g_game.ingameMapWidth - 1, ix + 1); ++i)
        {
            const std::vector<Monster*>& tile = map.enemyController.getMonstersOnTile(j, i);
            for (Monster* m : tile)
                if (!m->isKilled)
                    return true;
        }
    }

    return false;
}

void ShrineLightning::activate(Gem* pGem)
{
    int numTargets = 0.5f + 0.5f * pow(5.0f + pGem->grade * 11.0f, 1.13f);

    cachedTargets = getTargets();

    for (size_t i = 0; (i < cachedTargets.size()) && (i < numTargets); ++i)
        cachedTargets[i]->receiveShotDamage();
}

std::vector<Targetable*> ShrineLightning::getTargets()
{
    std::vector<Targetable*> targets;

    for (int j = std::max(0, iy - 1); j <= std::min(g_game.ingameMapHeight - 1, iy + 1); ++j)
    {
        for (int i = 0; i < g_game.ingameMapWidth; ++i)
        {
            const std::vector<Monster*>& tile = map.enemyController.getMonstersOnTile(j, i);
            for (Monster* m : tile)
                if (!m->isKilled)
                    targets.push_back(m);
        }
    }

    for (int j = 0; j < iy - 1; ++j)
    {
        for (int i = std::max(0, ix - 1); i <= std::min(g_game.ingameMapWidth - 1, ix + 1); ++i)
        {
            const std::vector<Monster*>& tile = map.enemyController.getMonstersOnTile(j, i);
            for (Monster* m : tile)
                if (!m->isKilled)
                    targets.push_back(m);
        }
    }

    for (int j = iy + 2; j < g_game.ingameMapHeight; ++j)
    {
        for (int i = std::max(0, ix - 1); i <= std::min(g_game.ingameMapWidth - 1, ix + 1); ++i)
        {
            const std::vector<Monster*>& tile = map.enemyController.getMonstersOnTile(j, i);
            for (Monster* m : tile)
                if (!m->isKilled)
                    targets.push_back(m);
        }
    }

    return targets;
}