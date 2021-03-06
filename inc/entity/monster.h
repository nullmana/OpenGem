#pragma once

#include "constants/target_type.h"

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
    void adjustSpeedAngle();

    float speedCos;
    float speedSin;

    int motionCycle;

    int incomingShotsOnShield;

    double banishmentCostMultiplier;

    double getBanishmentCost() const { return mana * 2.0 * banishmentCostMultiplier; }

    inline bool isHoppingGCL() const { return !!(type & (TARGET_RUNNER | TARGET_REAVER)); }
    inline bool isCrawlingGCL() const { return !!(type & (TARGET_ARMORED | TARGET_GIANT)); }

public:
    Monster(const MonsterSpawnNode* pStart, const MonsterNode* pTarget, const MonsterPrototype& mp);

    void spawn();

    virtual uint32_t receiveShotDamage(ShotData& shot, uint32_t numShots, double damage, double crit,
        Gem* pSourceGem, bool isKillingShot);
    virtual void receiveShrineDamage(double damage);
    virtual void receiveBombDamage(const ShotData& shot, double damage);
    virtual double calculateIncomingDamage(double damage, double crit);
    virtual void addIncomingDamage(double damage);
    virtual void removeIncomingDamage(double damage);
    virtual bool canBeTargeted() const { return !isKilled && (invulnerabilityTimer <= 0); }

    void receiveBeaconHeal(double heal);
    void receiveBeaconHaste();
    void receiveBeaconCleanse();
    void receiveBeaconInvulnerability();
    void receiveBeaconShield();
    void receiveBeaconManaBind();

    /*!
     * @brief Frame tick for this monster
     * @param dt Delta time change since last frame
     * @returns true if monster was killed, false otherwise
     */
    bool tick(IngameMap& map, int frames);

    void pickNextTarget();

    void multiplyBanishmentCost(double mult);

    virtual void setKillingShot()
    {
        isKillingShotOnTheWay = true;
        killingShotTimer = 45;
    }

    int nextX;
    int nextY;
    float nearNextX;
    float nearNextY;
    float shockX;
    float shockY;

    double mana;
    double shockImmunity;
    double poisonDamage;

    int shield;

    float scale;

    float speed;
    float speedMax;
    float motionAngle;

    const MonsterSpawnNode* pSourceNode;
    const MonsterNode* pTargetNode;

    int32_t slowTimer;
    int16_t poisonTimer;
    int16_t manaBindTimer;
    int16_t invulnerabilityTimer;
    int8_t healthBarTimer;
    int8_t killingShotTimer;
    int8_t shockTimer;

    uint32_t color;

#ifdef DEBUG
    virtual void debugPrint() const;
#endif
};
