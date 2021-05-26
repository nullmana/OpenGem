#include "ingame/ingame_building_controller.h"

#include "constants/game_header.h"

#include "entity/gem.h"

#include "wrapfbg.h"

#include <cstdlib>

IngameBuildingController::IngameBuildingController() {}

void IngameBuildingController::render(struct _fbg* pFbg, const Window& window) const
{
    float scale    = window.width / float(g_game.ingameMapWidth);
    float gemScale = 0.85f * scale;

    for (const Tower& t : towers)
    {
        if ((t.pGem != NULL) && !t.pGem->isDragged)
        {
            fbg_rect(pFbg, t.x * scale + window.x - 0.5f * gemScale,
                t.y * scale + window.y - 0.5f * gemScale, gemScale, gemScale,
                (t.pGem->color >> 4) & 0xFF, (t.pGem->color >> 2) & 0xFF, t.pGem->color & 0xFF);
        }
    }

    for (const Trap& t : traps)
    {
        if ((t.pGem != NULL) && !t.pGem->isDragged)
        {
            fbg_rect(pFbg, t.x * scale + window.x - 0.5f * gemScale,
                t.y * scale + window.y - 0.5f * gemScale, gemScale, gemScale,
                (t.pGem->color >> 4) & 0xFF, (t.pGem->color >> 2) & 0xFF, t.pGem->color & 0xFF);
        }
    }
}

void IngameBuildingController::tickBuildings(IngameMap& map, int frames)
{
    for (Tower& t : towers)
        t.tick(map, frames);

    for (Trap& t : traps)
        t.tick(map, frames);
}

Tower& IngameBuildingController::addTower(int x, int y)
{
    towers.emplace_back(x, y);
    return towers.back();
}

Trap& IngameBuildingController::addTrap(int x, int y)
{
    traps.emplace_back(x, y);
    return traps.back();
}

void IngameBuildingController::destroyTower(Tower* pTower)
{
    for (std::list<Tower>::iterator it = towers.begin(); it != towers.end(); ++it)
    {
        if (pTower == &(*it))
        {
            towers.erase(it);
            break;
        }
    }
}

void IngameBuildingController::destroyTrap(Trap* pTrap)
{
    for (std::list<Trap>::iterator it = traps.begin(); it != traps.end(); ++it)
    {
        if (pTrap == &(*it))
        {
            traps.erase(it);
            break;
        }
    }
}

void IngameBuildingController::destroyBuilding(Building* pBuilding)
{
    switch (pBuilding->type)
    {
        case BUILDING_TOWER:
            destroyTower(reinterpret_cast<Tower*>(pBuilding));
            break;
        case BUILDING_TRAP:
            destroyTrap(reinterpret_cast<Trap*>(pBuilding));
            break;
    }
}
