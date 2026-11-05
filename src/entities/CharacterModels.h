#pragma once
// ====================================================================
// FILE:     src/entities/CharacterModels.h
// OWNER:    Member 2
// TASKS:    M2-03 to M2-08
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Builds the six distinct character models from primitives. Each
//   model has a clearly different shape, silhouette and equipment, not
//   just different colours (US-28, US-38).
//
// RESPONSIBILITIES:
//   - Melee Player  (Kai,  Fire Ninja)   - sword, lean fighter   (US-25)
//   - Ranged Player (Zane, Ice Ninja)    - shuriken, slim thrower (US-26)
//   - Defense Player(Cole, Earth Ninja)  - big shield, bulky wall (US-27)
//   - Easy enemy    (Venomari, upper ranged, spits from above)    (US-35)
//   - Medium enemy  (Hypnobrai, lower ranged, staff sweep low)    (US-36)
//   - Hard enemy    (Constrictai, melee brute with spiked mace)   (US-37)
//   - Everything built from multiple parts, never a single primitive
//
// PART NAMES (shared contract with Animation.cpp):
//   body, chest, head, hood, armL, armR, forearmL, forearmR,
//   legL, legR, weapon, shield, tail, staff, fistL, fistR
//
// INTERACTS WITH: HierarchicalModel, Player, Enemy, Cutscene
// ====================================================================

#include "Entity.h"              // UnitType
#include "HierarchicalModel.h"

namespace CharacterModels {

    HierarchicalModel buildMeleePlayer();    // Kai   - Fire Ninja
    HierarchicalModel buildRangedPlayer();   // Zane  - Ice Ninja
    HierarchicalModel buildDefensePlayer();  // Cole  - Earth Ninja
    HierarchicalModel buildEasyEnemy();      // Venomari   - upper ranged
    HierarchicalModel buildMediumEnemy();    // Hypnobrai  - lower ranged
    HierarchicalModel buildHardEnemy();      // Constrictai - melee brute

    // Build the model that belongs to a unit type (used by Player/Enemy).
    HierarchicalModel buildForType(UnitType type);

}
