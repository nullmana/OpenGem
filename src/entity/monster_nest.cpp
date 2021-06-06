#include "entity/monster_nest.h"

MonsterNest::MonsterNest(int ix_, int iy_)
    : Structure(ix_, iy_, g_game.ingameMonsterNestSize, g_game.ingameMonsterNestSize),
      MonsterSpawnNode(
          g_game.ingameMonsterNestSize * 0.5f + ix_, g_game.ingameMonsterNestSize * 0.5f + iy_)
{
    x = spawnX = nodeX;
    y = spawnY = nodeY;
}
