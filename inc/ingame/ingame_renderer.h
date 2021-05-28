#pragma once

#include "constants/status.h"

#include "ingame/ingame_input_handler.h"

#include "interface/window.h"
#include "interface/window_button.h"
#include "interface/window_inventory.h"
#include "interface/window_map.h"

class IngameCore;
struct _fbg;

class IngameRenderer
{
private:
    Window rootWindow;
    WindowMap windowMap;
    WindowInventory windowInventory;
    WindowButton windowBuildSpells;
    WindowButton windowCreateGems;

public:
    IngameRenderer(IngameCore& core);

    STATUS init(IngameCore& core);

    STATUS render(IngameCore& core);

    void resize(int width, int height);

    Window* getRootWindow() { return &rootWindow; }

    void setBuildButtonActive(int button, bool active)
    {
        windowBuildSpells.setButtonActive(button, active);
    }
    void setGemButtonActive(int button, bool active)
    {
        windowCreateGems.setButtonActive(button, active);
    }

#ifdef DEBUG
    WindowMap* getMapWindow() { return &windowMap; }
#endif
};
