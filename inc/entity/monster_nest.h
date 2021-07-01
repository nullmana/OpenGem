#pragma once

#include "entity/monster_node.h"
#include "entity/structure.h"

class MonsterNest : public Structure, public MonsterSpawnNode
{
public:
    MonsterNest(int ix_, int iy_);

    virtual float getShotVariance() const { return 0.3f * width; }

#ifdef DEBUG
    virtual void debugPrint() const;
#endif
};
