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

    virtual STATUS render(struct _fbg* pFbg);

    virtual void handleMouseInput(GLFWwindow* pWindow, int button, int action, int mods);
    virtual void handleKeyInput(GLFWwindow* pWindow, int key, int scancode, int action, int mods);
};
