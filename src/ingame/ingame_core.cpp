#include "ingame/ingame_core.h"

#include "wrapfbg.h"

IngameCore::IngameCore(IngameLevelDefinition& level)
    : inputHandler(*this),
      renderer(*this),
      map(*this, level),
      inventory(manaPool, map.projectileController, 36, level.stage.gemTypes),
      waveController(map, manaPool, level.stage)
{
    switch (g_game.game)
    {
        case GC_LABYRINTH:
            Gem::gemCreateCostCurrent = 77.0;
            Gem::gemCombineCostCurrent = 77.0;
            break;
        case GC_CHASINGSHADOWS:
            Gem::gemCreateCostCurrent = 60.0;
            Gem::gemCombineCostCurrent = 120.0;
            break;
        default:
            throw "Game Code Unavailable!";
            break;
    }
}

STATUS IngameCore::init(struct _fbg* pFbg_)
{
    STATUS status = STATUS_OK;
    struct _fbg_glfw_context* pGlfwContext = (struct _fbg_glfw_context*)pFbg_->user_context;

    glfwSetWindowUserPointer(pGlfwContext->window, this);

    pFbg = pFbg_;

    status = inputHandler.init();
    if (status != STATUS_OK)
        return status;

    status = renderer.init(*this);
    if (status != STATUS_OK)
        return status;

    return status;
}
