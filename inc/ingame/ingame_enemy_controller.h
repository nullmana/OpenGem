#pragma once

#include "common/vector2d.h"

#include "constants/target_type.h"

#include "entity/monster.h"
#include "entity/monster_node.h"

#include "interface/window.h"

#include <list>
#include <map>

class IngameMap;
class IngamePathfinder;
class IngameManaPool;
struct _fbg;

class IngameEnemyController
{
private:
    IngameManaPool& manaPool;

    std::list<Monster> monsters;
    std::multimap<uint64_t, const MonsterPrototype&> pendingMonsters;
    vector2d<std::vector<Monster*>> monstersOnTile;

    uint64_t pendingMonsterClock;

public:
    IngameEnemyController(IngameManaPool& mp_);
    void spawnMonsters(const IngamePathfinder& pathfinder, int num);

    void tickMonsters(IngameMap& map, int frames);

    void addPendingMonsters(const MonsterPrototype& mp, const std::vector<int>& times);

    // Force monsters currently pathing towards a tile which was replaced to repath
    void forceRepath(int x, int y, int w, int h);

    void render(struct _fbg* pFbg, const Window& window) const;

    const std::vector<Monster*>& getMonstersOnTile(int y, int x) const { return monstersOnTile.at(y, x); }
    const std::vector<Monster*>& getMonstersOnTile(int y, int x) { return monstersOnTile.at(y, x); }

    std::vector<Targetable*>& getTargetsWithinRangeSq(std::vector<Targetable*>& targets, float y, float x,
        float rangeSq, uint32_t typeMask, bool ignoreKillingShot);
    bool hasTargetsWithinRangeSq(float y, float x,
        float rangeSq, uint32_t typeMask, bool ignoreKillingShot) const;

    Targetable* getTargetAtPosition(float y, float x);
    Targetable* getNextTarget(Targetable* pTarget, int increment);

    std::vector<Targetable*>& getTowerTargetsWithinRangeSq(std::vector<Targetable*>& targets,
        float y, float x, float rangeSq, uint32_t typeMask)
    {
        return getTargetsWithinRangeSq(targets, y, x, rangeSq, typeMask, false);
    }
    std::vector<Targetable*> getShrineTargetsWithinRangeSq(float y, float x, float rangeSq)
    {
        std::vector<Targetable*> targets;
        return getTargetsWithinRangeSq(targets, y, x, rangeSq, TARGET_ENEMY, true);
    }
    std::vector<Targetable*> getBeaconTargetsWithinRangeSq(float y, float x, float rangeSq, bool ignoreKillingShot)
    {
        std::vector<Targetable*> targets;
        return getTargetsWithinRangeSq(targets, y, x, rangeSq, TARGET_MONSTER, ignoreKillingShot);
    }
    bool hasShrineTargetsWithinRangeSq(float y, float x, float rangeSq) const
    {
        return hasTargetsWithinRangeSq(y, x, rangeSq, TARGET_ENEMY, true);
    }

    static inline bool isTargetWithinRangeSq(const Targetable* t, float y, float x, float rangeSq)
    {
        return (t->y - y) * (t->y - y) + (t->x - x) * (t->x - x) <= rangeSq;
    }

#ifdef DEBUG
    const std::list<Monster>& getMonsters() const
    {
        return monsters;
    }
#endif
};
