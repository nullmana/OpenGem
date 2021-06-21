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

    const float xrel = (xpos - x) / scale;
    const float yrel = (ypos - y) / scale;

    int ix = std::floor(xrel);
    int iy = std::floor(yrel);

    if ((ix < 0) || (ix >= g_game.ingameMapWidth) || (iy < 0) || (iy >= g_game.ingameMapHeight))
        return;

    switch (pInputHandler->getInputState())
    {
        case INPUT_IDLE:
            if (action == GLFW_PRESS)
            {
                bool left = (button == GLFW_MOUSE_BUTTON_LEFT);
                bool right = (button == GLFW_MOUSE_BUTTON_RIGHT);
                bool shouldCheckTarget = true;

                Building* pBuilding = map.getBuilding(iy, ix);
                Structure* pStructure = map.getStructure(iy, ix);

                if (!(mods & GLFW_MOD_CONTROL))
                {
#ifdef DEBUG
                    if (button == GLFW_MOUSE_BUTTON_MIDDLE)
                    {
                        if (pBuilding != NULL)
                        {
                            Gem* pGem = pBuilding->pGem;
                            if (pGem != NULL)
                                pGem->debugPrint();
                            break;
                        }
                        else if (pStructure != NULL)
                        {
                            pStructure->debugPrint();
                            break;
                        }
                    }
                    else
#endif
                        if (pBuilding != NULL)
                    {
                        Gem* pGem = pBuilding->pGem;
                        if (pGem != NULL)
                        {
                            if (right || !!(mods & GLFW_MOD_SHIFT))
                            {
                                pGem->x = scale * pBuilding->x + x;
                                pGem->y = scale * pBuilding->y + y;
                                pCore->renderer.openTargetSelect(pGem);
                            }
                            else if (left)
                            {
                                pCore->inventory.startDragGem(pGem);
                                pGem->x = xpos;
                                pGem->y = ypos;
                                pInputHandler->setInputState(INPUT_DRAGGING_IDLE);
                            }
                            shouldCheckTarget = false;
                        }
                        else if (left)
                        {
                            pGem = pCore->inventory.getFirstGem();
                            if (pGem != NULL)
                            {
                                pCore->inventory.placeGemIntoBuilding(pGem, pBuilding, false);
                                shouldCheckTarget = false;
                            }
                        }
                    }
                    else if ((pStructure != NULL) && (!pStructure->isIndestructible))
                    {
                        map.setSelectedTarget(pStructure);
                        shouldCheckTarget = false;
                    }
                }

                if (shouldCheckTarget)
                {
                    Targetable* pTarget = map.enemyController.getTargetAtPosition(yrel, xrel);
                    if (pTarget != NULL)
                    {
                        if (!left)
                        {
#ifdef DEBUG
                            if (button == GLFW_MOUSE_BUTTON_MIDDLE)
                                pTarget->debugPrint();
#endif
                            break;
                        }

                        map.setSelectedTarget(pTarget);
                    }
                    else
                    {
                        map.setSelectedTarget(NULL);
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
        case INPUT_BUILD_SHRINE_CB:
            if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                map.buildShrine(ix, iy, SHRINE_CHARGED_BOLTS);
                if (!(mods & GLFW_MOD_SHIFT))
                    pInputHandler->setInputState(INPUT_IDLE);
            }
            break;
        case INPUT_BUILD_SHRINE_LI:
            if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                map.buildShrine(ix, iy, SHRINE_LIGHTNING);
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
                    if (pDraggedGem->pBuilding != NULL)
                        pCore->inventory.removeGemFromBuilding(pDraggedGem->pBuilding);
                    map.demolishBuilding(ix, iy);

                    Structure* pStructure = map.getStructure(iy, ix);
                    if (pStructure != NULL)
                        pStructure->receiveBombDamage(pDraggedGem->shotFinal, pDraggedGem->getBombDamage());

                    map.dropGemBomb(pDraggedGem, xrel, yrel);

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
                        map.demolishBuilding(ix, iy);

                        Structure* pStructure = map.getStructure(iy, ix);
                        if (pStructure != NULL)
                            pStructure->receiveBombDamage(pBombGem->shotFinal, pBombGem->getBombDamage());

                        map.dropGemBomb(pBombGem, xrel, yrel);

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

    if (pCore->inventory.getDraggedGem() != NULL)
        return;

    Building* pBuilding = map.getBuilding(iy, ix);
    Gem* pGem = NULL;
    if (pBuilding != NULL)
        pGem = pBuilding->pGem;

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_D:
                if ((pGem != NULL) && (pCore->manaPool.getMana() >= pGem->manaCost))
                {
                    if (NULL != pCore->inventory.duplicateGemIntoSlot(pGem, -1))
                        pCore->manaPool.addMana(-pGem->manaCost, false);
                }
                break;
            case GLFW_KEY_U:
                if ((pGem != NULL) && (pCore->manaPool.getMana() >= (pGem->manaCost + Gem::gemCombineCostCurrent)))
                {
                    pCore->manaPool.addMana(-(pGem->manaCost + Gem::gemCombineCostCurrent), false);
                    pCore->inventory.combineGems(pGem, pGem);
                }
                break;
            case GLFW_KEY_X:
                if (pGem != NULL)
                    pCore->inventory.salvageGem(pGem);
                break;
            case GLFW_KEY_TAB:
                if (pGem != NULL)
                    pCore->inventory.dropGemIntoInventory(pGem);
                break;
            case GLFW_KEY_UP:
            case GLFW_KEY_DOWN:
            {
                Targetable* pSelectedTarget = map.getSelectedTarget();
                if ((pSelectedTarget != NULL) && !!(pSelectedTarget->type & TARGET_MONSTER))
                {
                    int delta = (key == GLFW_KEY_UP) ? 1 : -1;
                    map.setSelectedTarget(map.enemyController.getNextTarget(pSelectedTarget, delta));
                }
            }
        }
    }
}
