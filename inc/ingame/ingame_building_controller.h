#pragma once

#include "entity/amplifier.h"
#include "entity/orb.h"
#include "entity/tower.h"
#include "entity/trap.h"

#include "entity/gcl/shrine_charged_bolts.h"
#include "entity/gcl/shrine_lightning.h"

#include "interface/window.h"

#include <list>
#include <unordered_set>

class IngameLevelDefinition;
class IngameManaPool;
class Targetable;

class IngameBuildingController
{
private:
    std::list<Tower> towers;
    std::list<Trap> traps;
    std::list<Amplifier> amplifiers;
    Orb orb;

    // Shrines : GCL
    std::list<ShrineChargedBolts> shrinesCB;
    std::list<ShrineLightning> shrinesLI;
    // Shrines : GCCS

    double wallCostCurrent;
    double towerCostCurrent;
    double trapCostCurrent;
    double amplifierCostCurrent;
    double shrineCostCurrent;

    void destroyTower(Tower* pTower);
    void destroyTrap(Trap* pTrap);
    void destroyAmplifier(Amplifier* pAmplifier);

    void linkAmplifier(Amplifier* pAmplifier, Amplifiable* pAmplified);
    void removeBuildingAmplifiers(Building* pBuilding);
    std::vector<Amplifier*> getAdjacentAmplifiers(Amplifiable* pBuilding);
    std::vector<Amplifiable*> getAdjacentAmplifiableBuildings(Building* pBuilding);

public:
    IngameBuildingController(const IngameLevelDefinition& level);

    void render(struct _fbg* pFbg, const Window& window) const;

    void tickBuildings(IngameMap& map, int frames);

    bool hasBuildMana(const IngameManaPool& manaPool, TILE_TYPE building, int num) const;
    void spendBuildMana(IngameManaPool& manaPool, TILE_TYPE building, int num);

    Tower& addTower(int x, int y);
    Trap& addTrap(int x, int y);
    Amplifier& addAmplifier(int x, int y);

    Shrine* addShrine(IngameMap& map, int x, int y, SHRINE_TYPE type);

    void destroyBuilding(Building* pBuilding);

    Orb& getOrb() { return orb; }
};
