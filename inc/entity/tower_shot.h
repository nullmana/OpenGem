#pragma once

#include "entity/targetable.h"

class Tower;

class TowerShot
{
public:
	TowerShot(const Tower &tower, Targetable *pTarget_);

	Targetable *pTarget;
	float lastTargetX;
	float lastTargetY;

	float x;
	float y;
	float z; // altitude

	float velX;
	float velY;
	float velZ;

	bool tick(int frames);
};
