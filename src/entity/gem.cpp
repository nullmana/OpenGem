#include "entity/gem.h"
#include "entity/building.h"

#include "constants/game_header.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#ifdef DEBUG
#include <cstdio>
#endif

double Gem::gemCreateCostCurrent = 0;
double Gem::gemCombineCostCurrent = 0;

static uint32_t HSVtoRGB(uint32_t HSV)
{
    int H = (HSV >> 16) & 0xFFFF;
    int S = (HSV >> 8) & 0xFF;
    int V = HSV & 0xFF;

    float C = (V / 100.0f) * (S / 100.0f);
    float X = C * (1.0f - fabs(fmod(H / 60.0f, 2) - 1.0f));
    float m = (V / 100.0f) - C;

    float r = 0.0f, g = 0.0f, b = 0.0f;

    switch (H / 60)
    {
        case 0:
            r = C;
            g = X;
            break;
        case 1:
            r = X;
            g = C;
            break;
        case 2:
            g = C;
            b = X;
            break;
        case 3:
            g = X;
            b = C;
            break;
        case 4:
            r = X;
            b = C;
            break;
        case 5:
            r = C;
            b = X;
            break;
    }

    return (uint32_t((r + m) * 0xFF) << 16) | (uint32_t((g + m) * 0xFF) << 8) |
           uint32_t((b + m) * 0xFF);
}

static uint32_t combineHSV(uint32_t a, uint32_t b, bool increaseSat)
{
    int H, S, V;

    if (a == b)
    {
        if (g_game.game == GC_LABYRINTH)
        {
            H = a >> 16;
            V = a & 0xFF;

            S = ((a >> 8) & 0xFF) / 2 + ((b >> 8) & 0xFF) / 2;
            if (increaseSat)
                S = std::min<int>(100, S + 10);
            else
                S = std::max<int>(50, S - 10);

            return (uint32_t(H) << 16) | (uint32_t(S) << 8) | uint32_t(V);
        }
        else
            return a;
    }

    if ((a & 0xFF00) == 0)
    {
        if ((b & 0xFF00) == 0)
        {
            H = 0;
            S = 0;
        }
        else
        {
            H = b >> 16;
            S = (b >> 8) & 0xFF;
        }
    }
    else
    {
        if ((b & 0xFF00) == 0)
        {
            H = a >> 16;
            S = (a >> 8) & 0xFF;
        }
        else
        {
            int dH = abs((a >> 16) - (b >> 16));
            if (dH > 180)
            {
                dH = 360 - dH;
                H = (std::max<int>(a >> 16, b >> 16) + dH / 2) % 360;
            }
            else
            {
                H = std::min<int>(a >> 16, b >> 16) + dH / 2;
            }

            if (g_game.game == GC_LABYRINTH)
            {
                S = ((a >> 8) & 0xFF) / 2 + ((b >> 8) & 0xFF) / 2;
                if (increaseSat)
                    S = std::min<int>(100, S + 10);
                else
                    S = std::max<int>(50, S - 10);
            }
            else
            {
                S = std::max<int>(((a >> 8) & 0xFF), ((b >> 8) & 0xFF));
            }
        }
    }

    V = ((a & 0xFF) + (b & 0xFF)) / 2;

    return (uint32_t(H) << 16) | (uint32_t(S) << 8) | uint32_t(V);
}

Gem::Gem(int grade_, GEM_COMPONENT_TYPE type)
{
    pBuilding = NULL;
    x = 0.0f;
    y = 0.0f;
    isDragged = false;

    float rangeScale = (g_game.game == GC_LABYRINTH) ? (1.0f / 33.0f) : (1.0f / 17.0f);

    grade = 0;
    manaCost = gemCreateCostCurrent;

    hits = 0;
    kills = 0;
    killsNonCombined = 0;

    componentMask = (1 << type);
    displayComponents[0] = type;
    displayComponents[1] = GEM_COMPONENT_TYPE_COUNT;
    displayComponents[2] = GEM_COMPONENT_TYPE_COUNT;

    switch (type)
    {
        case GEM_SLOW:
            HSV = (250 << 16) | (100 << 8) | 100;
            shotRaw.damageMin = 5.0;
            shotRaw.damageMax = 12.0;
            shotRaw.range = 77.0f * rangeScale;
            shotRaw.fireRate = 2.1f * 30.0f / 100.0f;
            if (g_game.game == GC_LABYRINTH)
            {
                shotRaw.component[COMPONENT_SLOW_POWER] = 0.18;
                shotRaw.component[COMPONENT_SLOW_DURATION] = 120;
            }
            else
            {
                shotRaw.component[COMPONENT_SLOW_POWER] = 1.04;
                shotRaw.component[COMPONENT_SLOW_DURATION] = 60;
            }
            break;
        case GEM_CHAIN:
            if (g_game.game == GC_LABYRINTH)
                HSV = (80 << 16) | (100 << 8) | 100;
            else
                HSV = (100 << 8) | 100;
            shotRaw.damageMin = 6.0;
            shotRaw.damageMax = 10.0;
            shotRaw.range = 78.0f * rangeScale;
            shotRaw.fireRate = 2.2f * 30.0f / 100.0f;
            if (g_game.game == GC_LABYRINTH)
                shotRaw.component[COMPONENT_CHAIN] = 0.15;
            else
                shotRaw.component[COMPONENT_CHAIN] = 1.01;
            break;
        case GEM_POISON:
            HSV = (127 << 16) | (100 << 8) | 100;
            shotRaw.damageMin = 5.0;
            shotRaw.damageMax = 10.0;
            shotRaw.range = 79.0f * rangeScale;
            shotRaw.fireRate = 2.1f * 30.0f / 100.0f;
            shotRaw.component[COMPONENT_POISON] = 8.0;
            break;
        case GEM_ARMOR:
            HSV = (300 << 16) | (100 << 8) | 100;
            shotRaw.damageMin = 3.0;
            shotRaw.damageMax = 11.0;
            shotRaw.range = 73.0f * rangeScale;
            shotRaw.fireRate = 2.2f * 30.0f / 100.0f;
            if (g_game.game == GC_LABYRINTH)
                shotRaw.component[COMPONENT_ARMOR] = 0.18;
            else
                shotRaw.component[COMPONENT_ARMOR] = 1.73;
            break;
        case GEM_SHOCK:
            HSV = (195 << 16) | (100 << 8) | 100;
            shotRaw.damageMin = 4.0;
            shotRaw.damageMax = 10.0;
            shotRaw.range = 75.0f * rangeScale;
            shotRaw.fireRate = 2.2f * 30.0f / 100.0f;
            shotRaw.component[COMPONENT_SHOCK] = 0.17;
            break;
        case GEM_BLOODBOUND:
            if (g_game.game == GC_LABYRINTH)
                HSV = (100 << 8) | 100;
            else
                HSV = 0;
            shotRaw.damageMin = 4.0;
            shotRaw.damageMax = 13.0;
            shotRaw.range = 80.0f * rangeScale;
            shotRaw.fireRate = 2.0f * 30.0f / 100.0f;
            if (g_game.game == GC_LABYRINTH)
                shotRaw.component[COMPONENT_BLOODBOUND] = 0.18;
            else
                shotRaw.component[COMPONENT_BLOODBOUND] = 0.036;
            break;
        case GEM_CRITICAL:
            HSV = (57 << 16) | (100 << 8) | 100;
            shotRaw.damageMin = 4.0;
            shotRaw.damageMax = 11.0;
            shotRaw.range = 78.0f * rangeScale;
            shotRaw.fireRate = 2.0f * 30.0f / 100.0f;
            if (g_game.game == GC_LABYRINTH)
            {
                shotRaw.component[COMPONENT_CRITICAL_POWER] = 0.14;
            }
            else
            {
                shotRaw.component[COMPONENT_CRITICAL_POWER] = 0.35;
                shotRaw.component[COMPONENT_CRITICAL_CHANCE] = 0.111;
            }
            break;
        case GEM_LEECH:
            HSV = (30 << 16) | (100 << 8) | 100;
            shotRaw.damageMin = 4.0;
            shotRaw.damageMax = 8.0;
            shotRaw.range = 77.0f * rangeScale;
            shotRaw.fireRate = 1.8f * 30.0f / 100.0f;
            if (g_game.game == GC_LABYRINTH)
                shotRaw.component[COMPONENT_LEECH] = 0.26;
            else
                shotRaw.component[COMPONENT_LEECH] = 0.39;
            break;
        case GEM_POOLBOUND:
            HSV = 100;
            shotRaw.damageMin = 4.0;
            shotRaw.damageMax = 12.0;
            shotRaw.range = 76.0f * rangeScale;
            shotRaw.fireRate = 2.2f * 30.0f / 100.0f;
            shotRaw.component[COMPONENT_POOLBOUND] = 1.01;
            break;
        case GEM_SUPPRESSING:
            HSV = (195 << 16) | (100 << 8) | 100;
            shotRaw.damageMin = 3.0;
            shotRaw.damageMax = 12.0;
            shotRaw.range = 75.0f * rangeScale;
            shotRaw.fireRate = 2.1f * 30.0f / 100.0f;
            shotRaw.component[COMPONENT_SUPPRESS] = 13.3;
            break;
        default:
            throw "Invalid Gem Type!\n";
            break;
    }
    if (g_game.game == GC_LABYRINTH)
        HSV = (((((HSV >> 16) + rand() % 6 - 3) + 360) % 360) << 16) | (HSV & 0xFFFF);
    else if (g_game.game == GC_CHASINGSHADOWS)
    {
        shotRaw.damageMin *= 2.0;
        shotRaw.damageMax *= 2.0;
    }
    shotRaw = shotRaw.round();

    RGB = HSVtoRGB(HSV);

    for (int i = 0; i < grade_; ++i)
        combineWith(this);

    recalculateShotData();

#ifdef DEBUG
    printf("Gem: %s | HSV: %x | RGB: %x\n", GEM_COMPONENT_TYPE_NAME[type], HSV, RGB);
#endif
}

Gem::Gem(Gem* pSourceGem) // Duplicate Gem Constructor, not Copy
{
    pBuilding = NULL;
    x = 0.0f;
    y = 0.0f;
    isDragged = false;

    HSV = pSourceGem->HSV;
    RGB = pSourceGem->RGB;

    hits = 0;
    kills = 0;
    killsNonCombined = 0;

    componentMask = pSourceGem->componentMask;
    memcpy(displayComponents, pSourceGem->displayComponents, sizeof(displayComponents));

    grade = pSourceGem->grade;
    manaCost = pSourceGem->manaCost;

    shotRaw = pSourceGem->shotRaw;
    recalculateShotData();
}

#define COMBINE_COMPONENT(r1, r2, comp)                                           \
    shotNew.component[comp] =                                                     \
        (r1)*std::max(shotRaw.component[comp], pOther->shotRaw.component[comp]) + \
        (r2)*std::min(shotRaw.component[comp], pOther->shotRaw.component[comp])

void Gem::combineWith(const Gem* pOther)
{
    const double rand1 = (rand() / double(RAND_MAX));
    const double rand2 = (rand() / double(RAND_MAX));

    ShotData shotNew;

    if (grade == pOther->grade)
    {
        shotNew.damageMin = round(
            rand1 * 1.2 + ceil(0.83 * std::max(shotRaw.damageMin, pOther->shotRaw.damageMin) +
                               0.71 * std::min(shotRaw.damageMin, pOther->shotRaw.damageMin)));
        shotNew.damageMax = round(
            rand2 * 2.4 + ceil(0.87 * std::max(shotRaw.damageMax, pOther->shotRaw.damageMax) +
                               0.71 * std::min(shotRaw.damageMax, pOther->shotRaw.damageMax)));
        shotNew.range = 0.694f * std::max(shotRaw.range, pOther->shotRaw.range) +
                        0.388f * std::min(shotRaw.range, pOther->shotRaw.range);
        shotNew.fireRate = 0.74f * std::max(shotRaw.fireRate, pOther->shotRaw.fireRate) +
                           0.44f * std::min(shotRaw.fireRate, pOther->shotRaw.fireRate);
        if (g_game.game == GC_LABYRINTH)
        {
            COMBINE_COMPONENT(0.78, 0.31, COMPONENT_BLOODBOUND);
            COMBINE_COMPONENT(0.87, 0.38, COMPONENT_SLOW_POWER);
            COMBINE_COMPONENT(0.96, 0.62, COMPONENT_POISON);
            COMBINE_COMPONENT(0.88, 0.50, COMPONENT_CRITICAL_POWER);
            COMBINE_COMPONENT(0.88, 0.50, COMPONENT_CHAIN);
            COMBINE_COMPONENT(0.88, 0.50, COMPONENT_LEECH);
            COMBINE_COMPONENT(0.93, 0.45, COMPONENT_ARMOR);
            COMBINE_COMPONENT(0.87, 0.38, COMPONENT_SHOCK);
        }
        else if (g_game.game == GC_CHASINGSHADOWS)
        {
            COMBINE_COMPONENT(0.78, 0.31, COMPONENT_BLOODBOUND);
            COMBINE_COMPONENT(0.81, 0.35, COMPONENT_SLOW_POWER);
            COMBINE_COMPONENT(0.96, 0.85, COMPONENT_POISON);
            COMBINE_COMPONENT(0.88, 0.50, COMPONENT_CRITICAL_POWER);
            COMBINE_COMPONENT(0.81, 0.35, COMPONENT_CRITICAL_CHANCE);
            COMBINE_COMPONENT(0.88, 0.50, COMPONENT_CHAIN);
            COMBINE_COMPONENT(0.88, 0.50, COMPONENT_LEECH);
            COMBINE_COMPONENT(0.94, 0.69, COMPONENT_ARMOR);
            COMBINE_COMPONENT(0.87, 0.38, COMPONENT_POOLBOUND);
            COMBINE_COMPONENT(0.96, 1.91, COMPONENT_SUPPRESS);
        }
        else
            throw "Game Code Unavailable!";
    }
    else if ((grade == pOther->grade + 1) || (grade == pOther->grade - 1))
    {
        shotNew.damageMin = round(
            rand1 * 0.9 + ceil(0.82 * std::max(shotRaw.damageMin, pOther->shotRaw.damageMin) +
                               0.70 * std::min(shotRaw.damageMin, pOther->shotRaw.damageMin)));
        shotNew.damageMax = round(
            rand2 * 2.0 + ceil(0.86 * std::max(shotRaw.damageMax, pOther->shotRaw.damageMax) +
                               0.70 * std::min(shotRaw.damageMax, pOther->shotRaw.damageMax)));
        shotNew.range = 0.80f * std::max(shotRaw.range, pOther->shotRaw.range) +
                        0.25f * std::min(shotRaw.range, pOther->shotRaw.range);
        shotNew.fireRate = 0.80f * std::max(shotRaw.fireRate, pOther->shotRaw.fireRate) +
                           0.25f * std::min(shotRaw.fireRate, pOther->shotRaw.fireRate);
        if (g_game.game == GC_LABYRINTH)
        {
            COMBINE_COMPONENT(0.79, 0.29, COMPONENT_BLOODBOUND);
            COMBINE_COMPONENT(0.88, 0.35, COMPONENT_SLOW_POWER);
            COMBINE_COMPONENT(0.97, 0.42, COMPONENT_POISON);
            COMBINE_COMPONENT(0.90, 0.45, COMPONENT_CRITICAL_POWER);
            COMBINE_COMPONENT(0.90, 0.47, COMPONENT_CHAIN);
            COMBINE_COMPONENT(0.89, 0.44, COMPONENT_LEECH);
            COMBINE_COMPONENT(0.94, 0.42, COMPONENT_ARMOR);
            COMBINE_COMPONENT(0.89, 0.34, COMPONENT_SHOCK);
        }
        else if (g_game.game == GC_CHASINGSHADOWS)
        {
            COMBINE_COMPONENT(0.79, 0.29, COMPONENT_BLOODBOUND);
            COMBINE_COMPONENT(0.80, 0.28, COMPONENT_SLOW_POWER);
            COMBINE_COMPONENT(0.97, 0.62, COMPONENT_POISON);
            COMBINE_COMPONENT(0.88, 0.44, COMPONENT_CRITICAL_POWER);
            COMBINE_COMPONENT(0.80, 0.28, COMPONENT_CRITICAL_CHANCE);
            COMBINE_COMPONENT(0.90, 0.47, COMPONENT_CHAIN);
            COMBINE_COMPONENT(0.89, 0.44, COMPONENT_LEECH);
            COMBINE_COMPONENT(0.95, 0.57, COMPONENT_ARMOR);
            COMBINE_COMPONENT(0.87, 0.38, COMPONENT_POOLBOUND);
            COMBINE_COMPONENT(0.92, 1.13, COMPONENT_SUPPRESS);
        }
        else
            throw "Game Code Unavailable!";
    }
    else
    {
        shotNew.damageMin = round(
            rand1 * 0.7 + ceil(0.81 * std::max(shotRaw.damageMin, pOther->shotRaw.damageMin) +
                               0.69 * std::min(shotRaw.damageMin, pOther->shotRaw.damageMin)));
        shotNew.damageMax = round(
            rand2 * 1.8 + ceil(0.85 * std::max(shotRaw.damageMax, pOther->shotRaw.damageMax) +
                               0.69 * std::min(shotRaw.damageMax, pOther->shotRaw.damageMax)));
        shotNew.range = 0.92f * std::max(shotRaw.range, pOther->shotRaw.range) +
                        0.09f * std::min(shotRaw.range, pOther->shotRaw.range);
        shotNew.fireRate = 0.92f * std::max(shotRaw.fireRate, pOther->shotRaw.fireRate) +
                           0.09f * std::min(shotRaw.fireRate, pOther->shotRaw.fireRate);
        if (g_game.game == GC_LABYRINTH)
        {
            COMBINE_COMPONENT(0.80, 0.27, COMPONENT_BLOODBOUND);
            COMBINE_COMPONENT(0.89, 0.33, COMPONENT_SLOW_POWER);
            COMBINE_COMPONENT(0.98, 0.22, COMPONENT_POISON);
            COMBINE_COMPONENT(0.92, 0.40, COMPONENT_CRITICAL_POWER);
            COMBINE_COMPONENT(0.92, 0.44, COMPONENT_CHAIN);
            COMBINE_COMPONENT(0.90, 0.38, COMPONENT_LEECH);
            COMBINE_COMPONENT(0.95, 0.39, COMPONENT_ARMOR);
            COMBINE_COMPONENT(0.91, 0.30, COMPONENT_SHOCK);
        }
        else if (g_game.game == GC_CHASINGSHADOWS)
        {
            COMBINE_COMPONENT(0.80, 0.27, COMPONENT_BLOODBOUND);
            COMBINE_COMPONENT(0.79, 0.26, COMPONENT_SLOW_POWER);
            COMBINE_COMPONENT(0.98, 0.42, COMPONENT_POISON);
            COMBINE_COMPONENT(0.88, 0.44, COMPONENT_CRITICAL_POWER);
            COMBINE_COMPONENT(0.79, 0.26, COMPONENT_CRITICAL_CHANCE);
            COMBINE_COMPONENT(0.92, 0.44, COMPONENT_CHAIN);
            COMBINE_COMPONENT(0.90, 0.38, COMPONENT_LEECH);
            COMBINE_COMPONENT(0.96, 0.45, COMPONENT_ARMOR);
            COMBINE_COMPONENT(0.87, 0.38, COMPONENT_POOLBOUND);
            COMBINE_COMPONENT(0.92, 0.73, COMPONENT_SUPPRESS);
        }
        else
            throw "Game Code Unavailable!";
    }
    shotNew.damageMin =
        std::max(shotNew.damageMin, std::max(shotRaw.damageMin, pOther->shotRaw.damageMin));
    shotNew.damageMax =
        std::max(shotNew.damageMax, std::max(shotRaw.damageMax, pOther->shotRaw.damageMax));
    shotNew.range = std::min(shotNew.range, 48.0f);
    shotNew.fireRate = std::min(shotNew.fireRate, 48.0f);

    if (g_game.game == GC_LABYRINTH)
    {
        if (shotNew.component[COMPONENT_BLOODBOUND] < 0.01)
            shotNew.component[COMPONENT_BLOODBOUND] = 0.0;
        if (shotNew.component[COMPONENT_SLOW_POWER] < 0.01)
            shotNew.component[COMPONENT_SLOW_POWER] = 0.0;
        if (shotNew.component[COMPONENT_POISON] < 0.5)
            shotNew.component[COMPONENT_POISON] = 0.0;
        if (shotNew.component[COMPONENT_CRITICAL_POWER] < 0.01)
            shotNew.component[COMPONENT_CRITICAL_POWER] = 0.0;
        if (shotNew.component[COMPONENT_CHAIN] < 0.01)
            shotNew.component[COMPONENT_CHAIN] = 0.0;
        if (shotNew.component[COMPONENT_LEECH] < 0.09)
            shotNew.component[COMPONENT_LEECH] = 0.0;
        if (shotNew.component[COMPONENT_ARMOR] < 0.09)
            shotNew.component[COMPONENT_ARMOR] = 0.0;
        if (shotNew.component[COMPONENT_SHOCK] < 0.01)
            shotNew.component[COMPONENT_SHOCK] = 0.0;
    }
    else
    {
        if (shotNew.component[COMPONENT_BLOODBOUND] < 0.001)
            shotNew.component[COMPONENT_BLOODBOUND] = 0.0;
        if (shotNew.component[COMPONENT_BLOODBOUND] < 0.01)
            shotNew.component[COMPONENT_BLOODBOUND] = 0.0;
        if (shotNew.component[COMPONENT_SLOW_POWER] < 0.01)
            shotNew.component[COMPONENT_SLOW_POWER] = 0.0;
        if (shotNew.component[COMPONENT_POISON] < 0.5)
            shotNew.component[COMPONENT_POISON] = 0.0;
        if ((shotNew.component[COMPONENT_CRITICAL_POWER] < 0.01) ||
            (shotNew.component[COMPONENT_CRITICAL_CHANCE] < 0.01))
            shotNew.component[COMPONENT_CRITICAL_POWER] = 0.0;
        if (shotNew.component[COMPONENT_CHAIN] < 0.01)
            shotNew.component[COMPONENT_CHAIN] = 0.0;
        if (shotNew.component[COMPONENT_LEECH] < 0.09)
            shotNew.component[COMPONENT_LEECH] = 0.0;
        if (shotNew.component[COMPONENT_ARMOR] < 0.09)
            shotNew.component[COMPONENT_ARMOR] = 0.0;
        if (shotNew.component[COMPONENT_POOLBOUND] < 0.001)
            shotNew.component[COMPONENT_POOLBOUND] = 0.0;
        if (shotNew.component[COMPONENT_SUPPRESS] < 0.01)
            shotNew.component[COMPONENT_SUPPRESS] = 0.0;
    }

    shotRaw = shotNew;

    HSV = combineHSV(HSV, pOther->HSV, grade == pOther->grade);
    RGB = HSVtoRGB(HSV);

    grade = (grade == pOther->grade) ? (grade + 1) : std::max(grade, pOther->grade);
    manaCost = floor(gemCombineCostCurrent + manaCost + pOther->manaCost);

    hits += pOther->hits;
    kills += pOther->kills;
    killsNonCombined = 0;

    componentMask |= pOther->componentMask;
    if (displayComponents[1] == GEM_COMPONENT_TYPE_COUNT)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (pOther->displayComponents[i] == GEM_COMPONENT_TYPE_COUNT)
                break;

            if (displayComponents[0] != pOther->displayComponents[i])
            {
                displayComponents[1] = pOther->displayComponents[i];
                break;
            }
        }
    }
    if (displayComponents[2] == GEM_COMPONENT_TYPE_COUNT)
    {
        for (int i = 0; i < 3; ++i)
        {
            if ((displayComponents[0] != pOther->displayComponents[i]) &&
                (displayComponents[1] != pOther->displayComponents[i]))
            {
                displayComponents[2] = pOther->displayComponents[i];
                break;
            }
        }
    }
}

#undef COMBINE_COMPONENT

ShotData Gem::transformShotDataComponents(const ShotData& sd) const
{
    ShotData comp = sd;
    int cc = numComponents();

    if (g_game.game == GC_LABYRINTH)
    {
        if (cc == 2)
        {
            comp.multiply(1.05, 1.05, 1.05, 0.8);
        }
        else if (cc == 3)
        {
            comp.multiply(1.1, 1.1, 1.1, 0.8);
        }
        else if (cc >= 4)
        {
            // Mixing two enums here... fine since GCL's gems all line up with components
            memset(comp.component, 0, sizeof(comp.component));
            for (int i = 0; i < 3; ++i)
                comp.component[displayComponents[i]] = 0.6 * sd.component[displayComponents[i]];
            comp.component[COMPONENT_SLOW_DURATION] = sd.component[COMPONENT_SLOW_DURATION];
        }
    }
    else if (g_game.game == GC_CHASINGSHADOWS)
    {
        static const double multComponents[] = {
            0.0, 1.0, 0.7, 0.5, 0.4, 0.35, 0.30, 0.25, 0.20, 0.20};
        double mc = multComponents[cc];

        if (cc == 1) {}
        else if (cc < 5)
        {
            comp.damageMin *= 1.2;
            comp.damageMax *= 1.2;
        }

        for (int i = 0; i < GEM_COMPONENT_INDEX_COUNT; ++i)
        {
            if (sd.component[i] > 0.0)
                comp.component[i] *= mc;
        }
    }
    else
        throw "Game Code Unavailable!";

    return sd;
}

ShotData Gem::transformShotDataReal(const ShotData& sd) const
{
    ShotData real = sd;

    static constexpr double invlog37 = 1.0 / std::log(3.7);

    if (g_game.game == GC_LABYRINTH)
    {
        if (sd.component[COMPONENT_SLOW_POWER] > 0.0)
            real.component[COMPONENT_SLOW_POWER] = 1.0 / (1.0 + sd.component[COMPONENT_SLOW_POWER]);
    }
    else if (g_game.game == GC_CHASINGSHADOWS)
    {
        // TODO
        double boundMultiplier = 1.0;

        if (sd.component[COMPONENT_SLOW_POWER] > 0.0)
        {
            real.component[COMPONENT_SLOW_POWER] = std::max(0.1,
                1.0 - boundMultiplier *
                          (1.0 - 1.0 / pow(1.0 + sd.component[COMPONENT_SLOW_POWER] / 2.0, 0.37)));
        }

        real.component[COMPONENT_CRITICAL_CHANCE] =
            std::min(0.8, sd.component[COMPONENT_CRITICAL_CHANCE]);

        if (sd.component[COMPONENT_CHAIN] > 0.0)
        {
            real.component[COMPONENT_CHAIN] =
                boundMultiplier * std::log(3.7 + sd.component[COMPONENT_CHAIN]) * invlog37;
            if (real.component[COMPONENT_CHAIN] > 8.0)
                real.component[COMPONENT_CHAIN] =
                    7.0 + pow(real.component[COMPONENT_CHAIN] - 7.0, 0.7);

            if ((pBuilding != NULL) && (pBuilding->type == TILE_TRAP))
            {
                double trapEnchantMultiplier = 1.8;
                real.component[COMPONENT_CHAIN] *= trapEnchantMultiplier;
            }
        }
    }
    else
        throw "Game Code Unavailable!";

    return real;
}

void Gem::recalculateShotData()
{
    ShotData shotComponents = transformShotDataComponents(shotRaw);

    if (pBuilding != NULL)
        shotAmplified = pBuilding->transformShotDataAmplify(shotComponents);
    else
        shotAmplified = shotComponents;

    if (pBuilding != NULL)
        shotFinal = pBuilding->transformShotDataBuilding(shotAmplified);
    else
        shotFinal = shotAmplified;

    shotFinal = transformShotDataReal(shotFinal);
}

double Gem::gemCreateCost(int grade)
{
    double numCreates = floor(pow(2.0, (double)grade));
    return floor(gemCreateCostCurrent * numCreates + gemCombineCostCurrent * (numCreates - 1.0));
}

int Gem::numComponents() const
{
    int count = 0;
    int mask = componentMask;
    while (mask != 0)
    {
        mask &= (mask - 1);
        ++count;
    }
    return count;
}

#ifdef DEBUG
void Gem::debugPrint() const
{
    static const char* color[GEM_COMPONENT_TYPE_COUNT] = {
        "Blue",
        g_game.game == GC_LABYRINTH ? "Lime" : "Red",
        "Green",
        "Purple",
        "Cyan",
        g_game.game == GC_LABYRINTH ? "Red" : "Black",
        "Yellow",
        "Orange",

        "White",
        "Cyan",
    };

    printf("Gem:\n");
    printf("\tGrade: %d | Cost: %f: | %s", grade + 1, manaCost, color[displayComponents[0]]);
    for (int i = 1; i < 3; ++i)
        if (displayComponents[i] != GEM_COMPONENT_TYPE_COUNT)
            printf("-%s", color[displayComponents[i]]);
    printf("\n\t%d Color Components (0x%x)\n", numComponents(), componentMask);
    printf("\tHits: %lu | Kills: %lu | Total Kills: %lu\n", hits, killsNonCombined, kills);
    printf("Raw:\n");
    shotRaw.debugPrint();
    printf("Amp:\n");
    shotAmplified.debugPrint();
    printf("Fin:\n");
    shotFinal.debugPrint();
}
#endif
