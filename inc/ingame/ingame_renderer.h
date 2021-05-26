#pragma once

#include "constants/status.h"

#include "ingame/ingame_input_handler.h"
#include "interface/window.h"
#include "interface/window_map.h"

class IngameCore;
struct _fbg;

class IngameRenderer
{
private:
    // TODO make root window with windowMap as child
    WindowMap windowMap;

public:
    IngameRenderer(IngameCore& core);

    STATUS init(IngameCore& core);

    STATUS render(IngameCore& core);

    Window* getRootWindow() { return &windowMap; }
};
