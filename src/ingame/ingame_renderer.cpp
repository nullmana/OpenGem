#include "ingame/ingame_renderer.h"
#include "ingame/ingame_core.h"

#include "wrapfbg.h"

#include <algorithm>

IngameRenderer::IngameRenderer(IngameCore& core) : windowMap(core.map, 39, 53, 918, 544)
{
    if (g_game.game == GC_LABYRINTH)
        windowMap.resize(30, 30, 660, 561);
}

static void windowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);

    float aspect_w = (float)g_game.ingameMapWidth / (float)g_game.ingameMapHeight;
    float realHeight = height * 0.95f - 53;
    float realWidth = fmin(realHeight * aspect_w, width * 0.75f - 39);

    int mapW = std::max<int>(realWidth, g_game.ingameMapWidth);
    int mapH = std::max<int>(realHeight, g_game.ingameMapHeight);
    pCore->renderer.getRootWindow()->resize(39, 53, mapW, mapH);
}

STATUS IngameRenderer::init(IngameCore& core)
{
    struct _fbg_glfw_context* pGlfwContext = (struct _fbg_glfw_context*)core.fbg()->user_context;

    glfwSetWindowSizeCallback(pGlfwContext->window, windowSizeCallback);

    return STATUS_OK;
}

STATUS IngameRenderer::render(IngameCore& core)
{
    struct _fbg* pFbg = core.fbg();

    fbg_rect(pFbg, windowMap.x - 2, windowMap.y - 2, windowMap.width + 4, windowMap.height + 4,
        0x22, 0x26, 0x31);

    windowMap.render(pFbg);

    core.map.enemyController.render(pFbg, windowMap);
    core.map.projectileController.render(pFbg, windowMap);

    return STATUS_OK;
}
