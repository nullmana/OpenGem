#pragma once

#include "entity/targetable.h"

#include <cstdint>

class IngameMap;
class MonsterNode;
class MonsterSpawnNode;

class Monster : public Targetable
{
private:
    void setNextTarget(int nx, int ny);
    void adjustMotionAngle();

public:
    Monster(const MonsterSpawnNode* pStart, const MonsterNode* pTarget);

    void spawn();

    virtual void receiveShotDamage() { isKilled = true; }

    /*!
     * @brief Frame tick for this monster
     * @param dt Delta time change since last frame
     * @returns true if monster was killed, false otherwise
     */
    bool tick(IngameMap& map, int frames);

    void pickNextTarget();

    int nextX;
    int nextY;
    float nearNextX;
    float nearNextY;

    float speed;
    float speedCos;
    float speedSin;
    float motionAngle;

    const MonsterSpawnNode* pSourceNode;
    const MonsterNode* pTargetNode;

    uint32_t color;
};
