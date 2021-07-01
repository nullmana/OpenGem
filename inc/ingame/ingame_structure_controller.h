#pragma once

#include "common/vector2d.h"

#include "entity/beacon.h"
#include "entity/monster_nest.h"

#include "interface/window.h"

#include <list>
#include <vector>

class IngameLevelDefinition;
class IngameMap;

class IngameStructureController
{
private:
    std::vector<MonsterNest> monsterNests;
    std::list<Beacon> beacons;
    vector2d<int> tileStatic;

    void fillProtecting(Beacon* pBeacon);
    void applyStaticBeacon(Beacon* pBeacon, int delta);

    void destroyBeacon(Beacon* pBeacon);

public:
    IngameStructureController(const IngameLevelDefinition& level);

    void render(struct _fbg* pFbg, const Window& window) const;

    void tickStructures(IngameMap& map, int frames);

    Beacon& addBeacon(int x, int y);

    bool checkStaticBeacons(int x, int y, int width, int height);

    std::vector<MonsterNest>& getMonsterNests() { return monsterNests; }

    std::vector<Targetable*>& getTargetableStructuresWithinRangeSq(std::vector<Targetable*>& targets,
        float y, float x, float rangeSq);
};
