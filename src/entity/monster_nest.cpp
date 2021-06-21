#include "entity/monster_nest.h"

#ifdef DEBUG
#include <cstdio>
#endif

MonsterNest::MonsterNest(int ix_, int iy_)
    : Structure(ix_, iy_, g_game.ingameMonsterNestSize, g_game.ingameMonsterNestSize),
      MonsterSpawnNode(g_game.ingameMonsterNestSize * 0.5f + ix_, g_game.ingameMonsterNestSize * 0.5f + iy_)
{
    spawnX = nodeX;
    spawnY = nodeY;
    nodeType = TILE_MONSTER_NEST;
}

#ifdef DEBUG
void MonsterNest::debugPrint() const
{
    printf("Monster Nest:\n");
    printf("\tHP: %lf/%lf | Armor: %lf\n", hp, hpMax, armor);
}
#endif
