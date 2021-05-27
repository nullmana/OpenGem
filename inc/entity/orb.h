#pragma once

#include "entity/amplifiable.h"
#include "entity/monster_node.h"

class Orb : public MonsterNode, public Amplifiable
{
public:
    Orb(int ix, int iy);
};
