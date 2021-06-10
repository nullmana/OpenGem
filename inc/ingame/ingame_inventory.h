#pragma once

#include "entity/gem.h"

#include "interface/window.h"

#include <list>
#include <vector>

class Building;
class IngameManaPool;

class IngameInventory
{
private:
    IngameManaPool& manaPool;
    std::vector<Gem*> inventory;
    std::list<Gem> gems;
    Gem* pDraggedGem;

    void checkAdjacentAmplifiers(Gem* pGem);

    Gem* createGem(GEM_COMPONENT_TYPE gemType, int grade);

public:
    IngameInventory(IngameManaPool& manaPool_, int slots_);

    STATUS render(struct _fbg* pFbg, const Window& window) const;

    void removeGemFromInventory(Gem* pGem);
    void placeGemIntoInventory(Gem* pGem, int slot, bool forceReplace);
    void removeGemFromBuilding(Building* pBuilding);
    void placeGemIntoBuilding(Gem* pGem, Building* pBuilding, bool forceReplace);

    void swapGems(Gem* pGem1, Gem* pGem2);

    /*!
     * @brief Combine `pGem2` into `pGem1`.
     * @param[in] pGem1 base gem being combined into
     * @param[in] pGem2 second gem being consumed by the combination
     * @return Pointer to combined gem
     */
    Gem* combineGems(Gem* pGem1, Gem* pGem2);

    /*!
     * @brief Duplicate `pGem` into `slot` if provided, or last slot if `slot` is -1
     * @param[in] pGem base gem to be duplicated
     * @param[in] slot Inventory slot to place result into, or -1 to pick last open slot
     * @return Pointer to combined gem
     */
    Gem* duplicateGemIntoSlot(Gem* pGem, int slot);

    void deleteGem(Gem* pGem);
    void salvageGem(Gem* pGem);

    Gem* getGemInSlot(int slot) const;
    Gem* getFirstGem() const;

    void startDragGem(Gem* pGem);
    void clearDraggedGem();
    Gem* getDraggedGem() const { return pDraggedGem; }

    bool createGemInSlot(GEM_COMPONENT_TYPE gemType, int slot);
    bool createAllGemsInSlot(GEM_COMPONENT_TYPE gemType, int slot);
};
