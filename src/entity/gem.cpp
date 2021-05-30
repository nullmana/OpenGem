#include "entity/gem.h"

double Gem::gemCreateCostCurrent = 0;
double Gem::gemCombineCostCurrent = 0;

Gem::Gem(int grade_, uint32_t color_)
{
    pBuilding = NULL;
    x = 0.0f;
    y = 0.0f;
    isDragged = false;

    color = color_;
    grade = grade_;
    manaCost = gemCreateCost(grade_);
}

Gem::Gem(Gem* pSourceGem) // Duplicate Gem Constructor, not Copy
{
    pBuilding = NULL;
    x = 0.0f;
    y = 0.0f;
    isDragged = false;

    color = pSourceGem->color;
    grade = pSourceGem->grade;
    manaCost = pSourceGem->manaCost;
}

double Gem::gemCreateCost(int grade)
{
    double numCreates = floor(pow(2.0, (double)grade));
    return floor(gemCreateCostCurrent * numCreates + gemCombineCostCurrent * (numCreates - 1.0));
}
