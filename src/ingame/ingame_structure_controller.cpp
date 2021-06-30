#include "ingame/ingame_structure_controller.h"
#include "ingame/ingame_level_definition.h"
#include "ingame/ingame_map.h"

#include "wrapfbg.h"

#include <algorithm>
#include <unordered_set>

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

void IngameStructureController::tickStructures(IngameMap& map, int frames)
{
    std::unordered_set<Targetable*> invalidatedWithShots;

    for (std::list<Beacon>::iterator it = beacons.begin(); it != beacons.end();)
    {
        Beacon* pBeacon = &(*it);
        if (pBeacon->tick(map, frames))
        {
            if (pBeacon->incomingShots > 0)
                invalidatedWithShots.insert(pBeacon);

            if (pBeacon->isSelectedTarget)
                map.setSelectedTarget(NULL);

            map.destroyBeacon(pBeacon);
            destroyBeacon(pBeacon);
            beacons.erase(it++);
        }
        else
        {
            ++it;
        }
    }

    map.projectileController.clearShotsFromTarget(invalidatedWithShots);
}

void IngameStructureController::fillProtecting(Beacon* pBeacon)
{
    int ix = pBeacon->ix;
    int iy = pBeacon->iy;

    // TODO maybe optimize this.
    for (Beacon& b : beacons)
    {
        if ((b.isProtector() != pBeacon->isProtector()) &&
            (abs(b.ix - pBeacon->ix) <= g_game.ingameBuildingSize) &&
            (abs(b.iy - pBeacon->iy) <= g_game.ingameBuildingSize))
        {
            pBeacon->protecting.push_back(&b);
            b.protecting.push_back(pBeacon);
        }
    }
}

Beacon& IngameStructureController::addBeacon(int x, int y)
{
    BEACON_TYPE type;
    if (g_game.game == GC_LABYRINTH)
        type = BEACON_TYPE(rand() % BEACON_TYPE_COUNT_GCL);
    else
        type = BEACON_TYPE(rand() % BEACON_TYPE_COUNT_GCCS);

    beacons.emplace_back(x, y, type);

    Beacon* pBeacon = &beacons.back();

    if (g_game.game != GC_LABYRINTH)
        fillProtecting(pBeacon);

    return beacons.back();
}

void IngameStructureController::destroyBeacon(Beacon* pBeacon)
{
    if (g_game.game != GC_LABYRINTH)
    {
        for (Beacon* pOther : pBeacon->protecting)
        {
            std::list<Beacon*>::iterator it = std::find(pOther->protecting.begin(), pOther->protecting.end(), pBeacon);

            if (it != pOther->protecting.end())
                pOther->protecting.erase(it);
        }
    }
}

std::vector<Targetable*>& IngameStructureController::getTargetableStructuresWithinRangeSq(std::vector<Targetable*>& targets, float y, float x, float rangeSq)
{
    for (MonsterNest& m : monsterNests)
    {
        if (!m.isKilled && !m.isKillingShotOnTheWay && m.canBeTargeted() &&
            ((m.y - y) * (m.y - y) + (m.x - x) * (m.x - x) <= rangeSq))
        {
            targets.push_back(&m);
        }
    }

    for (Beacon& b : beacons)
    {
        if (!b.isKilled && !b.isKillingShotOnTheWay && b.canBeTargeted() &&
            ((b.y - y) * (b.y - y) + (b.x - x) * (b.x - x) <= rangeSq))
        {
            targets.push_back(&b);
        }
    }

    return targets;
}
