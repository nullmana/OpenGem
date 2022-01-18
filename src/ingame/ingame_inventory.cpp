#include "ingame/ingame_inventory.h"
#include "ingame/ingame_core.h"
#include "ingame/ingame_mana_pool.h"
#include "ingame/ingame_projectile_controller.h"

#include "constants/game_header.h"

#include "entity/building.h"
#include "entity/shrine.h"

#include "wrapfbg.h"

#include <algorithm>

#include <cstddef>

IngameInventory::IngameInventory(IngameManaPool& mp_, IngameProjectileController& pc_, int slots_, uint32_t availableGemTypes_)
    : manaPool(mp_), projectileController(pc_), inventory(slots_)
{
    pDraggedGem = NULL;
    unlockedGemTypeMask = availableGemTypes_;
}

STATUS IngameInventory::render(struct _fbg* pFbg, const Window& window) const
{
    struct _fbg_glfw_context* pGlfwContext = (struct _fbg_glfw_context*)pFbg->user_context;
    const IngameCore* pCore = (const IngameCore*)glfwGetWindowUserPointer(pGlfwContext->window);

    float scale = window.width / 3.0f;
    float gemScale = scale - 4.0f;

    INGAME_INPUT_STATE inputState = pCore->inputHandler.getInputState();
    bool highlightFirstGem = (inputState == INPUT_BOMB_MULTIPLE) || (inputState == INPUT_BOMB_TEMPLATE);

    for (int i = 0; i < inventory.size(); ++i)
    {
        if ((inventory[i] != NULL) && !inventory[i]->isDragged)
        {
            int ix = (i % 3) * scale + window.x;
            int iy = (i / 3) * scale + window.y;

            if (highlightFirstGem)
            {
                fbg_hline(pFbg, ix, iy, scale, 0xFF, 0xFF, 0xFF);
                fbg_hline(pFbg, ix, iy + scale, scale, 0xFF, 0xFF, 0xFF);
                fbg_vline(pFbg, ix, iy, scale, 0xFF, 0xFF, 0xFF);
                fbg_vline(pFbg, ix + scale, iy, scale, 0xFF, 0xFF, 0xFF);
                highlightFirstGem = false;
            }

            fbg_rect(pFbg, ix + 2, iy + 2, gemScale, gemScale,
                (inventory[i]->RGB >> 16) & 0xFF, (inventory[i]->RGB >> 8) & 0xFF, inventory[i]->RGB & 0xFF);
        }
    }

    if (g_game.game == GC_LABYRINTH)
    {
        if (inputState == INPUT_CREATE_GEM)
        {
            for (int i = 0; i < inventory.size() / 3; ++i)
            {
                if (manaPool.getMana() >= Gem::gemCreateCost(i))
                {
                    int ix = window.x - 0.925f * scale;
                    int iy = (11.075f - i) * scale + window.y;
                    fbg_rect(pFbg, ix, iy, gemScale, gemScale, 0xE0, 0xE0, 0xE0);
                    fbg_rect(pFbg, ix + 1, iy + 1, gemScale - 2, gemScale - 2, 0x10, 0x10, 0x10);
                }
                else
                    break;
            }
        }
    }
    else if (g_game.game == GC_CHASINGSHADOWS)
    {
        for (int i = 0; i < inventory.size() / 3; ++i)
        {
            if (manaPool.getMana() >= Gem::gemCreateCost(i))
                fbg_rect(pFbg, window.x + 3 * scale + 4, (11 - i) * scale + window.y + 1, 4, scale - 2, 0xE0, 0xE0, 0xE0);
            else
                break;
        }
    }

    if (pDraggedGem != NULL)
    {
        const Window* pRootWindow = window.getRootWindow();
        int ix = pDraggedGem->x - gemScale;
        int iy = pDraggedGem->y - gemScale;
        int iw = 2 * gemScale;
        int ih = 2 * gemScale;

        if (ix < 0)
        {
            iw += ix;
            ix = 0;
        }
        else if (ix + iw >= pRootWindow->width)
        {
            iw = pRootWindow->width - ix;
        }
        if (iy < 0)
        {
            ih += iy;
            iy = 0;
        }
        else if (iy + ih >= pRootWindow->height)
        {
            ih = pRootWindow->height - iy;
        }

        if ((iw > 0) && (ih > 0))
        {
            fbg_rect(pFbg, ix, iy, iw, ih, (pDraggedGem->RGB >> 16) & 0xFF,
                (pDraggedGem->RGB >> 8) & 0xFF, pDraggedGem->RGB & 0xFF);
        }
    }

    return STATUS_OK;
}

void IngameInventory::dropGemIntoInventory(Gem* pGem)
{
    int slot = -1;

    for (int s = 0; s < inventory.size(); ++s)
    {
        if (inventory[s] == NULL)
        {
            slot = s;
            break;
        }
    }

    if (slot != -1)
    {
        placeGemIntoInventory(pGem, slot, false);
    }
}

void IngameInventory::removeGemFromInventory(Gem* pGem)
{
    std::vector<Gem*>::iterator it = std::find(inventory.begin(), inventory.end(), pGem);

    if (it != inventory.end())
        *it = NULL;
}

void IngameInventory::placeGemIntoInventory(Gem* pGem, int slot, bool forceReplace)
{
    if (inventory[slot] == pGem)
        return;

    if ((inventory[slot] != NULL) && !forceReplace)
    {
        swapGems(pGem, inventory[slot]);
    }
    else
    {
        if (pGem->pBuilding != NULL)
            removeGemFromBuilding(pGem->pBuilding);
        else
            removeGemFromInventory(pGem);

        inventory[slot] = pGem;
        if (pGem->isDragged)
        {
            pGem->isDragged = false;
            pDraggedGem = NULL;
        }
    }
}

void IngameInventory::removeGemFromBuilding(Building* pBuilding)
{
    pBuilding->removeGem();
}

void IngameInventory::placeGemIntoBuilding(Gem* pGem, Building* pBuilding, bool forceReplace)
{
    if (pBuilding->pGem == pGem)
        return;

    if ((pBuilding->pGem != NULL) && !forceReplace)
    {
        swapGems(pGem, pBuilding->pGem);
    }
    else
    {
        if (pGem->isDragged)
        {
            pGem->isDragged = false;
            pDraggedGem = NULL;
        }

        switch (pBuilding->type)
        {
            case TILE_SHRINE_CB:
            case TILE_SHRINE_LI:
            {
                Shrine* pShrine = reinterpret_cast<Shrine*>(pBuilding);
                if (pShrine->canActivate())
                {
                    if (pGem->pBuilding != NULL)
                        removeGemFromBuilding(pGem->pBuilding);
                    else
                        removeGemFromInventory(pGem);

                    pShrine->activate(pGem);

                    deleteGem(pGem);
                }
                break;
            }
            default:
                if (pGem->pBuilding != NULL)
                    removeGemFromBuilding(pGem->pBuilding);
                else
                    removeGemFromInventory(pGem);

                if (forceReplace && (pBuilding->pGem != NULL))
                    pBuilding->pGem->pBuilding = NULL;

                pBuilding->insertGem(pGem);

                break;
        }
    }
}

void IngameInventory::swapGems(Gem* pGem1, Gem* pGem2)
{
    Building* pBuilding1 = pGem1->pBuilding;
    Building* pBuilding2 = pGem2->pBuilding;

    std::vector<Gem*>::iterator it1 = std::find(inventory.begin(), inventory.end(), pGem1);
    std::vector<Gem*>::iterator it2 = std::find(inventory.begin(), inventory.end(), pGem2);
    int slot1 = (it1 == inventory.end()) ? -1 : (it1 - inventory.begin());
    int slot2 = (it2 == inventory.end()) ? -1 : (it2 - inventory.begin());

    if (pBuilding1 != NULL)
    {
        placeGemIntoBuilding(pGem2, pBuilding1, true);

        if (pBuilding2 != NULL)
            placeGemIntoBuilding(pGem1, pBuilding2, true);
        else
            placeGemIntoInventory(pGem1, slot2, true);
    }
    else
    {
        if (pBuilding2 != NULL)
            placeGemIntoBuilding(pGem1, pBuilding2, true);
        else
            placeGemIntoInventory(pGem1, slot2, true);

        placeGemIntoInventory(pGem2, slot1, true);
    }
}

Gem* IngameInventory::combineGems(Gem* pGem1, Gem* pGem2)
{
    pGem1->combineWith(pGem2);
    pGem1->recalculateShotData();

    if (pGem1->pBuilding != NULL)
        pGem1->pBuilding->updateGem();

    if (pGem1 != pGem2)
        deleteGem(pGem2);

    return pGem1;
}

Gem* IngameInventory::duplicateGemIntoSlot(Gem* pGem, int slot)
{
    if (slot == -1)
    {
        for (int s = inventory.size() - 1; s >= 0; --s)
        {
            if (inventory[s] == NULL)
            {
                slot = s;
                break;
            }
        }
    }
    else if (inventory[slot] != NULL)
    {
        return NULL;
    }

    if (slot == -1)
        return NULL;

    gems.emplace_back(pGem);

    placeGemIntoInventory(&gems.back(), slot, true);

    return &gems.back();
}

void IngameInventory::salvageGem(Gem* pGem)
{
    manaPool.addMana(floor(0.7 * pGem->manaCost), false);

    deleteGem(pGem);
}

Gem* IngameInventory::getGemInSlot(int slot) const
{
    if ((slot < 0) || (slot >= inventory.size()))
        return NULL;

    return inventory[slot];
}

Gem* IngameInventory::getFirstGem() const
{
    for (int s = 0; s < inventory.size(); ++s)
    {
        if (inventory[s] != NULL)
            return inventory[s];
    }

    return NULL;
}

void IngameInventory::startDragGem(Gem* pGem)
{
    if (pDraggedGem != NULL)
        pDraggedGem->isDragged = false;

    pDraggedGem = pGem;
    pGem->isDragged = true;
}

void IngameInventory::clearDraggedGem()
{
    if (pDraggedGem != NULL)
    {
        pDraggedGem->isDragged = false;
        pDraggedGem = NULL;
    }
}

Gem* IngameInventory::createGem(GEM_COMPONENT_TYPE gemType, int grade)
{
    gems.emplace_back(grade, gemType);
    return &gems.back();
}

void IngameInventory::deleteGem(Gem* pGem)
{
    if (pGem->pBuilding != NULL)
        removeGemFromBuilding(pGem->pBuilding);
    else
        removeGemFromInventory(pGem);

    if (pGem == pDraggedGem)
        pDraggedGem = NULL;

    projectileController.clearShotsFromGem(pGem);

    for (std::list<Gem>::iterator it = gems.begin(); it != gems.end(); ++it)
    {
        if (&(*it) == pGem)
        {
            gems.erase(it);
            break;
        }
    }
}

bool IngameInventory::createGemInSlot(GEM_COMPONENT_TYPE gemType, int slot)
{
    int foundSlot = -1;
    int grade = (inventory.size() / 3) - (slot / 3) - 1;

    if (manaPool.getMana() < Gem::gemCreateCost(grade))
        return false;

    for (int s = slot; s < inventory.size(); ++s)
    {
        if (inventory[s] == NULL)
        {
            foundSlot = s;
            break;
        }
    }

    if (foundSlot == -1)
    {
        for (int s = slot; s >= 0; --s)
        {
            if (inventory[s] == NULL)
            {
                foundSlot = s;
                break;
            }
        }
    }

    if (foundSlot != -1)
    {
        inventory[foundSlot] = createGem(gemType, grade);
        placeGemIntoInventory(&gems.back(), foundSlot, true);
        manaPool.addMana(-gems.back().manaCost, false);
        return true;
    }
    return false;
}

bool IngameInventory::createAllGemsInSlot(GEM_COMPONENT_TYPE gemType, int slot)
{
    bool createdGems = false;
    bool status = true;

    while (status)
    {
        status = createGemInSlot(gemType, slot);
        if (!createdGems && status)
            createdGems = true;
    }

    return createdGems;
}

int IngameInventory::massCombineGems(int grade)
{
    std::vector<Gem*> combinePairs;

    for (int i = 0; i < inventory.size(); ++i)
    {
        if ((inventory[i] != NULL) && (inventory[i]->grade == grade))
            combinePairs.push_back(inventory[i]);
    }

    int numCombined = 0;
    for (int i = (combinePairs.size() % 2); i < combinePairs.size(); i += 2)
    {
        if (manaPool.getMana() >= Gem::gemCombineCostCurrent)
        {
            manaPool.addMana(-Gem::gemCombineCostCurrent, false);
            combineGems(combinePairs[i + 1], combinePairs[i]);
            ++numCombined;
        }
        else
            break;
    }

    return numCombined;
}

void IngameInventory::unlockGemType(GEM_COMPONENT_TYPE type)
{
    double unlockCost = Gem::gemUnlockCostGCL(getNumUnlockedGemTypes());
    if (manaPool.getMana() >= unlockCost)
    {
        unlockedGemTypeMask |= (1 << type);
        manaPool.addMana(-unlockCost, false);
    }
}
