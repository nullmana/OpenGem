#pragma once

#include "interface/window.h"

class IngameInventory;

class WindowInventory : public Window
{
private:
    IngameInventory& inventory;

public:
    WindowInventory(IngameInventory& inventory_) : inventory(inventory_) {}

    virtual STATUS render(struct _fbg* pFbg);
    virtual void handleMouseInput(GLFWwindow* pWindow, int button, int action, int mods);
};
