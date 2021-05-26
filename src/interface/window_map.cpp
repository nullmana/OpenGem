#include "interface/window_map.h"

#include "ingame/ingame_core.h"
#include "ingame/ingame_map.h"

#include "wrapfbg.h"

#include <cmath>
#include <cstdlib>

STATUS WindowMap::render(struct _fbg* pFbg)
{
    return map.render(pFbg, *this);
}

void WindowMap::handleMouseInput(GLFWwindow* pWindow, int button, int action, int mods)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
    IngameInputHandler* pInputHandler = &pCore->inputHandler;
    double xpos, ypos;
    float scale = width / g_game.ingameMapWidth;

    glfwGetCursorPos(pWindow, &xpos, &ypos);

    xpos -= x;
    ypos -= y;

    int ix = std::floor(xpos / scale);
    int iy = std::floor(ypos / scale);

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
    }
}
