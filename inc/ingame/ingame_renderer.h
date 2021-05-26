#pragma once

#include "constants/status.h"

#include "ingame/ingame_input_handler.h"

#include "interface/window.h"
#include "interface/window_map.h"
#include "interface/window_inventory.h"

class IngameCore;
struct _fbg;

class IngameRenderer
{
private:
	Window rootWindow;
	WindowMap windowMap;
	WindowInventory windowInventory;
public:
	IngameRenderer(IngameCore &core);

	STATUS init(IngameCore &core);

	STATUS render(IngameCore &core);

	void resize(int width, int height);

	Window * getRootWindow() { return &windowMap; }
};
