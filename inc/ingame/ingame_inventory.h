#pragma once

#include "entity/gem.h"

#include "interface/window.h"

#include <vector>

class IngameInventory
{
private:
	std::vector<bool> occupied;
	std::vector<Gem> inventory;
	Gem *pDraggedGem;
public:
	IngameInventory (int slots);

	STATUS render(struct _fbg *pFbg, const Window &window)const;
};
