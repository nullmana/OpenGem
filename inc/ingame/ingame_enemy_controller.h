#pragma once

#include "common/vector2d.h"

#include "entity/monster.h"
#include "entity/monster_node.h"

#include "interface/window.h"

#include <list>

class IngameMap;
class IngamePathfinder;
class IngameManaPool;
struct _fbg;

class IngameEnemyController
{
private:
    IngameManaPool& manaPool;

    std::list<Monster> monsters;
    vector2d<std::vector<Monster*>> monstersOnTile;

    std::vector<Targetable*> getTargetsWithinRangeSq(
        float y, float x, float rangeSq, bool ignoreKillingShot);
    bool hasTargetsWithinRangeSq(float y, float x, float rangeSq, bool ignoreKillingShot) const;

public:
    IngameEnemyController(IngameManaPool& mp_);
    void spawnMonsters(const IngamePathfinder& pathfinder, int num);

    void tickMonsters(IngameMap& map, int frames);

    // Force monsters currently pathing towards a tile which was replaced to repath
    void forceRepath(int x, int y, int w, int h);

    void render(struct _fbg* pFbg, const Window& window) const;

    const std::vector<Monster*>& getMonstersOnTile(int y, int x) const
    {
        return monstersOnTile.at(y, x);
    }
    const std::vector<Monster*>& getMonstersOnTile(int y, int x) { return monstersOnTile.at(y, x); }

    std::vector<Targetable*> getTowerTargetsWithinRangeSq(float y, float x, float rangeSq)
    {
        return getTargetsWithinRangeSq(y, x, rangeSq, false);
    }
    std::vector<Targetable*> getShrineTargetsWithinRangeSq(float y, float x, float rangeSq)
    {
        return getTargetsWithinRangeSq(y, x, rangeSq, true);
    }
    bool hasShrineTargetsWithinRangeSq(float y, float x, float rangeSq) const
    {
        return hasTargetsWithinRangeSq(y, x, rangeSq, true);
    }

#ifdef DEBUG
    const std::list<Monster>& getMonsters() const { return monsters; }
#endif
};
