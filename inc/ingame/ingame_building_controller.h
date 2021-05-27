#pragma once

#include "entity/amplifier.h"
#include "entity/orb.h"
#include "entity/tower.h"
#include "entity/trap.h"

#include "interface/window.h"

#include <list>

class IngameLevelDefinition;

class IngameBuildingController
{
private:
    std::list<Tower> towers;
    std::list<Trap> traps;
    std::list<Amplifier> amplifiers;
    Orb orb;

    bool amplifierRecalculateRequired;

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

    Tower& addTower(int x, int y);
    Trap& addTrap(int x, int y);
    Amplifier& addAmplifier(int x, int y);

    void destroyBuilding(Building* pBuilding);

    inline void signalAmplifierRecalculateRequired() { amplifierRecalculateRequired = true; }

    Orb& getOrb() { return orb; }
};
