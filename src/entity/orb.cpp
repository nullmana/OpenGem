#include "entity/orb.h"

Orb::Orb(int ix, int iy) : MonsterNode(ix + 1.0f, iy + 1.0f)
{
    type = BUILDING_ORB;
}
