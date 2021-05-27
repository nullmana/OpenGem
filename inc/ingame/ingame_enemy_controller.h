#pragma once

#include "common/vector2d.h"

#include "entity/monster.h"
#include "entity/monster_node.h"

#include "interface/window.h"

#include <list>

class IngameMap;
class IngamePathfinder;
struct GraphicsContext;

class IngameEnemyController
{
private:
    std::list<Monster> monsters;
    vector2d<std::vector<Monster*>> monstersOnTile;

public:
    IngameEnemyController();
    void spawnMonsters(const IngamePathfinder& pathfinder, int num);

    void tickMonsters(IngameMap& map, int frames);

    // Force monsters currently pathing towards a tile which was replaced to repath
    void forceRepath(int x, int y, int w, int h);

    void render(struct GraphicsContext* pContext, const Window& window) const;

    const std::vector<Monster*>& getMonstersOnTile(int y, int x) const
    {
        return monstersOnTile.at(y, x);
    }
    const std::vector<Monster*>& getMonstersOnTile(int y, int x) { return monstersOnTile.at(y, x); }

    std::vector<Monster*> getLiveTargetsWithinRangeSq(float y, float x, float rangeSq);
#ifdef DEBUG
    const std::list<Monster>& getMonsters() const { return monsters; }
#endif
};
