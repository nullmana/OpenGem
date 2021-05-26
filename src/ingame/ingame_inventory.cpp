#include "ingame/ingame_inventory.h"

#include "glfw/fbg_glfw.h"

#include <cstddef>

IngameInventory::IngameInventory (int slots) : occupied(slots), inventory(slots)
{
	pDraggedGem = NULL;
	for (int i = 0; i < slots; ++i)
		if (i % 2)
			occupied[i] = true;
}

STATUS IngameInventory::render(struct _fbg *pFbg, const Window &window)const
{
	float scale = window.width / 3.0f;

	for (int i = 0; i < occupied.size(); ++i)
	{
		if (occupied[i])
		{
			fbg_rect(pFbg, (i%3)*scale+window.x, (i/3)*scale+window.y, scale, scale, 0xFF, 0x00, 0x00);
		}
	}

	return STATUS_OK;
}
