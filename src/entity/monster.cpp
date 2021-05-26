#include "entity/monster.h"
#include "entity/monster_node.h"

#include "ingame/ingame_level_definition.h"
#include "ingame/ingame_map.h"

#include <cmath>

#define MONSTER_SPEED_FLOAT_FACTOR (1.0f / 17.0f)

Monster::Monster(const MonsterSpawnNode *pStart, const MonsterNode *pTarget)
{
	pSourceNode = pStart;
	pTargetNode = pTarget;

	color = 0xFFFFFF * (rand() / float(RAND_MAX));
	speed = 0.4f + (rand() / float(RAND_MAX));

	spawn();
}

void Monster::spawn()
{
	x = pSourceNode->spawnX;
	y = pSourceNode->spawnY;

	int ix = (int)x;
	int iy = (int)y;

	nextX = ix;
	nextY = iy;

	if (nextX < 0)
		nextX = 0;
	else if (nextX > g_game.ingameMapWidth-1)
		nextX = g_game.ingameMapWidth-1;
	if (nextY < 0)
		nextY = 0;
	else if (nextY > g_game.ingameMapHeight-1)
		nextY = g_game.ingameMapHeight-1;

	if ((ix != nextX) || (iy != nextY))
	{
		// Started off-screen, need to reach map first
		distanceToOrb = 1000000 * pTargetNode->tileDistance.at(nextY, nextX) + 10000;
		setNextTarget(nextX, nextY);
	}
	else
	{
		// Started on-screen, pick next node immediately
		pickNextTarget();
	}
}

void Monster::pickNextTarget()
{
	int ix = (int)x;
	int iy = (int)y;

	if (pTargetNode->type == BUILDING_ORB)
		distanceToOrb = 1000000 * pTargetNode->tileDistance.at(iy, ix) + 10000;
	else if (pSourceNode->type == BUILDING_ORB)
		distanceToOrb = 1000000 * pSourceNode->tileDistance.at(iy, ix) + 10000;
	else
		distanceToOrb = INT_MAX;

	int nextDirection = MonsterNode::pickDirection(pTargetNode->tileDirection.at(iy, ix));
	if (nextDirection != -1)
	{
		ix += + PathWeight::dx[nextDirection];
		iy += + PathWeight::dy[nextDirection];
	}
	setNextTarget(ix, iy);
}

void Monster::setNextTarget(int nx, int ny)
{
	nextX = nx;
	nextY = ny;

	nearNextX = nextX + 0.2f + 0.6f * (rand() / float(RAND_MAX));
	nearNextY = nextY + 0.2f + 0.6f * (rand() / float(RAND_MAX));

	adjustMotionAngle();
}

void Monster::adjustMotionAngle()
{
	motionAngle = atan2(nearNextY - y, nearNextX - x);

	speedCos = speed * cos(motionAngle) * MONSTER_SPEED_FLOAT_FACTOR;
	speedSin = speed * sin(motionAngle) * MONSTER_SPEED_FLOAT_FACTOR;
}

bool Monster::tick(IngameMap &map, int frames)
{
	if (isKilled)
		return true;

	if (frames > 0)
	{
		for (int f = 0; f < frames; ++f)
		{
			x += speedCos;
			y += speedSin;
			distanceToOrb -= 100*speed;

			int ix = (int)x;
			int iy = (int)y;

			if ((ix == nextX) && (iy == nextY))
			{
				if (pTargetNode->tileDirection.at(nextY, nextX).sum == 0)
				{
					// On zero weight tile, assume reached destination
					// Just delete for now
					map.monsterReachesTarget(*this);
					if (isKilled)
						return true;
				}
				else
				{
					pickNextTarget();
				}
			}
		}
	}

	return isKilled;
}
