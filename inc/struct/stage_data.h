#pragma once

struct StageMonsterData
{
public:
    // common
    int numWaves;
    double hpInitial;
    double armorIncrement;

    // gcl
    int typeFrequent;
    int typeUnused;

    // gccs
    int numGiantWaves;
    int numSwarmlingWaves;
    int firstNonReaverWave;
    int firstBuffedWave;
    int buffValueIncrement;
    double hpGrowth;
    double armorInitial;
};

struct StageData
{
public:
    StageMonsterData monsterData;

    uint32_t gemTypes;
};
