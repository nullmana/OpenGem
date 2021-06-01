#include "interface/window_map.h"

#include "ingame/ingame_core.h"
#include "ingame/ingame_map.h"

#include "wrapfbg.h"

#include <cmath>
#include <cstdlib>

STATUS WindowMap::render(struct _fbg* pFbg)
{
    fbg_rect(pFbg, x - 2, y - 2, width + 4, height + 4, 0x22, 0x26, 0x31);

    return map.render(pFbg, *this);
}

void WindowMap::handleMouseInput(GLFWwindow* pWindow, int button, int action, int mods)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
    IngameInputHandler* pInputHandler = &pCore->inputHandler;
    double xpos, ypos;
    float scale = width / g_game.ingameMapWidth;

    glfwGetCursorPos(pWindow, &xpos, &ypos);

    int ix = std::floor((xpos - x) / scale);
    int iy = std::floor((ypos - y) / scale);

    if ((ix < 0) || (ix >= g_game.ingameMapWidth) || (iy < 0) || (iy >= g_game.ingameMapHeight))
        return;

    switch (pInputHandler->getInputState())
    {
        case INPUT_IDLE:
            if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                Building* pBuilding = map.getBuilding(iy, ix);
                if (pBuilding != NULL)
                {
                    Gem* pGem = pBuilding->pGem;
                    if (pGem != NULL)
                    {
                        pCore->inventory.startDragGem(pGem);
                        pGem->x = xpos;
                        pGem->y = ypos;
                        pInputHandler->setInputState(INPUT_DRAGGING_IDLE);
                    }
                }
            }
            break;
        case INPUT_COMBINE_GEM:
            if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                Building* pBuilding = map.getBuilding(iy, ix);
                if (pBuilding != NULL)
                {
                    Gem* pGem = pBuilding->pGem;
                    if (pGem != NULL)
                    {
                        pCore->inventory.startDragGem(pGem);
                        pGem->x = xpos;
                        pGem->y = ypos;
                        pInputHandler->setInputState(INPUT_DRAGGING_COMBINE);
                    }
                }
            }
            break;
        case INPUT_BOMB_GEM:
            if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                Building* pBuilding = map.getBuilding(iy, ix);
                if (pBuilding != NULL)
                {
                    Gem* pGem = pBuilding->pGem;
                    if (pGem != NULL)
                    {
                        pCore->inventory.startDragGem(pGem);
                        pGem->x = xpos;
                        pGem->y = ypos;
                        pInputHandler->setInputState(INPUT_DRAGGING_BOMB);
                    }
                }
            }
            break;
        case INPUT_BUILD_WALL:
            if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                dragStartX = ix;
                dragStartY = iy;
                pInputHandler->setInputState(INPUT_BUILD_WALL_DRAGGING);
            }
            break;
        case INPUT_BUILD_WALL_DRAGGING:
            if ((action == GLFW_RELEASE) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                pInputHandler->setInputState(INPUT_BUILD_WALL_DRAGGING);
                if ((ix == dragStartX) || (iy == dragStartY))
                    map.buildWall(dragStartX, dragStartY, ix, iy);
                pInputHandler->setInputState(
                    (mods & GLFW_MOD_SHIFT) ? INPUT_BUILD_WALL : INPUT_IDLE);
            }
            break;
        case INPUT_BUILD_TOWER:
            if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                map.buildTower(ix, iy);
                if (!(mods & GLFW_MOD_SHIFT))
                    pInputHandler->setInputState(INPUT_IDLE);
            }
            break;
        case INPUT_BUILD_TRAP:
            if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                map.buildTrap(ix, iy);
                if (!(mods & GLFW_MOD_SHIFT))
                    pInputHandler->setInputState(INPUT_IDLE);
            }
            break;
        case INPUT_BUILD_AMPLIFIER:
            if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                map.buildAmplifier(ix, iy);
                if (!(mods & GLFW_MOD_SHIFT))
                    pInputHandler->setInputState(INPUT_IDLE);
            }
            break;
        case INPUT_DRAGGING_IDLE:
            if ((action == GLFW_RELEASE) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                Building* pBuilding = map.getBuilding(iy, ix);
                Gem* pDraggedGem = pCore->inventory.getDraggedGem();
                if (pDraggedGem != NULL)
                {
                    if (pBuilding != NULL)
                    {
                        pCore->inventory.placeGemIntoBuilding(pDraggedGem, pBuilding, false);
                    }
                    pCore->inventory.clearDraggedGem();
                }
                pInputHandler->setInputState(INPUT_IDLE);
            }
            break;
        case INPUT_DRAGGING_COMBINE:
            if ((action == GLFW_RELEASE) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                Building* pBuilding = map.getBuilding(iy, ix);
                Gem* pDraggedGem = pCore->inventory.getDraggedGem();
                if (pDraggedGem != NULL)
                {
                    if ((pBuilding != NULL) && (pBuilding->pGem != NULL))
                    {
                        if (pCore->manaPool.getMana() >= Gem::gemCombineCostCurrent)
                        {
                            pCore->inventory.combineGems(pBuilding->pGem, pDraggedGem);
                            pCore->manaPool.addMana(-Gem::gemCombineCostCurrent, false);
                        }
                    }
                    pCore->inventory.clearDraggedGem();
                }
                pInputHandler->setInputState(INPUT_IDLE);
            }
            break;
        case INPUT_DRAGGING_BOMB:
            if ((action == GLFW_RELEASE) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                Gem* pDraggedGem = pCore->inventory.getDraggedGem();
                if (pDraggedGem != NULL)
                {
                    Building* pBuilding = map.getBuilding(iy, ix);
                    if ((pBuilding != NULL) && (pBuilding->pGem == NULL))
                        map.demolishBuilding(ix, iy);

                    map.dropGemBomb(pDraggedGem, xpos - x, ypos - y);

                    pCore->inventory.deleteGem(pDraggedGem);
                }
                pInputHandler->setInputState(INPUT_IDLE);
            }
            break;
        case INPUT_BOMB_MULTIPLE:
        case INPUT_BOMB_TEMPLATE:
            if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                INGAME_INPUT_STATE state = pInputHandler->getInputState();
                Gem* pBombGem = pCore->inventory.getFirstGem();
                bool bClearState = true;
                if (pBombGem != NULL)
                {
                    if ((state == INPUT_BOMB_MULTIPLE) ||
                        (pCore->manaPool.getMana() >= pBombGem->manaCost))
                    {
                        Building* pBuilding = map.getBuilding(iy, ix);
                        if ((pBuilding != NULL) && (pBuilding->pGem == NULL))
                            map.demolishBuilding(ix, iy);

                        map.dropGemBomb(pBombGem, xpos - x, ypos - y);

                        if (state == INPUT_BOMB_TEMPLATE)
                        {
                            pCore->manaPool.addMana(-pBombGem->manaCost, false);
                            bClearState = false;
                        }
                        else
                        {
                            pCore->inventory.deleteGem(pBombGem);
                            if (pCore->inventory.getFirstGem() != NULL)
                                bClearState = false;
                        }
                    }
                }
                if (bClearState)
                    pInputHandler->setInputState(INPUT_IDLE);
            }
            break;
    }
}

void WindowMap::handleKeyInput(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
    double xpos, ypos;
    float scale = width / g_game.ingameMapWidth;

    glfwGetCursorPos(pWindow, &xpos, &ypos);

    int ix = std::floor((xpos - x) / scale);
    int iy = std::floor((ypos - y) / scale);

    if ((ix < 0) || (ix >= g_game.ingameMapWidth) || (iy < 0) || (iy >= g_game.ingameMapHeight))
        return;

    Building* pBuilding = map.getBuilding(iy, ix);
    if ((pBuilding == NULL) || (pBuilding->pGem == NULL) ||
        (pCore->inventory.getDraggedGem() != NULL))
        return;

    Gem* pGem = pBuilding->pGem;

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_D:
                if (pCore->manaPool.getMana() >= pGem->manaCost)
                {
                    if (NULL != pCore->inventory.duplicateGemIntoSlot(pGem, -1))
                        pCore->manaPool.addMana(-pGem->manaCost, false);
                }
                break;
            case GLFW_KEY_U:
                if (pCore->manaPool.getMana() >= (pGem->manaCost + Gem::gemCombineCostCurrent))
                {
                    pCore->inventory.combineGems(pGem, pGem);
                    pCore->manaPool.addMana(-(pGem->manaCost + Gem::gemCombineCostCurrent), false);
                }
                break;
            case GLFW_KEY_X:
                pCore->inventory.salvageGem(pGem);
                break;
        }
    }
}
