#include "interface/window_inventory.h"

#include "ingame/ingame_core.h"
#include "ingame/ingame_inventory.h"

#include "wrapfbg.h"

STATUS WindowInventory::render(struct _fbg* pFbg)
{
    fbg_rect(pFbg, x - 2, y - 2, width + 4, height + 4, 0x1D, 0x22, 0x2E);

    return inventory.render(pFbg, *this);
}

int WindowInventory::getInventorySlot(float xpos, float ypos) const
{
    float relX = (xpos - x) / width;
    float relY = (ypos - y) / height;

    return 3 * int(relY * 12) + int(relX * 3);
}

void WindowInventory::handleMouseInput(GLFWwindow* pWindow, int button, int action, int mods)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
    IngameInputHandler* pInputHandler = &pCore->inputHandler;
    double xpos, ypos;

    glfwGetCursorPos(pWindow, &xpos, &ypos);

    int slot = getInventorySlot(xpos, ypos);

    if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
    {
        switch (pInputHandler->getInputState())
        {
            case INPUT_IDLE:
            case INPUT_BUILD_WALL:
            case INPUT_BUILD_TOWER:
            case INPUT_BUILD_TRAP:
            {
                Gem* pGem = inventory.getGemInSlot(slot);
                if (pGem != NULL)
                {
                    inventory.startDragGem(pGem);
                    pGem->x = xpos;
                    pGem->y = ypos;
                }
                pInputHandler->setInputState(INPUT_DRAGGING_IDLE);
                break;
            }
            case INPUT_CREATE_GEM:
            {
                int gemType = pInputHandler->getCreatingGemType();
                if (gemType != -1)
                {
                    if ((mods & GLFW_MOD_CONTROL))
                        inventory.createAllGemsInSlot(gemType, slot);
                    else
                        inventory.createGemInSlot(gemType, slot);
                }
                if (!(mods & GLFW_MOD_SHIFT))
                    pInputHandler->setInputState(INPUT_IDLE);
                break;
            }
        }
    }
    else if ((action == GLFW_RELEASE) && (button == GLFW_MOUSE_BUTTON_LEFT))
    {
        switch (pInputHandler->getInputState())
        {
            case INPUT_DRAGGING_IDLE:
            {
                Gem* pDraggedGem = inventory.getDraggedGem();
                if (pDraggedGem != NULL)
                {
                    inventory.placeGemIntoInventory(pDraggedGem, slot, false);
                }
                pInputHandler->setInputState(INPUT_IDLE);
            }
        }
    }
}
