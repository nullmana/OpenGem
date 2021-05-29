#include "ingame/ingame_building_controller.h"
#include "ingame/ingame_level_definition.h"

#include "constants/game_header.h"

#include "entity/gem.h"

#include "wrapfbg.h"

#include <algorithm>
#include <cstdlib>

IngameBuildingController::IngameBuildingController(const IngameLevelDefinition& level)
    : orb(level.orbX, level.orbY)
{
    amplifierRecalculateRequired = true;
}

#ifdef DEBUG
#include "ingame/ingame_core.h"

static void debugDrawAmplifierDirections(
    struct _fbg* pFbg, const Window& window, const Building* pBuilding)
{
    struct _fbg_glfw_context* pGlfwContext = (struct _fbg_glfw_context*)pFbg->user_context;
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pGlfwContext->window);
    IngameInputHandler* pInputHandler = &pCore->inputHandler;

    float scale = window.width / float(g_game.ingameMapWidth);

    if (pInputHandler->getInputState() == INPUT_BUILD_AMPLIFIER)
    {
        int x1 = pBuilding->x * scale + window.x;
        int y1 = pBuilding->y * scale + window.y;

        for (const Amplifiable* a : pBuilding->amplifying)
        {
            fbg_line(pFbg, x1, y1, (a->ix + 0.5f) * scale + window.x,
                (a->iy + 0.5f) * scale + window.y, 0x00, 0xFF,
                (pBuilding->type == BUILDING_AMPLIFIER) ? 0xFF : 0x00);
        }
    }
}
#endif

void IngameBuildingController::render(struct _fbg* pFbg, const Window& window) const
{
    float scale = window.width / float(g_game.ingameMapWidth);
    float gemScale = 0.85f * scale * g_game.ingameBuildingSize;

    for (const Tower& t : towers)
    {
        if ((t.pGem != NULL) && !t.pGem->isDragged)
        {
            fbg_rect(pFbg, t.x * scale + window.x - 0.5f * gemScale,
                t.y * scale + window.y - 0.5f * gemScale, gemScale, gemScale,
                (t.pGem->color >> 16) & 0xFF, (t.pGem->color >> 8) & 0xFF, t.pGem->color & 0xFF);
        }
#ifdef DEBUG
        debugDrawAmplifierDirections(pFbg, window, &t);
#endif
    }

    for (const Trap& t : traps)
    {
        if ((t.pGem != NULL) && !t.pGem->isDragged)
        {
            fbg_rect(pFbg, t.x * scale + window.x - 0.5f * gemScale,
                t.y * scale + window.y - 0.5f * gemScale, gemScale, gemScale,
                (t.pGem->color >> 16) & 0xFF, (t.pGem->color >> 8) & 0xFF, t.pGem->color & 0xFF);
        }
#ifdef DEBUG
        debugDrawAmplifierDirections(pFbg, window, &t);
#endif
    }

    for (const Amplifier& t : amplifiers)
    {
        if ((t.pGem != NULL) && !t.pGem->isDragged)
        {
            fbg_rect(pFbg, t.x * scale + window.x - 0.5f * gemScale,
                t.y * scale + window.y - 0.5f * gemScale, gemScale, gemScale,
                (t.pGem->color >> 16) & 0xFF, (t.pGem->color >> 8) & 0xFF, t.pGem->color & 0xFF);
        }
#ifdef DEBUG
        debugDrawAmplifierDirections(pFbg, window, &t);
#endif
    }

    if (orb.isBroken())
    {
        fbg_rect(pFbg, (orb.x - 0.5f * g_game.ingameBuildingSize) * scale + window.x,
            (orb.y - 0.5f * g_game.ingameBuildingSize) * scale + window.y,
            scale * g_game.ingameBuildingSize, scale * g_game.ingameBuildingSize, 0x08, 0x08, 0x20);
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

    Tower* pTower = &towers.back();

    std::vector<Amplifier*> sources = getAdjacentAmplifiers(pTower);

    for (Amplifier* a : sources)
        linkAmplifier(a, pTower);

    return towers.back();
}

Trap& IngameBuildingController::addTrap(int x, int y)
{
    traps.emplace_back(x, y);

    Trap* pTrap = &traps.back();

    std::vector<Amplifier*> sources = getAdjacentAmplifiers(pTrap);

    for (Amplifier* a : sources)
        linkAmplifier(a, pTrap);

    return traps.back();
}

Amplifier& IngameBuildingController::addAmplifier(int x, int y)
{
    amplifiers.emplace_back(x, y);

    Amplifier* pAmplifier = &amplifiers.back();

    std::vector<Amplifiable*> targets = getAdjacentAmplifiableBuildings(pAmplifier);

    for (Amplifiable* a : targets)
        linkAmplifier(pAmplifier, a);

    return amplifiers.back();
}

void IngameBuildingController::destroyTower(Tower* pTower)
{
    for (std::list<Tower>::iterator it = towers.begin(); it != towers.end(); ++it)
    {
        if (pTower == &(*it))
        {
            removeBuildingAmplifiers(pTower);
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
            removeBuildingAmplifiers(pTrap);
            traps.erase(it);
            break;
        }
    }
}

void IngameBuildingController::destroyAmplifier(Amplifier* pAmplifier)
{
    for (std::list<Amplifier>::iterator it = amplifiers.begin(); it != amplifiers.end(); ++it)
    {
        if (pAmplifier == &(*it))
        {
            removeBuildingAmplifiers(pAmplifier);
            amplifiers.erase(it);
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
        case BUILDING_AMPLIFIER:
            destroyAmplifier(reinterpret_cast<Amplifier*>(pBuilding));
            break;
    }
}

void IngameBuildingController::linkAmplifier(Amplifier* pAmplifier, Amplifiable* pAmplified)
{
    pAmplifier->amplifying.push_back(pAmplified);
    pAmplified->amplifying.push_back(pAmplifier);

    amplifierRecalculateRequired = true;
}

void IngameBuildingController::removeBuildingAmplifiers(Building* pBuilding)
{
    if (!pBuilding->amplifying.empty())
        amplifierRecalculateRequired = true;

    for (Amplifiable* pAmp : pBuilding->amplifying)
    {
        std::list<Amplifiable*>::iterator it =
            std::find(pAmp->amplifying.begin(), pAmp->amplifying.end(), pBuilding);

        if (it != pAmp->amplifying.end())
            pAmp->amplifying.erase(it);
    }
}

std::vector<Amplifier*> IngameBuildingController::getAdjacentAmplifiers(Amplifiable* pBuilding)
{
    std::vector<Amplifier*> adjacent;

    int ix = pBuilding->ix;
    int iy = pBuilding->iy;

    // TODO maybe optimize this.
    for (Amplifier& a : amplifiers)
    {
        if ((abs(a.ix - pBuilding->ix) <= g_game.ingameBuildingSize) &&
            (abs(a.iy - pBuilding->iy) <= g_game.ingameBuildingSize))
        {
            adjacent.push_back(&a);
        }
    }

    return adjacent;
}

std::vector<Amplifiable*> IngameBuildingController::getAdjacentAmplifiableBuildings(
    Building* pBuilding)
{
    std::vector<Amplifiable*> adjacent;

    int ix = pBuilding->ix;
    int iy = pBuilding->iy;

    // TODO maybe optimize this.
    for (Tower& t : towers)
    {
        if ((abs(t.ix - pBuilding->ix) <= g_game.ingameBuildingSize) &&
            (abs(t.iy - pBuilding->iy) <= g_game.ingameBuildingSize))
        {
            adjacent.push_back(&t);
        }
    }

    for (Trap& t : traps)
    {
        if ((abs(t.ix - pBuilding->ix) <= g_game.ingameBuildingSize) &&
            (abs(t.iy - pBuilding->iy) <= g_game.ingameBuildingSize))
        {
            adjacent.push_back(&t);
        }
    }

    if (g_game.game != GC_LABYRINTH)
    {
        if ((abs(orb.ix - pBuilding->ix) <= g_game.ingameBuildingSize) &&
            (abs(orb.iy - pBuilding->iy) <= g_game.ingameBuildingSize))
        {
            adjacent.push_back(&orb);
        }
    }

    return adjacent;
}
