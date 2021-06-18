#include "ingame/ingame_structure_controller.h"
#include "ingame/ingame_level_definition.h"

#include "wrapfbg.h"

IngameStructureController::IngameStructureController(const IngameLevelDefinition& level)
{
    for (const std::tuple<int, int, bool>& n : level.monsterNests)
    {
        monsterNests.emplace_back(std::get<0>(n), std::get<1>(n));
        monsterNests.back().isIndestructible = std::get<2>(n);
    }
}

void IngameStructureController::render(struct _fbg* pFbg, const Window& window) const
{
    struct _fbg_glfw_context* pGlfwContext = (struct _fbg_glfw_context*)pFbg->user_context;
    float scale = window.width / float(g_game.ingameMapWidth);
    double xpos, ypos;

    glfwGetCursorPos(pGlfwContext->window, &xpos, &ypos);

    for (const MonsterNest& n : monsterNests)
    {
        uint32_t color = 0x448262;
        uint8_t alpha = 0xFF;
        if (n.isIndestructible)
            color += 0x120022;
        if (n.isKilled)
            color /= 2;
        if ((xpos > n.ix * scale + window.x) && (xpos < (n.ix + n.width) * scale + window.x) &&
            (ypos > n.iy * scale + window.y) && (ypos < (n.iy + n.height) * scale + window.y))
        {
            alpha = 0x20;
        }

        fbgx_recta(pFbg, (n.ix + 0.5f) * scale + window.x, (n.iy + 0.5f) * scale + window.y,
            scale * (n.width - 1.0f), scale * (n.height - 1.0f), (color >> 16) & 0xFF,
            (color >> 8) & 0xFF, color & 0xFF, alpha);
    }
}

std::vector<Targetable*> IngameStructureController::getTargetableStructuresWithinRangeSq(
    float y, float x, float rangeSq)
{
    std::vector<Targetable*> targets;

    for (MonsterNest& m : monsterNests)
    {
        if (!m.isKilled && !m.isKillingShotOnTheWay && m.canBeTargeted() &&
            ((m.y - y) * (m.y - y) + (m.x - x) * (m.x - x) <= rangeSq))
        {
            targets.push_back(&m);
        }
    }

    return targets;
}
