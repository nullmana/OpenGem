#pragma once

#include "entity/tower.h"
#include "entity/trap.h"

#include "interface/window.h"

#include <list>

class IngameBuildingController
{
private:
    std::list<Tower> towers;
    std::list<Trap> traps;

    void destroyTower(Tower* pTower);
    void destroyTrap(Trap* pTrap);

public:
    IngameBuildingController();

    void render(struct _fbg* pFbg, const Window& window) const;

    void tickBuildings(IngameMap& map, int frames);

    Tower& addTower(int x, int y);
    Trap& addTrap(int x, int y);

    void destroyBuilding(Building* pBuilding);
};
