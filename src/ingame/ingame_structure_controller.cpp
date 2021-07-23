#include "ingame/ingame_structure_controller.h"
#include "ingame/ingame_level_definition.h"
#include "ingame/ingame_map.h"

#include "wrapfbg.h"

#include <algorithm>
#include <unordered_set>

IngameStructureController::IngameStructureController(IngameManaPool& manaPool_, const IngameLevelDefinition& level)
    : manaPool(manaPool_),
      tileStatic(g_game.ingameMapHeight, g_game.ingameMapWidth)
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

    for (const ManaShard& m : manaShards)
    {
        uint32_t color;
        if (m.isCorrupted)
            color = 0xDB9509;
        else if (m.armor > 0.0)
            color = 0x9B1AD9;
        else
            color = 0x269BEA;

        fbg_rect(pFbg, m.ix * scale + window.x, m.iy * scale + window.y, scale * m.width, scale * m.height,
            (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    }

    for (const Beacon& b : beacons)
    {
        switch (b.beaconType)
        {
            case BEACON_HEAL:
                fbg_hline(pFbg, (b.ix + 0.1f * b.width) * scale + window.x, (b.iy + 0.5f * b.width) * scale + window.y,
                    0.8f * b.width * scale, 0, 0, 0);
                fbg_vline(pFbg, (b.ix + 0.5f * b.width) * scale + window.x, (b.iy + 0.1f * b.width) * scale + window.y,
                    0.8f * b.width * scale, 0, 0, 0);
                break;
            case BEACON_SPEED:
                fbg_line(pFbg, (b.ix + 0.1f * b.width) * scale + window.x, (b.iy + 0.1f * b.width) * scale + window.y,
                    (b.ix + 0.8f * b.width) * scale + window.x, (b.iy + 0.5f * b.width) * scale + window.y, 0, 0, 0);
                fbg_line(pFbg, (b.ix + 0.8f * b.width) * scale + window.x, (b.iy + 0.5f * b.width) * scale + window.y,
                    (b.ix + 0.1f * b.width) * scale + window.x, (b.iy + 0.9f * b.width) * scale + window.y, 0, 0, 0);
                break;
            case BEACON_CLEANSE:
            {
                int vertices[8];
                vertices[0] = (b.ix + 0.5f * b.width) * scale + window.x;
                vertices[1] = (b.iy + 0.1f * b.width) * scale + window.y;
                vertices[2] = (b.ix + 0.9f * b.width) * scale + window.x;
                vertices[3] = (b.iy + 0.5f * b.width) * scale + window.y;
                vertices[4] = (b.ix + 0.5f * b.width) * scale + window.x;
                vertices[5] = (b.iy + 0.9f * b.width) * scale + window.y;
                vertices[6] = (b.ix + 0.1f * b.width) * scale + window.x;
                vertices[7] = (b.iy + 0.5f * b.width) * scale + window.y;
                fbg_polygon(pFbg, 4, vertices, 0, 0, 0);
                break;
            }
            case BEACON_SHIELD:
            {
                int vertices[6];
                vertices[0] = (b.ix + 0.1f * b.width) * scale + window.x;
                vertices[1] = (b.iy + 0.1f * b.width) * scale + window.y;
                vertices[2] = (b.ix + 0.9f * b.width) * scale + window.x;
                vertices[3] = (b.iy + 0.1f * b.width) * scale + window.y;
                vertices[4] = (b.ix + 0.5f * b.width) * scale + window.x;
                vertices[5] = (b.iy + 0.9f * b.width) * scale + window.y;
                fbg_polygon(pFbg, 3, vertices, 0, 0, 0);
                break;
            }
            case BEACON_DISCHARGE:
            {
                int vertices[6];
                vertices[0] = (b.ix + 0.5f * b.width) * scale + window.x;
                vertices[1] = (b.iy + 0.1f * b.width) * scale + window.y;
                vertices[2] = (b.ix + 0.9f * b.width) * scale + window.x;
                vertices[3] = (b.iy + 0.9f * b.width) * scale + window.y;
                vertices[4] = (b.ix + 0.1f * b.width) * scale + window.x;
                vertices[5] = (b.iy + 0.9f * b.width) * scale + window.y;
                fbg_polygon(pFbg, 3, vertices, 0, 0, 0);
                break;
            }
            case BEACON_PROTECTOR:
            {
                int vertices[8];
                vertices[0] = (b.ix + 0.1f * b.width) * scale + window.x;
                vertices[1] = (b.iy + 0.1f * b.width) * scale + window.y;
                vertices[2] = (b.ix + 0.9f * b.width) * scale + window.x;
                vertices[3] = (b.iy + 0.1f * b.width) * scale + window.y;
                vertices[4] = (b.ix + 0.9f * b.width) * scale + window.x;
                vertices[5] = (b.iy + 0.9f * b.width) * scale + window.y;
                vertices[6] = (b.ix + 0.1f * b.width) * scale + window.x;
                vertices[7] = (b.iy + 0.9f * b.width) * scale + window.y;
                fbg_polygon(pFbg, 4, vertices, 0, 0, 0);
                break;
            }
            case BEACON_STATIC:
                fbg_line(pFbg, (b.ix + 0.1f * b.width) * scale + window.x, (b.iy + 0.1f * b.width) * scale + window.y,
                    (b.ix + 0.9f * b.width) * scale + window.x, (b.iy + 0.9f * b.width) * scale + window.y, 0, 0, 0);
                fbg_line(pFbg, (b.ix + 0.9f * b.width) * scale + window.x, (b.iy + 0.1f * b.width) * scale + window.y,
                    (b.ix + 0.1f * b.width) * scale + window.x, (b.iy + 0.9f * b.width) * scale + window.y, 0, 0, 0);
                break;
            case BEACON_OMNI:
            {
                int vertices[8];
                vertices[0] = (b.ix + 0.5f * b.width) * scale + window.x;
                vertices[1] = (b.iy + 0.1f * b.width) * scale + window.y;
                vertices[2] = (b.ix + 0.9f * b.width) * scale + window.x;
                vertices[3] = (b.iy + 0.5f * b.width) * scale + window.y;
                vertices[4] = (b.ix + 0.5f * b.width) * scale + window.x;
                vertices[5] = (b.iy + 0.9f * b.width) * scale + window.y;
                vertices[6] = (b.ix + 0.1f * b.width) * scale + window.x;
                vertices[7] = (b.iy + 0.5f * b.width) * scale + window.y;
                fbg_polygon(pFbg, 4, vertices, 0, 0, 0);

                fbg_hline(pFbg, (b.ix + 0.1f * b.width) * scale + window.x, (b.iy + 0.5f * b.width) * scale + window.y,
                    0.8f * b.width * scale, 0, 0, 0);
                fbg_vline(pFbg, (b.ix + 0.5f * b.width) * scale + window.x, (b.iy + 0.1f * b.width) * scale + window.y,
                    0.8f * b.width * scale, 0, 0, 0);

                fbg_line(pFbg, (b.ix + 0.1f * b.width) * scale + window.x, (b.iy + 0.1f * b.width) * scale + window.y,
                    (b.ix + 0.8f * b.width) * scale + window.x, (b.iy + 0.5f * b.width) * scale + window.y, 0, 0, 0);
                fbg_line(pFbg, (b.ix + 0.8f * b.width) * scale + window.x, (b.iy + 0.5f * b.width) * scale + window.y,
                    (b.ix + 0.1f * b.width) * scale + window.x, (b.iy + 0.9f * b.width) * scale + window.y, 0, 0, 0);
                break;
            }
        }
    }
}

void IngameStructureController::tickStructures(IngameMap& map, int frames)
{
    std::unordered_set<Targetable*> invalidatedWithShots;

    for (std::list<ManaShard>::iterator it = manaShards.begin(); it != manaShards.end();)
    {
        ManaShard* pManaShard = &(*it);
        if (pManaShard->isKilled)
        {
            if (pManaShard->incomingShots > 0)
                invalidatedWithShots.insert(pManaShard);

            if (pManaShard->isSelectedTarget)
                map.setSelectedTarget(NULL);

            map.destroyManaShard(pManaShard);
            manaShards.erase(it++);
        }
        else
        {
            ++it;
        }
    }

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

void IngameStructureController::applyStaticBeacon(Beacon* pBeacon, int delta)
{
    for (int y = std::max(0, pBeacon->iy - 3); y < std::min(g_game.ingameMapHeight, pBeacon->iy + 5); ++y)
        for (int x = std::max(0, pBeacon->ix - 3); x < std::min(g_game.ingameMapWidth, pBeacon->ix + 5); ++x)
            tileStatic.at(y, x) += delta;
}

ManaShard& IngameStructureController::addManaShard(int x, int y, int size, double mana, double shell, bool corrupted)
{
    manaShards.emplace_back(manaPool, x, y, size, mana, shell, corrupted);

    ManaShard* pShard = &manaShards.back();

    return manaShards.back();
}

Beacon& IngameStructureController::addBeacon(int x, int y)
{
    BEACON_TYPE beaconType;
    if (g_game.game == GC_LABYRINTH)
        beaconType = BEACON_TYPE(rand() % BEACON_TYPE_COUNT_GCL);
    else
        beaconType = BEACON_TYPE(rand() % BEACON_TYPE_COUNT_GCCS);

    beacons.emplace_back(x, y, beaconType);

    Beacon* pBeacon = &beacons.back();

    if (g_game.game != GC_LABYRINTH)
    {
        fillProtecting(pBeacon);

        if (beaconType == BEACON_STATIC)
            applyStaticBeacon(pBeacon, 1);
    }

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

        if (pBeacon->beaconType == BEACON_STATIC)
            applyStaticBeacon(pBeacon, -1);
    }
}

bool IngameStructureController::checkStaticBeacons(int x, int y, int width, int height)
{
    for (int j = y; j < y + height; ++j)
        for (int i = x; i < x + width; ++i)
            if (tileStatic.at(j, i) > 0)
                return false;
    return true;
}

std::vector<Targetable*>& IngameStructureController::getTargetableStructuresWithinRangeSq(std::vector<Targetable*>& targets, float y, float x, float rangeSq, bool isStructureTarget)
{
    for (MonsterNest& m : monsterNests)
    {
        if (!m.isKilled && !m.isKillingShotOnTheWay && m.canBeTargeted() &&
            ((m.y - y) * (m.y - y) + (m.x - x) * (m.x - x) <= rangeSq))
        {
            targets.push_back(&m);
        }
    }

    for (ManaShard& m : manaShards)
    {
        if (!m.isKilled && !m.isKillingShotOnTheWay && m.canBeTargeted() &&
            (!m.isCorrupted || isStructureTarget) &&
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
