#pragma once

#include "entity/wave_stone.h"

#include "interface/window.h"

#include <random>
#include <vector>

struct _fbg;
class IngameMap;
class IngameManaPool;

struct StageData;

class IngameWaveController
{
private:
    IngameMap& map;
    IngameManaPool& manaPool;

    std::vector<WaveStone> waves;
    std::minstd_rand randWave;

    int currentWaveStone;
    int rushWaveNum;
    int timeUntilNextWave;
    int waveStoneSpeed;

    uint64_t lastWaveTime;
    uint64_t waveClock;
    bool fullRushBonus;

    double getWaveHpGrowthGCL(int numWaves);

    void buildWaves(const StageData& stage);
    void activateNextWave();

    void activateWave();
    void activateSpark();

public:
    IngameWaveController(IngameMap& map_, IngameManaPool& manaPool_, const StageData& stage_);

    STATUS render(struct _fbg* pFbg, const Window& window) const;

    int getWaveStoneAtPosition(int ypos, int maxHeight) const;

    void tick(int frames);

    void callWavesEarly(int waveNum);

    void enrageWave(int waveNum, int grade, int count);
    void clearEnrageWave(int waveNum);

    int getWaveStoneSpeed() const { return waveStoneSpeed; }
};
