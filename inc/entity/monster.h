#pragma once

#include "entity/monster_prototype.h"
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
    Monster(const MonsterSpawnNode* pStart, const MonsterNode* pTarget, const MonsterPrototype& mp);

    void spawn();

    virtual void receiveShotDamage(ShotData& shot, double damage, Gem* pSourceGem);
    virtual void receiveShrineDamage(double damage);
    virtual double calculateIncomingDamage(double damage);

    /*!
     * @brief Frame tick for this monster
     * @param dt Delta time change since last frame
     * @returns true if monster was killed, false otherwise
     */
    bool tick(IngameMap& map, int frames);

    void pickNextTarget();

    double getBanishmentCost() const { return mana * 2.0 * banishmentCostMultiplier; }
    virtual void setKillingShot()
    {
        isKillingShotOnTheWay = true;
        killingShotTimer = 45;
    }

    int nextX;
    int nextY;
    float nearNextX;
    float nearNextY;

    double armor;
    double mana;
    double banishmentCostMultiplier;

    float speed;
    float speedCos;
    float speedSin;
    float motionAngle;

    const MonsterSpawnNode* pSourceNode;
    const MonsterNode* pTargetNode;

    int8_t healthBarTimer;
    int8_t killingShotTimer;

    uint32_t color;
};
