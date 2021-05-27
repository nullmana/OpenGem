#pragma once

#include "interface/window.h"

class IngameMap;

class WindowMap : public Window
{
private:
    IngameMap& map;

    int dragStartX;
    int dragStartY;

public:
    WindowMap(IngameMap& map_) : map(map_)
    {
        dragStartX = 0;
        dragStartY = 0;
    }

    virtual STATUS render(GraphicsContext* pContext);
    virtual void handleMouseInput(GraphicsContext* pContext, int button, int action, int mods);
};
