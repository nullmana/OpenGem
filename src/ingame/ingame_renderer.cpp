#include "ingame/ingame_renderer.h"
#include "ingame/ingame_core.h"

#include "graphics.h"

#include <algorithm>
#include <cmath>

IngameRenderer::IngameRenderer(IngameCore& core)
    : windowMap(core.map), windowInventory(core.inventory)
{
    int width = 1280, height = 720;

    rootWindow.addChildWindow(&windowMap);
    rootWindow.addChildWindow(&windowInventory);

    resize(width, height);
}

static void windowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
    pCore->renderer.resize(width, height);
}

void IngameRenderer::resize(int width, int height)
{
    int offsetX = 0;
    int offsetY = 0;

    if (g_game.game == GC_LABYRINTH)
    {
        offsetX = 30;
        offsetY = 30;
    }
    else if (g_game.game == GC_CHASINGSHADOWS)
    {
        offsetX = 39;
        offsetY = 53;
    }
    else
        throw "Game Code Unavailable!";

    float aspect_w = (float)g_game.ingameMapWidth / (float)g_game.ingameMapHeight;
    float realHeight = height * 0.95f - offsetY;
    float realWidth = fmin(realHeight * aspect_w, width * 0.75f - offsetX);
    realHeight = realWidth / aspect_w;

    float invWidth = width - realWidth - offsetX - 16;
    float invHeight = fmin(invWidth * 4.0f, realHeight * 2.0f / 3.0f);
    invWidth = invHeight * 0.25f;

    int mapW = std::max<int>(realWidth, g_game.ingameMapWidth);
    int mapH = std::max<int>(realHeight, g_game.ingameMapHeight);

    rootWindow.resize(0, 0, width, height);
    windowMap.resize(offsetX, offsetY, realWidth, realHeight);
    windowInventory.resize(
        offsetX + realWidth + 8, offsetY + (realHeight - invHeight) / 2, invWidth, invHeight);
}

STATUS IngameRenderer::init(IngameCore& core)
{
    GLFWwindow* pWindow = core.context()->win;

    glfwSetWindowSizeCallback(pWindow, windowSizeCallback);

    return STATUS_OK;
}

STATUS IngameRenderer::render(IngameCore& core)
{
    struct GraphicsContext* pContext = core.context();

    rootWindow.render(pContext);

    return STATUS_OK;
}
