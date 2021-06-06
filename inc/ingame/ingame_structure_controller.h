#pragma once

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

public:
    IngameStructureController(const IngameLevelDefinition& level);

    void render(struct _fbg* pFbg, const Window& window) const;

    void tickStructures(IngameMap& map, int frames);

    void destroyStructure(Structure* pStructure);

    std::vector<MonsterNest>& getMonsterNests() { return monsterNests; }

    std::vector<Targetable*> getTargetableStructuresWithinRangeSq(float y, float x, float rangeSq);
};
