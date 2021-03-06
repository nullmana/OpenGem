#include "ingame/ingame_renderer.h"
#include "ingame/ingame_core.h"

#include "constants/ingame_input_state.h"

#include "wrapfbg.h"

#include <algorithm>

#include "ingame/ingame_button_functions.hpp"

static const std::vector<ButtonDefinition> buildButtonDefs_GCL = {
    {buttonBuildWall_handleMouseInput, button_handleMouseHover, buttonBuildWall_checkDisable},
    {buttonBuildTower_handleMouseInput, button_handleMouseHover, buttonBuildTower_checkDisable},
    {buttonBuildAmplifier_handleMouseInput, button_handleMouseHover, buttonBuildAmplifier_checkDisable},
    {buttonBuildShrineCB_handleMouseInput, button_handleMouseHover, buttonBuildShrine_checkDisable},
    {buttonBuildTrap_handleMouseInput, button_handleMouseHover, buttonBuildTrap_checkDisable},
    {buttonBuildShrineLI_handleMouseInput, button_handleMouseHover, buttonBuildShrine_checkDisable},
    {buttonBuildBomb_handleMouseInput, button_handleMouseHover, NULL},
    {buttonBuildCombine_handleMouseInput, button_handleMouseHover, buttonBuildCombine_checkDisable},
    {buttonBuildMana_handleMouseInput, button_handleMouseHover, buttonBuildMana_checkDisable},
};

static const std::vector<ButtonDefinition> buildButtonDefs_GCCS = {
    {buttonBuildWall_handleMouseInput, button_handleMouseHover, buttonBuildWall_checkDisable},
    {buttonBuildTower_handleMouseInput, button_handleMouseHover, buttonBuildTower_checkDisable},
    {buttonBuildAmplifier_handleMouseInput, button_handleMouseHover, buttonBuildAmplifier_checkDisable},
    {buttonBuildBomb_handleMouseInput, button_handleMouseHover, NULL},
    {buttonBuildTrap_handleMouseInput, button_handleMouseHover, buttonBuildTrap_checkDisable},
    {buttonBuildCombine_handleMouseInput, button_handleMouseHover, buttonBuildCombine_checkDisable},
};

static const std::vector<ButtonDefinition> gemButtonDefs_GCL = {
    {buttonGemCH_handleMouseInput, button_handleMouseHover, buttonGemCH_checkDisable, 0x669900},
    {buttonGemCR_handleMouseInput, button_handleMouseHover, buttonGemCR_checkDisable, 0x999100},
    {buttonGemML_handleMouseInput, button_handleMouseHover, buttonGemML_checkDisable, 0x994D00},
    {buttonGemPO_handleMouseInput, button_handleMouseHover, buttonGemPO_checkDisable, 0x009912},
    {buttonGemAnvil_handleMouseInput, NULL, NULL},
    {buttonGemBB_handleMouseInput, button_handleMouseHover, buttonGemBB_checkDisable, 0x990000},
    {buttonGemSH_handleMouseInput, button_handleMouseHover, buttonGemSH_checkDisable, 0x007399},
    {buttonGemSL_handleMouseInput, button_handleMouseHover, buttonGemSL_checkDisable, 0x1A0099},
    {buttonGemAT_handleMouseInput, button_handleMouseHover, buttonGemAT_checkDisable, 0x990099},
};

static const std::vector<ButtonDefinition> gemButtonDefs_GCCS = {
    {buttonGemML_handleMouseInput, button_handleMouseHover, buttonGemML_checkDisable, 0x994D00},
    {buttonGemCR_handleMouseInput, button_handleMouseHover, buttonGemCR_checkDisable, 0x999100},
    {buttonGemPB_handleMouseInput, button_handleMouseHover, buttonGemPB_checkDisable, 0x999999},
    {buttonGemCH_handleMouseInput, button_handleMouseHover, buttonGemCH_checkDisable, 0x990000},
    {buttonGemPO_handleMouseInput, button_handleMouseHover, buttonGemPO_checkDisable, 0x009912},
    {buttonGemSP_handleMouseInput, button_handleMouseHover, buttonGemSP_checkDisable, 0x007399},
    {buttonGemBB_handleMouseInput, button_handleMouseHover, buttonGemBB_checkDisable, 0x333333},
    {buttonGemSL_handleMouseInput, button_handleMouseHover, buttonGemSL_checkDisable, 0x1A0099},
    {buttonGemAT_handleMouseInput, button_handleMouseHover, buttonGemAT_checkDisable, 0x990099},
};

static const std::vector<ButtonDefinition> speedButtonDefs = {
    {buttonSpeed0_handleMouseInput, button_handleMouseHover, NULL},
    {buttonSpeed1_handleMouseInput, button_handleMouseHover, NULL},
    {buttonSpeed3_handleMouseInput, button_handleMouseHover, NULL},
    {buttonSpeed9_handleMouseInput, button_handleMouseHover, NULL},
};

IngameRenderer::IngameRenderer(IngameCore& core)
    : windowMap(core.map),
      windowMana(core.manaPool),
      windowInventory(core.inventory),
      windowWave(core.waveController),
      windowBuildSpells(g_game.game == GC_LABYRINTH ? buildButtonDefs_GCL : buildButtonDefs_GCCS, 3),
      windowCreateGems(g_game.game == GC_LABYRINTH ? gemButtonDefs_GCL : gemButtonDefs_GCCS, 3),
      windowSpeed(speedButtonDefs, 4)
{
    int width = 1280, height = 720;

    windowRoot.addChildWindow(&windowTarget, 10);
    windowRoot.addChildWindow(&windowMap, 0);
    windowRoot.addChildWindow(&windowMana, 1);
    windowRoot.addChildWindow(&windowWave, 2);
    windowRoot.addChildWindow(&windowBuildSpells, 3);
    windowRoot.addChildWindow(&windowCreateGems, 4);
    windowRoot.addChildWindow(&windowSpeed, 5);
    windowRoot.addChildWindow(&windowInventory, 6);

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

    windowRoot.resize(0, 0, width, height);
    if (windowTarget.width != 0.0f)
        windowTarget.resize(0, 0, width, height);
    else
        windowTarget.resize(0, 0, 0, 0);

    windowMap.resize(offsetX, offsetY, realWidth, realHeight);
    windowWave.resize(4, offsetY, offsetX - 8, realHeight);

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
    windowSpeed.resize((offsetX - 18) / 2, (offsetY - 18) / 2, 72, 18);
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

    windowRoot.render(pFbg);

    return STATUS_OK;
}
