#pragma once

#include "entity/gem.h"

#include "interface/window.h"

#include <list>
#include <vector>

class Building;

class IngameInventory
{
private:
    std::vector<Gem*> inventory;
    std::list<Gem> gems;
    Gem* pDraggedGem;

    void checkAdjacentAmplifiers(Gem* pGem);

public:
    IngameInventory(int slots_);

    STATUS render(struct _fbg* pFbg, const Window& window) const;

    void removeGemFromInventory(Gem* pGem);
    void placeGemIntoInventory(Gem* pGem, int slot, bool forceReplace);
    Gem* removeGemFromBuilding(Building* pBuilding);
    void placeGemIntoBuilding(Gem* pGem, Building* pBuilding, bool forceReplace);

    void swapGems(Gem* pGem1, Gem* pGem2);

    Gem* getGemInSlot(int slot) const;

    void startDragGem(Gem* pGem);
    void clearDraggedGem();
    Gem* getDraggedGem() const { return pDraggedGem; }
};
