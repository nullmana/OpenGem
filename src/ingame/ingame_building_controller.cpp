#include "ingame/ingame_building_controller.h"

#include <cstdlib>

IngameBuildingController::IngameBuildingController() {}

void IngameBuildingController::tickBuildings(IngameMap &map, int frames)
{
	for (Tower &t : towers)
		t.tick(map, frames);

	for (Trap &t : traps)
		t.tick(map, frames);
}

Tower & IngameBuildingController::addTower(int x, int y)
{
	towers.emplace_back(x, y);
	return towers.back();
}

Trap & IngameBuildingController::addTrap(int x, int y)
{
	traps.emplace_back(x, y);
	return traps.back();
}

void IngameBuildingController::destroyTower(Tower *pTower)
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

void IngameBuildingController::destroyTrap(Trap *pTrap)
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

void IngameBuildingController::destroyBuilding(Building *pBuilding)
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
