#include "ingame/ingame_inventory.h"
#include "ingame/ingame_mana_pool.h"
#include "ingame/ingame_projectile_controller.h"

#include "entity/building.h"
#include "entity/shrine.h"

#include "wrapfbg.h"

#include <algorithm>

#include <cstddef>

IngameInventory::IngameInventory(IngameManaPool& mp_, IngameProjectileController& pc_, int slots_)
    : manaPool(mp_), projectileController(pc_), inventory(slots_)
{
    pDraggedGem = NULL;
}

STATUS IngameInventory::render(struct _fbg* pFbg, const Window& window) const
{
    float scale = window.width / 3.0f;

    for (int i = 0; i < inventory.size(); ++i)
    {
        if ((inventory[i] != NULL) && !inventory[i]->isDragged)
        {
            fbg_rect(pFbg, (i % 3) * scale + window.x, (i / 3) * scale + window.y, scale, scale,
                (inventory[i]->RGB >> 16) & 0xFF, (inventory[i]->RGB >> 8) & 0xFF,
                inventory[i]->RGB & 0xFF);
        }
    }

    if (pDraggedGem != NULL)
    {
        const Window* pRootWindow = window.getRootWindow();
        int ix = pDraggedGem->x - scale;
        int iy = pDraggedGem->y - scale;
        int iw = 2 * scale;
        int ih = 2 * scale;

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
