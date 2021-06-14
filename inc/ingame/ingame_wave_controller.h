#pragma once

#include "entity/wave_stone.h"

#include "interface/window.h"

#include <random>
#include <vector>

struct _fbg;
class IngameMap;
class IngameManaPool;

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

    double getWaveHpGrowthGCL(int numWaves);

    void buildWaves();
    void activateNextWave();

    void activateWave();
    void activateSpark();

public:
    IngameWaveController(IngameMap& map_, IngameManaPool& manaPool_);

    STATUS render(struct _fbg* pFbg, const Window& window) const;

    int getWaveStoneAtPosition(int ypos, int maxHeight) const;

    void tick(int frames);

    void callWavesEarly(int waveNum);
};