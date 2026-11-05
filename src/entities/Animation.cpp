#include "Animation.h"
// ====================================================================
// FILE:     src/entities/Animation.cpp
// OWNER:    Member 2
// TASKS:    M2-09, M2-10, M2-12
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Time-driven poses for walk, run, idle, attack swing, jump and duck.
//
// DESIGN NOTE:
//   Poses are written straight onto the HierarchicalModel with setPose().
//   setPose() on a part that does not exist is a harmless no-op, so the
//   same functions animate a two-legged ninja and a tailed serpent.
//   A pose is a Transform2D applied *after* the part's rest pose, i.e.
//   it is a delta on top of how the model was built.
//
// INTERACTS WITH: HierarchicalModel, Player, Enemy, Cutscene
// ====================================================================

#include <cmath>

namespace {

    // ---- tiny pose factories (keeps the code below readable) ----------
    inline Transform2D shift(float x, float y)              { return Transform2D({ x, y }, 0.0f, { 1.0f, 1.0f }); }
    inline Transform2D turn(float radians_)                 { return Transform2D({ 0, 0 }, radians_, { 1.0f, 1.0f }); }
    inline Transform2D shiftTurn(float x, float y, float r) { return Transform2D({ x, y }, r, { 1.0f, 1.0f }); }

    inline float lerp01(float t) { return t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t); }

    // The serpents have no legs: they sway their tail/coil instead.  If
    // the model has neither part this does nothing.
    void swayTail(HierarchicalModel& m, float t, float amount) {
        float s = std::sin(t * 3.0f);
        if (m.hasPart("tailTip"))  m.setPose("tailTip",  turn(s * 0.18f * amount));
        if (m.hasPart("tailBase")) m.setPose("tailBase", shiftTurn(0.0f, s * 1.2f, s * 0.06f * amount));
        if (m.hasPart("coil"))     m.setPose("coil",     shiftTurn(s * 1.5f, 0.0f, s * 0.05f * amount));
    }

    // Zane's shuriken spins the whole time; nobody else's weapon does.
    void spinGear(HierarchicalModel& m, float t) {
        if (m.hasPart("starN")) m.setPose("weapon", turn(t * 7.0f));
    }

    // Basic two-legged walk shared by players and the Hard enemy.
    void stride(HierarchicalModel& m, float t, float amp, float bob) {
        float s = std::sin(t);
        if (m.hasPart("legL")) m.setPose("legL", turn(-s * amp));
        if (m.hasPart("legR")) m.setPose("legR", turn( s * amp));
        // Arm swing; the arm holding the weapon swings slightly less.
        if (m.hasPart("armL")) m.setPose("armL", turn( s * amp * 0.65f));
        if (m.hasPart("armR")) m.setPose("armR", turn(-s * amp * 0.45f));
        // Body bobs up on each footfall.
        if (m.hasPart("body")) m.setPose("body", shift(0.0f, std::fabs(s) * bob));
    }

    // Which "swing shape" a model uses, decided by the equipment it has.
    //   0 = nothing, 1 = sword (Kai), 2 = shuriken (Zane),
    //   3 = staff (Hypnobrai), 4 = fist (Constrictai), 5 = venom spit (Venomari)
    int swingKind(const HierarchicalModel& m) {
        if (m.hasPart("blade"))  return 1;
        if (m.hasPart("starN"))  return 2;
        if (m.hasPart("staff"))  return 3;
        if (m.hasPart("fistR"))  return 4;
        if (m.hasPart("fangL"))  return 5;
        return 0;
    }

    // 0..1 ease-in-out, used to smooth swings.
    inline float ease(float t) {
        t = lerp01(t);
        return t * t * (3.0f - 2.0f * t);
    }

} // anonymous namespace

namespace Animation {

// --------------------------------------------------------------------
// IDLE - standing still (e.g. Cole the Defense player, US-27).
// --------------------------------------------------------------------
void updateIdle(HierarchicalModel& m, float t) {
    float b = std::sin(t * 2.0f);          // slow breathing
    if (m.hasPart("body")) m.setPose("body", shift(0.0f, b * 0.8f));
    if (m.hasPart("head")) m.setPose("head", turn(b * 0.03f));
    if (m.hasPart("armL")) m.setPose("armL", turn(b * 0.05f));
    if (m.hasPart("armR")) m.setPose("armR", turn(-b * 0.05f));
    swayTail(m, t, 0.6f);
    spinGear(m, t);
}

// --------------------------------------------------------------------
// WALK - advancing along the lane (US-25, US-26).
// --------------------------------------------------------------------
void updateWalk(HierarchicalModel& m, float t, float speedScale) {
    float phase = t * 6.0f * speedScale;   // 6 rad/s ~ a little under 1 Hz
    stride(m, phase, 0.45f, 1.6f);
    swayTail(m, phase, 1.0f);
    spinGear(m, t);
}

// --------------------------------------------------------------------
// RUN - the Hard enemy charges (US-37).  Faster, wider and lower.
// --------------------------------------------------------------------
void updateRun(HierarchicalModel& m, float t, float speedScale) {
    float phase = t * 9.5f * speedScale;
    stride(m, phase, 0.72f, 2.6f);
    if (m.hasPart("body")) m.setPose("body", shiftTurn(0.0f, std::fabs(std::sin(phase)) * 2.6f, 0.12f));
    swayTail(m, phase, 1.4f);
    spinGear(m, t);
}

// --------------------------------------------------------------------
// ATTACK - one full swing over phase 0..1 (US-25 melee / US-26 ranged).
// --------------------------------------------------------------------
void updateAttack(HierarchicalModel& m, float t, float phase, bool ranged) {
    phase = lerp01(phase);
    const float windup = 0.30f;            // 0    -> 0.30: pull back
    const float strike = 0.55f;            // 0.30 -> 0.55: the hit
    float angle;
    if (phase < windup)       angle = lerpf(0.0f,     -1.05f, ease(phase / windup));
    else if (phase < strike)  angle = lerpf(-1.05f,    1.65f, ease((phase - windup) / (strike - windup)));
    else                      angle = lerpf(1.65f,     0.0f, ease((phase - strike) / (1.0f - strike)));

    switch (swingKind(m)) {
        case 1: // Kai - the sword arm sweeps down and forward, torso follows.
            if (m.hasPart("armR"))   m.setPose("armR", turn(angle));
            if (m.hasPart("armL"))   m.setPose("armL", turn(-angle * 0.25f));
            if (m.hasPart("body"))   m.setPose("body", turn(angle * 0.14f));
            break;

        case 2: // Zane - cock the arm back, then flick the shuriken forward.
        case 3: // Hypnobrai - sweep the staff from high to low.
        case 4: // Constrictai - wind up and pound with the right fist.
        default: {
            if (m.hasPart("armR")) m.setPose("armR", turn(angle * (ranged ? 0.9f : 0.7f)));
            if (m.hasPart("armL")) m.setPose("armL", turn(-angle * 0.3f));
            if (m.hasPart("body")) m.setPose("body", turn(angle * 0.12f));
            break;
        }

        case 5: { // Venomari - rear the head back and lunge the snout forward.
            if (m.hasPart("head"))  m.setPose("head", shiftTurn(angle * 4.0f, angle * 2.0f, angle * 0.35f));
            if (m.hasPart("snout")) m.setPose("snout", shift(angle * 3.0f, 0.0f));
            if (m.hasPart("body"))  m.setPose("body", turn(angle * 0.10f));
            break;
        }
    }

    swayTail(m, t, 0.8f);
    spinGear(m, t);
}

// --------------------------------------------------------------------
// JUMP - tuck the legs and throw the arms up while airborne (US-29).
// --------------------------------------------------------------------
void updateJump(HierarchicalModel& m, float phase) {
    phase = lerp01(phase);
    float lift = std::sin(phase * 3.14159265f);   // 0 at take-off/landing, 1 at the top

    if (m.hasPart("legL")) m.setPose("legL", shiftTurn(0.0f,  lift * 3.0f, -lift * 0.55f));
    if (m.hasPart("legR")) m.setPose("legR", shiftTurn(0.0f,  lift * 3.0f,  lift * 0.30f));
    if (m.hasPart("body")) m.setPose("body", shift(0.0f, lift * 2.0f));
    if (m.hasPart("armL")) m.setPose("armL", turn(-lift * 0.7f));
    if (m.hasPart("armR")) m.setPose("armR", turn( lift * 0.7f));
    if (m.hasPart("head")) m.setPose("head", shift(0.0f, lift * 1.0f));
}

// --------------------------------------------------------------------
// DUCK - squash the upper body down and forward (US-30).  Legs stay
// planted so the unit still reads as standing, just much shorter.
// --------------------------------------------------------------------
void updateDuck(HierarchicalModel& m, float amount) {
    amount = lerp01(amount);
    float sy = lerpf(1.0f, C::DUCK_SCALE_Y, amount);

    // Scaling the body also scales everything parented to it (head, arms,
    // hood) toward the body origin, so the whole upper body sinks.
    if (m.hasPart("body")) m.setPose("body", Transform2D({ 0.0f, -5.0f * amount }, 0.10f * amount, { 1.0f, sy }));
    if (m.hasPart("head")) m.setPose("head", shift(3.0f * amount, 0.0f));
    if (m.hasPart("armL")) m.setPose("armL", turn(-0.35f * amount));
    if (m.hasPart("armR")) m.setPose("armR", turn(-0.35f * amount));
    // Splay the legs a little for a wide, braced crouch.
    if (m.hasPart("legL")) m.setPose("legL", turn(-0.10f * amount));
    if (m.hasPart("legR")) m.setPose("legR", turn( 0.10f * amount));
}

// --------------------------------------------------------------------
// HIT - a quick recoil that fades out (amount 1 -> 0), used for US-43.
// --------------------------------------------------------------------
void updateHit(HierarchicalModel& m, float amount) {
    amount = lerp01(amount);
    if (m.hasPart("body")) m.setPose("body", shiftTurn(-3.0f * amount, 0.0f, -0.18f * amount));
    if (m.hasPart("head")) m.setPose("head", turn(-0.25f * amount));
    if (m.hasPart("armL")) m.setPose("armL", turn(-0.30f * amount));
    if (m.hasPart("armR")) m.setPose("armR", turn( 0.30f * amount));
}

// --------------------------------------------------------------------
// DEATH - the unit topples over (phase 0 -> 1).  US-45 removes a
// defeated unit immediately, so this is for the cutscene and any
// optional "fall, then remove" fade the team chooses.
// --------------------------------------------------------------------
void updateDeath(HierarchicalModel& m, float phase) {
    phase = lerp01(phase);
    if (m.hasPart("body")) m.setPose("body", shiftTurn(phase * 18.0f, -phase * 10.0f, -phase * 1.45f));
    if (m.hasPart("head")) m.setPose("head", turn(-phase * 0.5f));
    if (m.hasPart("legL")) m.setPose("legL", turn( phase * 0.5f));
    if (m.hasPart("legR")) m.setPose("legR", turn( phase * 0.8f));
    if (m.hasPart("armL")) m.setPose("armL", turn(-phase * 0.9f));
    if (m.hasPart("armR")) m.setPose("armR", turn( phase * 0.6f));
    swayTail(m, phase * 2.0f, 1.0f);
}

} // namespace Animation
