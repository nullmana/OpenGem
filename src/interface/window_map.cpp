#include "interface/window_map.h"

#include "ingame/ingame_core.h"
#include "ingame/ingame_map.h"

#include "graphics.h"

#include <cmath>
#include <cstdlib>

STATUS WindowMap::render(GraphicsContext* pContext)
{
    nvgBeginPath(pContext->ctx);
    nvgRect(pContext->ctx, x - 2, y - 2, width + 4, height + 4);
    nvgFillColor(pContext->ctx, nvgRGB(0x22, 0x26, 0x31));
    nvgFill(pContext->ctx);

    return map.render(pContext, *this);
}

void WindowMap::handleMouseInput(GraphicsContext* pContext, int button, int action, int mods)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pContext->win);
    IngameInputHandler* pInputHandler = &pCore->inputHandler;
    double xpos, ypos;
    float scale = width / g_game.ingameMapWidth;

    glfwGetCursorPos(pContext->win, &xpos, &ypos);

    int ix = std::floor((xpos - x) / scale);
    int iy = std::floor((ypos - y) / scale);

    if ((ix < 0) || (ix >= g_game.ingameMapWidth) || (iy < 0) || (iy >= g_game.ingameMapHeight))
        return;

    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        map.destroyStructure(ix, iy);
        if (!(mods & GLFW_MOD_SHIFT))
            pInputHandler->setInputState(INPUT_IDLE);
        return;
    }

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
                    pInputHandler->setInputState(INPUT_IDLE);
                }
            }
            break;
    }
}
