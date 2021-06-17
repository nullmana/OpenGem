#pragma once

#include "constants/wave_type.h"

#include "entity/monster_prototype.h"

struct WaveStone
{
    WAVE_TYPE type;
    int timeOffset;

    bool isWave() const { return (type >= WAVE_REAVER) && (type <= WAVE_RUNNER); }

    // union everything below this for waves
    struct MonsterPrototype mp;

    WAVE_FORMATION formation;
    int waveNum;
    int numMonsters;
    int numMonstersUnenraged;
    double hpUnenraged;
    double armorUnenranged;

    // union everything below this for sparks
};
