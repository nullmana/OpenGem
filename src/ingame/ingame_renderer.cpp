#include "ingame/ingame_renderer.h"
#include "ingame/ingame_core.h"

#include "constants/ingame_input_state.h"

#include "wrapfbg.h"

#include <algorithm>

#include "ingame/ingame_button_functions.hpp"

static const std::vector<ButtonDefinition> buildButtonDefs_GCL = {
    {buttonBuildWall_handleMouseInput, button_handleMouseHover},
    {buttonBuildTower_handleMouseInput, button_handleMouseHover},
    {buttonBuildAmplifier_handleMouseInput, button_handleMouseHover},
    {NULL, button_handleMouseHover}, // Charged Bolt Shrine (C)
    {buttonBuildTrap_handleMouseInput, button_handleMouseHover},
    {NULL, button_handleMouseHover}, // Lightning Shrine (L)
    {NULL, button_handleMouseHover}, // Gem Bomb (B)
    {NULL, button_handleMouseHover}, // Combine Gems (G)
    {buttonBuildMana_handleMouseInput, button_handleMouseHover},
};

static const std::vector<ButtonDefinition> buildButtonDefs_GCCS = {
    {buttonBuildWall_handleMouseInput, button_handleMouseHover},
    {buttonBuildTower_handleMouseInput, button_handleMouseHover},
    {buttonBuildAmplifier_handleMouseInput, button_handleMouseHover},
    {NULL, button_handleMouseHover}, // Gem Bomb (B)
    {buttonBuildTrap_handleMouseInput, button_handleMouseHover},
    {NULL, button_handleMouseHover}, // Combine Gems (G)
};

static const std::vector<ButtonDefinition> gemButtonDefs_GCL = {
    {buttonGem0_handleMouseInput, button_handleMouseHover},
    {buttonGem1_handleMouseInput, button_handleMouseHover},
    {buttonGem2_handleMouseInput, button_handleMouseHover},
    {buttonGem3_handleMouseInput, button_handleMouseHover},
    {NULL, button_handleMouseHover},
    {buttonGem5_handleMouseInput, button_handleMouseHover},
    {buttonGem6_handleMouseInput, button_handleMouseHover},
    {buttonGem7_handleMouseInput, button_handleMouseHover},
    {buttonGem8_handleMouseInput, button_handleMouseHover},
};

static const std::vector<ButtonDefinition> gemButtonDefs_GCCS = {
    {buttonGem0_handleMouseInput, button_handleMouseHover},
    {buttonGem1_handleMouseInput, button_handleMouseHover},
    {buttonGem2_handleMouseInput, button_handleMouseHover},
    {buttonGem3_handleMouseInput, button_handleMouseHover},
    {buttonGem4_handleMouseInput, button_handleMouseHover},
    {buttonGem5_handleMouseInput, button_handleMouseHover},
    {buttonGem6_handleMouseInput, button_handleMouseHover},
    {buttonGem7_handleMouseInput, button_handleMouseHover},
    {buttonGem8_handleMouseInput, button_handleMouseHover},
};

IngameRenderer::IngameRenderer(IngameCore& core)
    : windowMap(core.map),
      windowMana(core.manaPool),
      windowInventory(core.inventory),
      windowBuildSpells(
          g_game.game == GC_LABYRINTH ? buildButtonDefs_GCL : buildButtonDefs_GCCS, 3),
      windowCreateGems(g_game.game == GC_LABYRINTH ? gemButtonDefs_GCL : gemButtonDefs_GCCS, 3)
{
    int width = 1280, height = 720;

    rootWindow.addChildWindow(&windowMap);
    rootWindow.addChildWindow(&windowMana);
    rootWindow.addChildWindow(&windowInventory);
    rootWindow.addChildWindow(&windowBuildSpells);
    rootWindow.addChildWindow(&windowCreateGems);

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
    float invHeight = fmin(invWidth * 4.0f, realHeight * 0.6f);
    invWidth = invHeight * 0.25f;

    float buildWidth = invWidth + 8;
    float buildHeight = buildWidth;
    if (g_game.game == GC_CHASINGSHADOWS)
        buildHeight = buildWidth * 2.0f / 3.0f;

    int mapW = std::max<int>(realWidth, g_game.ingameMapWidth);
    int mapH = std::max<int>(realHeight, g_game.ingameMapHeight);

    rootWindow.resize(0, 0, width, height);
    windowMap.resize(offsetX, offsetY, realWidth, realHeight);

    if (g_game.game == GC_LABYRINTH)
        windowMana.resize(0.635f * realWidth + offsetX, 4, 0.365f * realWidth, 20);
    else if (g_game.game == GC_CHASINGSHADOWS)
        windowMana.resize(0.4f * realWidth + offsetX, 5, 0.2f * realWidth, 40);
    else
        throw "Game Code Unavailable!";

    windowInventory.resize(
        offsetX + realWidth + 8, offsetY + (realHeight - invHeight) * 0.5f, invWidth, invHeight);
    windowBuildSpells.resize(offsetX + realWidth + 4, offsetY + 4, buildWidth, buildHeight);
    windowCreateGems.resize(
        offsetX + realWidth + 4, offsetY + realHeight - buildWidth, buildWidth, buildWidth);
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

    rootWindow.render(pFbg);

    return STATUS_OK;
}
