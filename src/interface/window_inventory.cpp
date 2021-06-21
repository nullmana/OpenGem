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

    if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_RIGHT))
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
                    pGem->x = (slot % 3 + 0.5f) * width / 3.0f + x;
                    pGem->y = (slot / 3 + 0.5f) * width / 3.0f + y;
                    pCore->renderer.openTargetSelect(pGem);
                    pInputHandler->setInputState(INPUT_IDLE);
                }
                break;
            }
        }
    }
    else if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
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
                    if (!!(mods & GLFW_MOD_SHIFT))
                    {
                        pGem->x = (slot % 3 + 0.5f) * width / 3.0f + x;
                        pGem->y = (slot / 3 + 0.5f) * width / 3.0f + y;
                        pCore->renderer.openTargetSelect(pGem);
                        pInputHandler->setInputState(INPUT_IDLE);
                    }
                    else
                    {
                        inventory.startDragGem(pGem);
                        pGem->x = xpos;
                        pGem->y = ypos;
                        pInputHandler->setInputState(INPUT_DRAGGING_IDLE);
                    }
                }
                break;
            }
            case INPUT_COMBINE_GEM:
            {
                Gem* pGem = inventory.getGemInSlot(slot);
                if (pGem != NULL)
                {
                    if (!!(mods & GLFW_MOD_CONTROL))
                    {
                        inventory.massCombineGems(pGem->grade);

                        if (!(mods & GLFW_MOD_SHIFT))
                            pInputHandler->setInputState(INPUT_IDLE);
                    }
                    else
                    {
                        inventory.startDragGem(pGem);
                        pGem->x = xpos;
                        pGem->y = ypos;
                        pInputHandler->setInputState(INPUT_DRAGGING_COMBINE);
                    }
                }
                break;
            }
            case INPUT_BOMB_GEM:
            {
                Gem* pGem = inventory.getGemInSlot(slot);
                if (pGem != NULL)
                {
                    inventory.startDragGem(pGem);
                    pGem->x = xpos;
                    pGem->y = ypos;
                    pInputHandler->setInputState(INPUT_DRAGGING_BOMB);
                }
                break;
            }
            case INPUT_CREATE_GEM:
            {
                GEM_COMPONENT_TYPE gemType = pInputHandler->getCreatingGemType();
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
                break;
            }
            case INPUT_DRAGGING_COMBINE:
            {
                Gem* pDraggedGem = inventory.getDraggedGem();
                Gem* pTargetGem = inventory.getGemInSlot(slot);
                if (pDraggedGem != NULL)
                {
                    if (pTargetGem == NULL)
                    {
                        if (pCore->manaPool.getMana() >= pDraggedGem->manaCost)
                        {
                            inventory.duplicateGemIntoSlot(pDraggedGem, slot);
                            pCore->manaPool.addMana(-pDraggedGem->manaCost, false);
                        }
                        inventory.clearDraggedGem();
                    }
                    else
                    {
                        if (pCore->manaPool.getMana() >= Gem::gemCombineCostCurrent)
                        {
                            inventory.combineGems(pTargetGem, pDraggedGem);
                            pCore->manaPool.addMana(-Gem::gemCombineCostCurrent, false);
                        }
                    }
                }
                if (!!(mods & GLFW_MOD_SHIFT))
                    pInputHandler->setInputState(INPUT_COMBINE_GEM);
                else
                    pInputHandler->setInputState(INPUT_IDLE);
                break;
            }
        }
    }
#ifdef DEBUG
    else if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_MIDDLE))
    {
        if (pInputHandler->getInputState() == INPUT_IDLE)
        {
            Gem* pGem = inventory.getGemInSlot(slot);
            if (pGem != NULL)
                pGem->debugPrint();
        }
    }
#endif
}

void WindowInventory::handleKeyInput(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
    double xpos, ypos;

    glfwGetCursorPos(pWindow, &xpos, &ypos);

    int slot = getInventorySlot(xpos, ypos);
    Gem* pGem = inventory.getGemInSlot(slot);

    if ((pGem == NULL) || (inventory.getDraggedGem() != NULL))
        return;

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_D:
                if (pCore->manaPool.getMana() >= pGem->manaCost)
                {
                    if (NULL != inventory.duplicateGemIntoSlot(pGem, -1))
                        pCore->manaPool.addMana(-pGem->manaCost, false);
                }
                break;
            case GLFW_KEY_U:
                if (pCore->manaPool.getMana() >= (pGem->manaCost + Gem::gemCombineCostCurrent))
                {
                    pCore->manaPool.addMana(-(pGem->manaCost + Gem::gemCombineCostCurrent), false);
                    inventory.combineGems(pGem, pGem);
                }
                break;
            case GLFW_KEY_X:
                inventory.salvageGem(pGem);
                break;
            default:
                // Do below only if performing a valid action
                return;
        }

        if ((pCore->inputHandler.getInputState() == INPUT_BOMB_MULTIPLE) ||
            (pCore->inputHandler.getInputState() == INPUT_BOMB_TEMPLATE))
        {
            pCore->inputHandler.setInputState(INPUT_IDLE);
        }
    }
}
