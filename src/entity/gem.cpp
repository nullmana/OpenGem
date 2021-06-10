#include "entity/gem.h"
#include "entity/building.h"

#include "constants/game_header.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

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

static uint32_t combineHSV(uint32_t a, uint32_t b)
{
    int H, S, V;

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
            S = std::max<int>((a >> 8) & 0xFF, (b >> 8) & 0xFF);
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

    switch (type)
    {
        case GEM_SLOW:
            HSV = (250 << 16) | (100 << 8) | 100;
            shotRaw.damageMin = 5.0;
            shotRaw.damageMax = 12.0;
            shotRaw.range = 77.0f * rangeScale;
            shotRaw.fireRate = 2.1f * 30.0f / 100.0f;
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
            break;
        case GEM_POISON:
            HSV = (127 << 16) | (100 << 8) | 100;
            shotRaw.damageMin = 5.0;
            shotRaw.damageMax = 10.0;
            shotRaw.range = 79.0f * rangeScale;
            shotRaw.fireRate = 2.1f * 30.0f / 100.0f;
            break;
        case GEM_ARMOR:
            HSV = (300 << 16) | (100 << 8) | 100;
            shotRaw.damageMin = 3.0;
            shotRaw.damageMax = 11.0;
            shotRaw.range = 73.0f * rangeScale;
            shotRaw.fireRate = 2.2f * 30.0f / 100.0f;
            break;
        case GEM_SHOCK:
            HSV = (195 << 16) | (100 << 8) | 100;
            shotRaw.damageMin = 4.0;
            shotRaw.damageMax = 10.0;
            shotRaw.range = 75.0f * rangeScale;
            shotRaw.fireRate = 2.2f * 30.0f / 100.0f;
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
            break;
        case GEM_CRITICAL:
            HSV = (57 << 16) | (100 << 8) | 100;
            shotRaw.damageMin = 4.0;
            shotRaw.damageMax = 11.0;
            shotRaw.range = 78.0f * rangeScale;
            shotRaw.fireRate = 2.0f * 30.0f / 100.0f;
            break;
        case GEM_LEECH:
            HSV = (30 << 16) | (100 << 8) | 100;
            shotRaw.damageMin = 4.0;
            shotRaw.damageMax = 8.0;
            shotRaw.range = 77.0f * rangeScale;
            shotRaw.fireRate = 1.8f * 30.0f / 100.0f;
            break;
        case GEM_POOLBOUND:
            HSV = 100;
            shotRaw.damageMin = 4.0;
            shotRaw.damageMax = 12.0;
            shotRaw.range = 76.0f * rangeScale;
            shotRaw.fireRate = 2.2f * 30.0f / 100.0f;
            break;
        case GEM_SUPPRESSING:
            HSV = (195 << 16) | (100 << 8) | 100;
            shotRaw.damageMin = 3.0;
            shotRaw.damageMax = 12.0;
            shotRaw.range = 75.0f * rangeScale;
            shotRaw.fireRate = 2.1f * 30.0f / 100.0f;
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

    RGB = pSourceGem->RGB;
    HSV = pSourceGem->HSV;

    grade = pSourceGem->grade;
    manaCost = pSourceGem->manaCost;

    shotRaw = pSourceGem->shotRaw;
    recalculateShotData();
}

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
    }
    shotNew.damageMin =
        std::max(shotNew.damageMin, std::max(shotRaw.damageMin, pOther->shotRaw.damageMin));
    shotNew.damageMax =
        std::max(shotNew.damageMax, std::max(shotRaw.damageMax, pOther->shotRaw.damageMax));
    shotNew.range = std::min(shotNew.range, 48.0f);
    shotNew.fireRate = std::min(shotNew.fireRate, 160.0f);

    shotRaw = shotNew;

    grade = (grade == pOther->grade) ? (grade + 1) : std::max(grade, pOther->grade);
    manaCost = floor(gemCombineCostCurrent + manaCost + pOther->manaCost);

    if (HSV == pOther->HSV)
    {
        HSV = pOther->HSV;
        RGB = pOther->RGB;
    }
    else
    {
        HSV = combineHSV(HSV, pOther->HSV);
        RGB = HSVtoRGB(HSV);
    }
}

ShotData Gem::transformShotDataComponents(const ShotData& sd) const
{
    // TODO
    return sd;
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
}

double Gem::gemCreateCost(int grade)
{
    double numCreates = floor(pow(2.0, (double)grade));
    return floor(gemCreateCostCurrent * numCreates + gemCombineCostCurrent * (numCreates - 1.0));
}

#ifdef DEBUG
void Gem::debugPrint() const
{
    printf("Gem:\n");
    printf("\tGrade: %d | Cost: %f:\n", grade + 1, manaCost);
    printf("Raw:\t");
    shotRaw.debugPrint();
    printf("Amp:\t");
    shotAmplified.debugPrint();
    printf("Fin:\t");
    shotFinal.debugPrint();
}
#endif
