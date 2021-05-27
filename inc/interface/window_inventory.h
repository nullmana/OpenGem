#pragma once

#include "interface/window.h"

class IngameInventory;

class WindowInventory : public Window
{
private:
    IngameInventory& inventory;

    int getInventorySlot(float xpos, float ypos) const;

public:
    WindowInventory(IngameInventory& inventory_) : inventory(inventory_) {}

    virtual STATUS render(struct GraphicsContext* pFbg);
    virtual void handleMouseInput(struct GLFWwindow* pWindow, int button, int action, int mods);
};
