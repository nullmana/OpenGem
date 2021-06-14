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
    wallCostCurrent = 20.0;
    towerCostCurrent = 150.0;
    trapCostCurrent = 150.0;
    amplifierCostCurrent = 400.0;
    shrineCostCurrent = 900.0;
}

#ifdef DEBUG
#include "ingame/ingame_core.h"

static void debugDrawAmplifierDirections(struct _fbg* pFbg, const Window& window, const Building* pBuilding)
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
                (pBuilding->type == TILE_AMPLIFIER) ? 0xFF : 0x00);
        }
    }
}
#endif

void IngameBuildingController::render(struct _fbg* pFbg, const Window& window) const
{
    float scale = window.width / float(g_game.ingameMapWidth);
    float cooldownScale = 0.92f * scale * g_game.ingameBuildingSize;
    float gemScale = 0.85f * scale * g_game.ingameBuildingSize;

    for (const Tower& t : towers)
    {
        if ((t.pGem != NULL) && !t.pGem->isDragged)
        {
            fbg_rect(pFbg, t.x * scale + window.x - 0.5f * gemScale, t.y * scale + window.y - 0.5f * gemScale,
                gemScale, gemScale, (t.pGem->RGB >> 16) & 0xFF, (t.pGem->RGB >> 8) & 0xFF, t.pGem->RGB & 0xFF);

            if (t.isCoolingDown())
            {
                float cooldown = t.getCooldown();
                fbg_recta(pFbg, t.x * scale + window.x - 0.5f * cooldownScale,
                    t.y * scale + window.y + (0.5f - cooldown) * cooldownScale,
                    cooldownScale, cooldownScale * cooldown, 0x20, 0x20, 0x20, 0x20);
            }
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
                (t.pGem->RGB >> 16) & 0xFF, (t.pGem->RGB >> 8) & 0xFF, t.pGem->RGB & 0xFF);

            if (t.isCoolingDown())
            {
                float cooldown = t.getCooldown();
                fbg_recta(pFbg, t.x * scale + window.x - 0.5f * cooldownScale,
                    t.y * scale + window.y + (0.5f - cooldown) * cooldownScale,
                    cooldownScale, cooldownScale * cooldown, 0x20, 0x20, 0x20, 0x20);
            }
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
                (t.pGem->RGB >> 16) & 0xFF, (t.pGem->RGB >> 8) & 0xFF, t.pGem->RGB & 0xFF);

            if (t.isCoolingDown())
            {
                float cooldown = t.getCooldown();
                fbg_recta(pFbg, t.x * scale + window.x - 0.5f * cooldownScale,
                    t.y * scale + window.y + (0.5f - cooldown) * cooldownScale,
                    cooldownScale, cooldownScale * cooldown, 0x20, 0x20, 0x20, 0x20);
            }
        }
#ifdef DEBUG
        debugDrawAmplifierDirections(pFbg, window, &t);
#endif
    }

    if (orb.isBroken())
    {
        fbg_rect(pFbg, (orb.nodeX - 0.5f * g_game.ingameBuildingSize) * scale + window.x,
            (orb.nodeY - 0.5f * g_game.ingameBuildingSize) * scale + window.y,
            scale * g_game.ingameBuildingSize, scale * g_game.ingameBuildingSize, 0x08, 0x08, 0x20);
    }
}

void IngameBuildingController::tickBuildings(IngameMap& map, int frames)
{
    for (Tower& t : towers)
    {
        if (t.isCoolingDown())
            t.tickCooldown(frames);
        if (!t.isCoolingDown())
            t.tick(map, frames);
    }

    for (Trap& t : traps)
    {
        if (t.isCoolingDown())
            t.tickCooldown(frames);
        if (!t.isCoolingDown())
            t.tick(map, frames);
    }

    for (Amplifier& a : amplifiers)
    {
        if (a.isCoolingDown())
            a.tickCooldown(frames);
    }
}

bool IngameBuildingController::hasBuildMana(const IngameManaPool& manaPool, TILE_TYPE building, int num) const
{
    switch (building)
    {
        case TILE_WALL:
        {
            double cost = wallCostCurrent;
            double sumCost = 0.0;
            for (int i = 0; i < num; ++i)
            {
                sumCost += std::max(0.0, cost);
                cost += 1.0;
            }
            return manaPool.getMana() >= sumCost;
            break;
        }
        case TILE_TOWER:
            return manaPool.getMana() >= towerCostCurrent;
        case TILE_TRAP:
            return manaPool.getMana() >= trapCostCurrent;
        case TILE_AMPLIFIER:
            return manaPool.getMana() >= amplifierCostCurrent;
        case TILE_SHRINE_CB:
        case TILE_SHRINE_LI:
            return manaPool.getMana() >= shrineCostCurrent;
        default:
            return false;
    }
}

void IngameBuildingController::spendBuildMana(IngameManaPool& manaPool, TILE_TYPE building, int num)
{
    switch (building)
    {
        case TILE_WALL:
        {
            double sumCost = 0.0;
            for (int i = 0; i < num; ++i)
            {
                sumCost += std::max(0.0, wallCostCurrent);
                wallCostCurrent += 1.0;
            }
            manaPool.addMana(-sumCost, false);
            break;
        }
        case TILE_TOWER:
            manaPool.addMana(-towerCostCurrent, false);
            towerCostCurrent += 30.0;
            break;
        case TILE_TRAP:
            manaPool.addMana(-trapCostCurrent, false);
            trapCostCurrent += 24.0;
            break;
        case TILE_AMPLIFIER:
            manaPool.addMana(-amplifierCostCurrent, false);
            amplifierCostCurrent += 240.0;
            break;
        case TILE_SHRINE_CB:
        case TILE_SHRINE_LI:
            manaPool.addMana(-shrineCostCurrent, false);
            shrineCostCurrent *= 2.35;
            break;
    }
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

Shrine* IngameBuildingController::addShrine(IngameMap& map, int x, int y, SHRINE_TYPE type)
{
    Shrine* pShrine = NULL;

    switch (type)
    {
        case SHRINE_CHARGED_BOLTS:
            shrinesCB.emplace_back(map, x, y);
            pShrine = &shrinesCB.back();
            break;
        case SHRINE_LIGHTNING:
            shrinesLI.emplace_back(map, x, y);
            pShrine = &shrinesLI.back();
            break;
    }

    return pShrine;
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
        case TILE_TOWER:
            destroyTower(reinterpret_cast<Tower*>(pBuilding));
            break;
        case TILE_TRAP:
            destroyTrap(reinterpret_cast<Trap*>(pBuilding));
            break;
        case TILE_AMPLIFIER:
            destroyAmplifier(reinterpret_cast<Amplifier*>(pBuilding));
            break;
    }
}

void IngameBuildingController::linkAmplifier(Amplifier* pAmplifier, Amplifiable* pAmplified)
{
    pAmplifier->amplifying.push_back(pAmplified);
    pAmplified->amplifying.push_back(pAmplifier);
}

void IngameBuildingController::removeBuildingAmplifiers(Building* pBuilding)
{
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

std::vector<Amplifiable*> IngameBuildingController::getAdjacentAmplifiableBuildings(Building* pBuilding)
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
