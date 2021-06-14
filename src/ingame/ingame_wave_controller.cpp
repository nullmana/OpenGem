#include "ingame/ingame_wave_controller.h"
#include "ingame/ingame_mana_pool.h"
#include "ingame/ingame_map.h"

#include "constants/game_header.h"
#include "constants/target_type.h"

#include "wrapfbg.h"

IngameWaveController::IngameWaveController(IngameMap& map_, IngameManaPool& manaPool_)
    : map(map_), manaPool(manaPool_)
{
    currentWaveStone = -1;
    rushWaveNum = -1;
    timeUntilNextWave = 5;
    waveStoneSpeed = 1;

    buildWaves();
}

#define rand01(gen) ((gen)() / double((gen).max()))
#define rand01f(gen) ((gen)() / float((gen).max()))

double IngameWaveController::getWaveHpGrowthGCL(int numWaves)
{
    if (numWaves == 1337)
        return 1.074;
    else if (numWaves < 10)
        return 1.23;
    else if (numWaves < 15)
        return 1.18;
    else if (numWaves < 20)
        return 1.15;
    else if (numWaves < 25)
        return 1.126;
    else if (numWaves < 30)
        return 1.114;
    else if (numWaves < 35)
        return 1.10;
    else if (numWaves < 40)
        return 1.094;
    else if (numWaves < 45)
        return 1.088;
    else if (numWaves < 50)
        return 1.085;
    else if (numWaves < 55)
        return 1.084;
    else if (numWaves < 60)
        return 1.083;
    else if (numWaves < 65)
        return 1.079;
    else if (numWaves < 70)
        return 1.074;
    else if (numWaves < 75)
        return 1.069;
    else if (numWaves < 80)
        return 1.063;
    else
        return 1.051;
}

void IngameWaveController::buildWaves()
{
    randWave.seed();

    // Get this from stage data
    int numWaves = 8;
    double hpFirstWave = 8.0;
    double armorFirstWave = 1.0;
    double hpMultiplier = 0.010;
    double armorIncrement = 0.4;

    waves.resize(numWaves);

    int iw = 0;
    int totalStones = numWaves;
    for (int i = 0; i < totalStones; ++i, ++iw)
    {
        WaveStone& wave = waves[i];
        MonsterPrototype& mp = wave.mp;

        wave.waveNum = iw;
        wave.type = WAVE_REAVER;
        if (iw > 3)
            wave.type = (WAVE_TYPE)((g_game.game == GC_LABYRINTH) ? (iw % 4) : (iw % 3));

        wave.timeOffset = (g_game.game == GC_LABYRINTH) ? 1000 : 840;

        float numMonstersRaw = 1.0;
        switch (wave.type)
        {
            case WAVE_REAVER:
                mp.type = TARGET_REAVER;
                if (g_game.game == GC_LABYRINTH)
                    numMonstersRaw = rand01f(randWave) * 8.99f + 8.0f;
                else
                    numMonstersRaw = rand01f(randWave) * 10.99f + 6.0f;
                break;
            case WAVE_SWARMLING:
                mp.type = TARGET_SWARMLING;
                if (g_game.game == GC_LABYRINTH)
                    numMonstersRaw = rand01f(randWave) * 20.99f + 40.0f;
                else
                    numMonstersRaw = rand01f(randWave) * 40.99f + 20.0f;
                break;
            case WAVE_GIANT:
                mp.type = TARGET_GIANT;
                if (g_game.game == GC_LABYRINTH)
                    numMonstersRaw = rand01f(randWave) * 3.99 + 1.0f;
                else
                    numMonstersRaw = rand01f(randWave) * 2.99 + 2.0f;
                break;
            case WAVE_ARMORED:
                mp.type = TARGET_ARMORED;
                numMonstersRaw = rand01f(randWave) * 8.99f + 8.0f;
                break;
            case WAVE_RUNNER:
                mp.type = TARGET_RUNNER;
                numMonstersRaw = rand01f(randWave) * 8.99f + 8.0f;
                break;
        }
        if (g_game.game == GC_CHASINGSHADOWS)
        {
            if (randWave() < randWave.max() / 5 * 2)
                numMonstersRaw *= rand01f(randWave) * 0.2f + 1.1f;
            if (randWave() < randWave.max() / 5)
                numMonstersRaw *= rand01f(randWave) * 0.2f + 1.1f;
        }

        if (g_game.game == GC_LABYRINTH)
        {
            wave.numMonsters = round(numMonstersRaw);

            mp.hp = hpFirstWave * pow(hpMultiplier, iw);
            mp.armor = std::max(0.0, round(armorIncrement * (iw - 3) * (1 + rand01(randWave))));
            mp.mana = 12.0 * (0.64 * iw + 10.0) / wave.numMonsters;
            mp.banishmentCostMultiplier = 1.0;
            mp.speed = 0.65f * (2.8f + 0.08f * ((randWave() % 20) - 10));

            switch (wave.type)
            {
                case WAVE_SWARMLING:
                    mp.hp *= 0.35;
                    mp.armor *= 0.8;
                    mp.mana *= 0.5;
                    mp.speed = round(mp.speed * 1.4f);
                    break;
                case WAVE_GIANT:
                    mp.hp *= floor(60.0 / wave.numMonsters);
                    mp.armor = mp.armor * 1.3 + 3.0;
                    mp.mana *= 1.4;
                    mp.speed = 0.55f;
                    break;
                case WAVE_ARMORED:
                    mp.hp *= 1.4;
                    mp.armor = mp.armor * 3.0 + 4.0;
                    mp.speed = round(mp.speed * 0.6f);
                    break;
                case WAVE_RUNNER:
                    mp.hp *= 0.8;
                    mp.armor *= 0.9;
                    mp.speed = round(mp.speed * 2.0f);
                    break;
            }

            if (mp.speed > 4.0f)
                mp.speed = 4.0f;
            if (mp.hp > 3333333333.0f)
                mp.hp = 3333333333.0f;
        }
        else if (g_game.game == GC_CHASINGSHADOWS)
        {
            wave.numMonsters = std::max<int>(2, round(numMonstersRaw));

            mp.hp = std::min(1E300, hpFirstWave * pow(hpMultiplier, iw));
            mp.armor = armorFirstWave + std::max(0.0, round(armorIncrement * (iw - 3) * (1 + rand01(randWave))));
            mp.mana = 0.7 * std::max(1.0, round(0.5 * 6.74 * (0.65 * iw + 10.0)));
            mp.banishmentCostMultiplier = 1.0;
            mp.speed = (rand() > (RAND_MAX / 2)) ? 2.34f : 1.3f;

            switch (wave.type)
            {
                case WAVE_REAVER:
                    mp.speed *= 0.67f;
                    break;
                case WAVE_SWARMLING:
                    mp.hp *= 0.187;
                    mp.armor *= 0.574;
                    mp.mana *= 0.25;
                    mp.speed *= 0.77f;
                    break;
                case WAVE_GIANT:
                    mp.hp *= 5.0 / wave.numMonsters * 0.67 * 37.3;
                    mp.armor = (mp.armor + 4.2) * 2.47 * 0.52;
                    mp.mana *= 3.0;
                    mp.speed = std::min<float>(1.5f, mp.speed * 0.17f);
                    break;
            }
        }
        else
            throw "Game Code Unavailable!";

        mp.hp = round(mp.hp);
        mp.armor = round(mp.armor);

        wave.hpUnenraged = mp.hp;
        wave.armorUnenranged = mp.armor;
        wave.numMonstersUnenraged = wave.numMonsters;

        if (g_game.game == GC_CHASINGSHADOWS)
        {
            if ((iw > 0) && (iw % 5 == 0) && (iw < numWaves - 1))
            {
                WaveStone& spark = waves[++i];
                spark.type = WAVE_SPARK;
                spark.timeOffset = 208;
                printf("Spark\n");

                waves.emplace_back();
                ++totalStones;
            }
        }
    }
}

void IngameWaveController::activateNextWave()
{
    ++currentWaveStone;
    const WaveStone& wave = waves[currentWaveStone];

    timeUntilNextWave = wave.timeOffset;
    waveStoneSpeed = 1;

    if (wave.isWave())
    {
        activateWave();
    }
    else
    {
        activateSpark();
    }
}

void IngameWaveController::activateWave()
{
    WaveStone& wave = waves[currentWaveStone];
    std::vector<int> times(wave.numMonsters);

    for (int i = 0; i < times.size(); ++i)
        times[i] = i * 5;

    int timeLimit;
    if (g_game.game == GC_LABYRINTH)
    {
        timeLimit = 170;
    }
    else if (g_game.game == GC_CHASINGSHADOWS)
    {
        float timeMultiplier = 1.0f;
        if (wave.numMonsters != wave.numMonstersUnenraged)
        {
            if (wave.numMonsters >= wave.numMonstersUnenraged * 9)
                timeMultiplier = 3.0f;
            else
                timeMultiplier = sqrt(wave.numMonsters / wave.numMonstersUnenraged);
        }
        timeLimit = 600 * timeMultiplier;
    }
    else
    {
        timeLimit = 1400;
    }

    int timeMax = times.back();
    if (timeMax > timeLimit)
    {
        for (int& t : times)
        {
            t *= timeLimit;
            t /= timeMax;
        }
    }

    map.enemyController.addPendingMonsters(wave.mp, times);
}

void IngameWaveController::activateSpark()
{
}

STATUS IngameWaveController::render(struct _fbg* pFbg, const Window& window) const
{
    float vscale = (g_game.game == GC_LABYRINTH) ? (1.0f / 14.0f) : (1.0f / 8.0f);

    int y = timeUntilNextWave * vscale;
    for (int i = currentWaveStone + 1; (i < waves.size()) && (y < window.height); ++i)
    {
        const WaveStone& wave = waves[i];
        uint32_t color = WAVE_TYPE_COLOR[wave.type];

        int height = wave.timeOffset * vscale;

        fbg_rect(pFbg, window.x, y + window.y, window.width, std::min<int>(height, window.height - y),
            (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);

        if ((i == rushWaveNum) && (y + height - 8 < window.height))
        {
            fbg_rect(pFbg, window.x, y + window.y + height - 8,
                window.width, std::min<int>(8, window.height - y), 0x33, 0x33, 0x33);
        }

        y += height + 1;
    }

    return STATUS_OK;
}

int IngameWaveController::getWaveStoneAtPosition(int ypos, int maxHeight) const
{
    float vscale = (g_game.game == GC_LABYRINTH) ? (1.0f / 14.0f) : (1.0f / 8.0f);

    int y = timeUntilNextWave * vscale;
    for (int i = currentWaveStone + 1; (i < waves.size()) && (y < maxHeight); ++i)
    {
        const WaveStone& wave = waves[i];

        int height = wave.timeOffset * vscale;

        if ((ypos >= y) && (ypos <= y + height))
            return i;

        y += height + 1;
    }

    return -1;
}

void IngameWaveController::tick(int frames)
{
    for (int f = 0; (f < frames) && (currentWaveStone < int(waves.size() - 1)); ++f)
    {
        if (rushWaveNum > currentWaveStone)
            waveStoneSpeed += 3;

        timeUntilNextWave = std::max<int>(0, timeUntilNextWave - waveStoneSpeed);

        if (timeUntilNextWave <= 0)
        {
            if ((waveStoneSpeed > 1) && (currentWaveStone > -1) && waves[currentWaveStone + 1].isWave())
            {
                // TODO proportional early wave bonus
                double manaBonus = 10.0 * (round(pow(0.54 * currentWaveStone + 1.6, 1.214) * 10.0) / 10.0);
                manaPool.addMana(manaBonus, true);
            }

            activateNextWave();
        }
    }
}

void IngameWaveController::callWavesEarly(int waveNum)
{
    if (waveNum == -1)
        waveNum = currentWaveStone + 1;

    rushWaveNum = waveNum;
}

#undef rand01
#undef rand01f
