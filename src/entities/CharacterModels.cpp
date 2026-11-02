#include "CharacterModels.h"
// ====================================================================
// FILE:     src/entities/CharacterModels.cpp
// OWNER:    Member 2
// TASKS:    M2-03 to M2-08
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Builds the six distinct character models from primitives. Each model
//   has a clearly different shape, silhouette and equipment - not just a
//   different colour (US-28 for players, US-38 for enemies).
//
// THEME (Ninjago - the Serpentine invasion, see the team document):
//   Players  (ninja, humanoid, standing upright, two legs):
//     Melee   - Kai,  Fire Ninja   : red gi, gold sash, long sword
//     Ranged  - Zane, Ice Ninja    : white/ice-blue, slim, throwing shuriken
//     Defense - Cole, Earth Ninja  : dark bulky body, huge tower shield
//   Enemies  (Serpentine, snake bodies, tail instead of legs, cobra hoods):
//     Easy    - Venomari    : tall, huge hood, spits venom from above
//     Medium  - Hypnobrai   : hunched, holds a staff low to the ground
//     Hard    - Constrictai : massive armoured brute with fists and horns
//
// INTERACTS WITH: HierarchicalModel, Player, Enemy, Cutscene
// ====================================================================

namespace {

    // ------------------------------------------------------- shape helpers
    PartShapeDef rect(float w, float h, const Vec4& c) {
        PartShapeDef d; d.shape = PartShape::Rect; d.size = { w, h }; d.colour = c; return d;
    }
    PartShapeDef circle(float diameter, const Vec4& c) {
        PartShapeDef d; d.shape = PartShape::Circle; d.size = { diameter, diameter }; d.colour = c; return d;
    }
    PartShapeDef tri(float w, float h, const Vec4& c) {
        PartShapeDef d; d.shape = PartShape::Triangle; d.size = { w, h }; d.colour = c; return d;
    }
    PartShapeDef trap(float w, float h, float topScale, const Vec4& c) {
        PartShapeDef d; d.shape = PartShape::Trapezoid; d.size = { w, h };
        d.topWidthScale = topScale; d.colour = c; return d;
    }
    PartShapeDef blade(float w, float h, const Vec4& c) {
        PartShapeDef d; d.shape = PartShape::Blade; d.size = { w, h }; d.colour = c; return d;
    }

    // ------------------------------------------------------------ palette
    const Vec4 BLACK      { 0.10f, 0.10f, 0.12f, 1.0f };
    const Vec4 SKIN       { 0.98f, 0.80f, 0.62f, 1.0f };
    const Vec4 STEEL      { 0.78f, 0.82f, 0.88f, 1.0f };
    const Vec4 GOLD       { 0.95f, 0.76f, 0.20f, 1.0f };
    const Vec4 WOOD       { 0.42f, 0.28f, 0.16f, 1.0f };

    // Kai - Fire Ninja (Melee)
    const Vec4 KAI_GI     { 0.80f, 0.15f, 0.12f, 1.0f };
    const Vec4 KAI_DARK   { 0.32f, 0.07f, 0.06f, 1.0f };

    // Zane - Ice Ninja (Ranged)
    const Vec4 ZANE_GI    { 0.88f, 0.93f, 0.97f, 1.0f };
    const Vec4 ZANE_ICE   { 0.38f, 0.70f, 0.92f, 1.0f };
    const Vec4 ZANE_DARK  { 0.22f, 0.38f, 0.52f, 1.0f };

    // Cole - Earth Ninja (Defense)
    const Vec4 COLE_GI    { 0.24f, 0.26f, 0.30f, 1.0f };
    const Vec4 COLE_STONE { 0.46f, 0.44f, 0.42f, 1.0f };
    const Vec4 COLE_DARK  { 0.16f, 0.15f, 0.14f, 1.0f };
    const Vec4 COLE_GOLD  { 0.72f, 0.60f, 0.28f, 1.0f };

    // Venomari (Easy enemy - upper ranged)
    const Vec4 VENOM_GREEN{ 0.42f, 0.68f, 0.20f, 1.0f };
    const Vec4 VENOM_DARK { 0.24f, 0.44f, 0.12f, 1.0f };
    const Vec4 VENOM_LIGHT{ 0.72f, 0.90f, 0.42f, 1.0f };
    const Vec4 VENOM_EYE  { 0.95f, 0.85f, 0.15f, 1.0f };

    // Hypnobrai (Medium enemy - lower ranged)
    const Vec4 HYP_BLUE   { 0.20f, 0.62f, 0.66f, 1.0f };
    const Vec4 HYP_DARK   { 0.10f, 0.36f, 0.42f, 1.0f };
    const Vec4 HYP_LIGHT  { 0.62f, 0.90f, 0.90f, 1.0f };
    const Vec4 HYP_EYE    { 0.95f, 0.30f, 0.25f, 1.0f };

    // Constrictai (Hard enemy - melee brute)
    const Vec4 CONST_MID  { 0.36f, 0.28f, 0.24f, 1.0f };
    const Vec4 CONST_DARK { 0.20f, 0.15f, 0.13f, 1.0f };
    const Vec4 CONST_ORNG { 0.95f, 0.48f, 0.18f, 1.0f };
    const Vec4 CONST_BONE { 0.88f, 0.84f, 0.74f, 1.0f };

} // anonymous namespace

namespace CharacterModels {

// ====================================================================
// M2-03 / US-25 / US-28 : Melee player - Kai, the Fire Ninja
// Silhouette: lean upright ninja, sword held forward in the right hand.
// ====================================================================
HierarchicalModel buildMeleePlayer() {
    HierarchicalModel m;
    m.setSize({ 42.0f, 74.0f });

    // --- legs (attached straight to the root so the body can lean) ---
    m.addPart("legL", "", rect(12, 30, KAI_DARK), { -8, 16 });
    m.addPart("legR", "", rect(12, 30, KAI_DARK), {  8, 16 });
    m.addPart("footL", "legL", rect(15, 7, BLACK), { 2, -14 });
    m.addPart("footR", "legR", rect(15, 7, BLACK), { 2, -14 });

    // --- torso: trapezoid chest, gold sash, flowing back sash ---
    m.addPart("body", "", trap(26, 30, 0.85f, KAI_GI), { 0, 45 });
    m.addPart("belt", "body", rect(26, 6, GOLD), { 0, -14 });
    m.addPart("sash", "body", rect(7, 26, KAI_DARK), { -15, 2 }, radians(15));

    // --- arms: left arm back, right arm forward and holding the sword ---
    m.addPart("armL", "body", rect(10, 24, KAI_GI), { -14, 6 },  radians(20));
    m.addPart("armR", "body", rect(10, 24, KAI_GI), {  14, 6 }, -radians(32));
    m.addPart("handR", "armR", circle(9, SKIN), { 0, -11 });

    // --- the sword lives in the hand, so it swings with the arm (US-25) ---
    m.addPart("weapon", "handR", rect(5, 16, WOOD), { 0, -2 });
    m.addPart("guard",  "weapon", rect(18, 5, GOLD), { 0, 5 });
    m.addPart("blade",  "weapon", blade(9, 38, STEEL), { 0, 26 });

    // --- hood drawn behind the head, then the head and its mask ---
    m.addPart("hood", "body", trap(30, 24, 0.35f, KAI_GI), { 0, 20 });
    m.addPart("head", "body", circle(18, SKIN), { 0, 20 });
    m.addPart("mask", "head", rect(19, 8, KAI_DARK), { 0, 2 });
    m.addPart("eyeL", "head", rect(4, 4, BLACK), { -5, 3 });
    m.addPart("eyeR", "head", rect(4, 4, BLACK), {  5, 3 });

    return m;
}

// ====================================================================
// M2-04 / US-26 / US-28 : Ranged player - Zane, the Ice Ninja
// Silhouette: slim, taller thrower pose, quiver on the back and a
// four-bladed shuriken in the right hand.
// ====================================================================
HierarchicalModel buildRangedPlayer() {
    HierarchicalModel m;
    m.setSize({ 40.0f, 74.0f });

    // --- slim legs ---
    m.addPart("legL", "", rect(10, 32, ZANE_DARK), { -7, 16 });
    m.addPart("legR", "", rect(10, 32, ZANE_DARK), {  7, 16 });
    m.addPart("footL", "legL", rect(14, 7, ZANE_ICE), { 2, -15 });
    m.addPart("footR", "legR", rect(14, 7, ZANE_ICE), { 2, -15 });

    // --- narrow torso, quiver on the back ---
    m.addPart("body", "", trap(22, 32, 0.80f, ZANE_GI), { 0, 46 });
    m.addPart("belt", "body", rect(22, 5, ZANE_ICE), { 0, -15 });
    m.addPart("quiver", "body", rect(10, 26, ZANE_DARK), { -13, 3 }, radians(20));
    m.addPart("pauldron", "body", circle(20, ZANE_DARK), { 12, 13 });

    // --- throwing pose: left arm back high, right arm forward ---
    m.addPart("armL", "body", rect(9, 24, ZANE_GI), { -12, 6 }, -radians(38));
    m.addPart("armR", "body", rect(9, 24, ZANE_GI), {  12, 2 },  radians(30));
    m.addPart("handR", "armR", circle(8, SKIN), { 0, -11 });

    // --- shuriken: a hub plus four blades (US-26, US-54) ---
    m.addPart("weapon", "handR", rect(7, 7, ZANE_DARK), { 0, -2 });
    m.addPart("starN", "weapon", tri(11, 13, STEEL), {  0,  8 });
    m.addPart("starS", "weapon", tri(11, 13, STEEL), {  0, -8 }, radians(180));
    m.addPart("starE", "weapon", tri(11, 13, STEEL), {  8,  0 }, radians(-90));
    m.addPart("starW", "weapon", tri(11, 13, STEEL), { -8,  0 }, radians(90));

    // --- hood + head ---
    m.addPart("hood", "body", trap(28, 22, 0.40f, ZANE_GI), { 0, 18 });
    m.addPart("head", "body", circle(17, SKIN), { 0, 18 });
    m.addPart("mask", "head", rect(18, 7, ZANE_ICE), { 0, 2 });
    m.addPart("eyeL", "head", rect(4, 3, BLACK), { -5, 3 });
    m.addPart("eyeR", "head", rect(4, 3, BLACK), {  5, 3 });

    return m;
}

// ====================================================================
// M2-05 / US-27 / US-28 : Defense player - Cole, the Earth Ninja
// Silhouette: short and very wide, wide stance, huge tower shield held
// in front, heavy stone shoulder pads. The wall of the team.
// ====================================================================
HierarchicalModel buildDefensePlayer() {
    HierarchicalModel m;
    m.setSize({ 54.0f, 66.0f });

    // --- wide, planted stance (this unit never moves) ---
    m.addPart("legL", "", rect(16, 22, COLE_DARK), { -12, 11 });
    m.addPart("legR", "", rect(16, 22, COLE_DARK), {  12, 11 });
    m.addPart("footL", "legL", rect(20, 8, COLE_DARK), { 0, -10 });
    m.addPart("footR", "legR", rect(20, 8, COLE_DARK), { 0, -10 });

    // --- broad torso with a stone chest plate ---
    m.addPart("body", "", trap(40, 26, 0.82f, COLE_GI), { 0, 40 });
    m.addPart("chest", "body", rect(34, 14, COLE_STONE), { 0, 4 });
    m.addPart("belt", "body", rect(40, 6, COLE_GOLD), { 0, -12 });

    // --- heavy shoulder pads ---
    m.addPart("shoulderL", "body", circle(24, COLE_STONE), { -22, 8 });
    m.addPart("shoulderR", "body", circle(24, COLE_STONE), {  22, 8 });

    // --- arms ---
    m.addPart("armL", "body", rect(12, 22, COLE_GI), { -16, -14 }, radians(10));
    m.addPart("armR", "body", rect(12, 22, COLE_GI), {  16, -14 }, -radians(10));

    // --- the tower shield in front (rim drawn first, face and boss on top) ---
    m.addPart("shieldRim",  "armR", circle(44, COLE_DARK),  { 4, -16 });
    m.addPart("shield",     "armR", circle(36, COLE_STONE), { 4, -16 });
    m.addPart("shieldBoss", "shield", circle(12, COLE_GOLD), { 0, 0 });

    // --- hood + head with a stone helmet band ---
    m.addPart("hood", "body", trap(32, 20, 0.40f, COLE_GI), { 0, 17 });
    m.addPart("head", "body", circle(18, SKIN), { 0, 17 });
    m.addPart("helmet", "head", rect(24, 8, COLE_STONE), { 0, 4 });
    m.addPart("eyeL", "head", rect(4, 4, BLACK), { -5, 0 });
    m.addPart("eyeR", "head", rect(4, 4, BLACK), {  5, 0 });

    return m;
}

// ====================================================================
// M2-06 / US-35 / US-38 : Easy enemy - Venomari (upper ranged)
// Tall serpent, huge cobra hood, mouth held high above the player so
// the player must duck (the attack comes from above).
// ====================================================================
HierarchicalModel buildEasyEnemy() {
    HierarchicalModel m;
    m.setSize({ 44.0f, 88.0f });

    // --- serpent base: tail instead of legs ---
    m.addPart("tailTip",  "", trap(26, 14, 0.30f, VENOM_DARK), { -26, 7 }, radians(-90));
    m.addPart("tailBase", "", rect(40, 12, VENOM_DARK), { 0, 7 });
    m.addPart("coil",     "", trap(32, 20, 0.75f, VENOM_GREEN), { 0, 21 });

    // --- long upright body with belly plates ---
    m.addPart("body", "", rect(22, 44, VENOM_GREEN), { 0, 52 });
    m.addPart("belly1", "body", rect(14, 6, VENOM_LIGHT), { 0, -16 });
    m.addPart("belly2", "body", rect(14, 6, VENOM_LIGHT), { 0, -8 });
    m.addPart("belly3", "body", rect(14, 6, VENOM_LIGHT), { 0, 0 });

    // --- small clawed arms ---
    m.addPart("armL", "body", rect(8, 22, VENOM_GREEN), { -13, -2 },  radians(22));
    m.addPart("armR", "body", rect(8, 22, VENOM_GREEN), {  13, -2 }, -radians(22));
    m.addPart("clawL", "armL", tri(10, 10, VENOM_LIGHT), { 0, -12 });
    m.addPart("clawR", "armR", tri(10, 10, VENOM_LIGHT), { 0, -12 });

    // --- huge cobra hood with the head high above (the "from above" tell) ---
    m.addPart("hood", "body", trap(46, 34, 0.32f, VENOM_DARK), { 0, 28 });
    m.addPart("head", "body", rect(24, 16, VENOM_GREEN), { 2, 26 });
    m.addPart("snout", "head", rect(18, 11, VENOM_GREEN), { 14, -1 });
    m.addPart("fangL", "snout", tri(6, 11, VENOM_LIGHT), { -4, -8 }, radians(180));
    m.addPart("fangR", "snout", tri(6, 11, VENOM_LIGHT), {  4, -8 }, radians(180));
    m.addPart("eyeL", "head", circle(7, VENOM_EYE), { -2, 3 });
    m.addPart("eyeR", "head", circle(7, VENOM_EYE), {  7, 3 });

    return m;
}

// ====================================================================
// M2-07 / US-36 / US-38 : Medium enemy - Hypnobrai (lower ranged)
// Shorter and hunched, holding a crystal-tipped staff that sweeps low
// along the ground, so the player must jump.
// ====================================================================
HierarchicalModel buildMediumEnemy() {
    HierarchicalModel m;
    m.setSize({ 46.0f, 70.0f });

    // --- serpent base ---
    m.addPart("tailTip",  "", trap(28, 14, 0.28f, HYP_DARK), { -28, 7 }, radians(-90));
    m.addPart("tailBase", "", rect(42, 12, HYP_DARK), { 0, 7 });
    m.addPart("coil",     "", trap(34, 20, 0.75f, HYP_BLUE), { 0, 20 });

    // --- shorter body, leaning forward (hunched) ---
    m.addPart("body", "", rect(24, 30, HYP_BLUE), { 0, 42 }, radians(-8));
    m.addPart("belly1", "body", rect(14, 6, HYP_LIGHT), { 0, -8 });
    m.addPart("belly2", "body", rect(14, 6, HYP_LIGHT), { 0, 0 });

    // --- arms: the right one grips the staff ---
    m.addPart("armL", "body", rect(8, 20, HYP_BLUE), { -13, -4 },  radians(28));
    m.addPart("armR", "body", rect(8, 20, HYP_BLUE), {  13, -4 }, -radians(52));
    m.addPart("handR", "armR", circle(8, HYP_BLUE), { 0, -10 });

    // --- staff angled to point down and forward: the low attack (US-36) ---
    m.addPart("staff",    "handR", rect(6, 76, WOOD), { 0, -34 }, radians(64));
    m.addPart("staffTip", "staff", circle(13, HYP_EYE), { 0, -38 });
    m.addPart("staffGem", "staff", circle(7, HYP_LIGHT), { 0, -38 });

    // --- hood + head ---
    m.addPart("hood", "body", trap(34, 22, 0.45f, HYP_DARK), { 0, 18 });
    m.addPart("head", "body", rect(24, 14, HYP_BLUE), { 0, 20 });
    m.addPart("snout", "head", rect(14, 10, HYP_BLUE), { 12, 0 });
    m.addPart("eyeL", "head", circle(6, HYP_EYE), { -2, 3 });
    m.addPart("eyeR", "head", circle(6, HYP_EYE), {  6, 3 });

    return m;
}

// ====================================================================
// M2-08 / US-37 / US-38 : Hard enemy - Constrictai (melee brute)
// The biggest model: short thick legs, enormous armoured torso, huge
// fists and horns. Runs fastest and hits for 2.
// ====================================================================
HierarchicalModel buildHardEnemy() {
    HierarchicalModel m;
    m.setSize({ 66.0f, 78.0f });

    // --- short, thick legs (a brute barely needs them) ---
    m.addPart("legL", "", rect(18, 24, CONST_DARK), { -14, 12 });
    m.addPart("legR", "", rect(18, 24, CONST_DARK), {  14, 12 });
    m.addPart("footL", "legL", rect(24, 9, CONST_DARK), { 0, -12 });
    m.addPart("footR", "legR", rect(24, 9, CONST_DARK), { 0, -12 });

    // --- massive torso with an orange chest plate ---
    m.addPart("body", "", trap(46, 36, 0.85f, CONST_MID), { 0, 42 });
    m.addPart("chest", "body", rect(36, 16, CONST_ORNG), { 0, 6 });
    m.addPart("belt", "body", rect(46, 7, CONST_DARK), { 0, -16 });
    m.addPart("spikeL", "body", tri(14, 16, CONST_BONE), { -14, 20 });
    m.addPart("spikeR", "body", tri(14, 16, CONST_BONE), {  14, 20 });

    // --- shoulders and huge fists (the melee weapon) ---
    m.addPart("shoulderL", "body", circle(26, CONST_DARK), { -24, 7 });
    m.addPart("shoulderR", "body", circle(26, CONST_DARK), {  24, 7 });
    m.addPart("armL", "body", rect(18, 28, CONST_MID), { -26, -6 },  radians(14));
    m.addPart("armR", "body", rect(18, 28, CONST_MID), {  26, -6 }, -radians(14));
    m.addPart("fistL", "armL", circle(24, CONST_DARK), { 0, -16 });
    m.addPart("fistR", "armR", circle(24, CONST_DARK), { 0, -16 });

    // --- low-set head with horns, jaws and glowing eyes ---
    m.addPart("head", "body", rect(30, 20, CONST_DARK), { 0, 20 });
    m.addPart("hornL", "head", tri(12, 18, CONST_BONE), { -10, 10 });
    m.addPart("hornR", "head", tri(12, 18, CONST_BONE), {  10, 10 });
    m.addPart("jaw", "head", rect(26, 9, CONST_MID), { 2, -12 });
    m.addPart("eyeL", "head", circle(6, CONST_ORNG), { -6, 2 });
    m.addPart("eyeR", "head", circle(6, CONST_ORNG), {  6, 2 });

    return m;
}

// ====================================================================
// One entry point Player/Enemy use so they never write a switch twice.
// ====================================================================
HierarchicalModel buildForType(UnitType type) {
    switch (type) {
        case UnitType::MeleePlayer:   return buildMeleePlayer();
        case UnitType::RangedPlayer:  return buildRangedPlayer();
        case UnitType::DefensePlayer: return buildDefensePlayer();
        case UnitType::EasyEnemy:     return buildEasyEnemy();
        case UnitType::MediumEnemy:   return buildMediumEnemy();
        case UnitType::HardEnemy:     return buildHardEnemy();
        default:                      return buildMeleePlayer();
    }
}

} // namespace CharacterModels
