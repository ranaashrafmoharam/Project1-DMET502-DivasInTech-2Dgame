#pragma once
// ====================================================================
// FILE:     src/entities/Animation.h
// OWNER:    Member 2
// TASKS:    M2-09, M2-10, M2-12
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   All character animation poses driven by time.
//
// RESPONSIBILITIES:
//   - Walk / run / idle cycles            (Melee & Ranged advancing, US-25/26)
//   - Melee attack swing                  (US-25) and ranged throw (US-26)
//   - Jump arc and duck pose              (US-29, US-30)
//   - Hit flinch and death feedback hook  (US-43, US-45)
//
// HOW IT WORKS:
//   Every function writes "pose deltas" onto a HierarchicalModel with
//   setPose().  The model composes those deltas on top of its rest pose
//   and draws them, so the character's limbs move without the caller
//   knowing anything about the body layout.  A pose for a part that a
//   model does not have (e.g. "tail" on a ninja) is simply ignored,
//   which is why the same walk function drives humans and serpents.
//
// PART NAMES it drives (contract shared with CharacterModels.cpp):
//   legs  : legL, legR          arms : armL, armR, handR
//   torso : body, chest, coil   head : head, hood, snout
//   gear  : weapon, staff, shield, tailBase, tailTip
//
// INTERACTS WITH: HierarchicalModel, Player, Enemy, Cutscene
// ====================================================================

#include "HierarchicalModel.h"

namespace Animation {

    // ---- locomotion -------------------------------------------------
    // Standing still: a slow "breathing" bob and a gently swaying tail.
    void updateIdle(HierarchicalModel& model, float t);

    // Walking to the right / left. speedScale stretches the cycle so a
    // faster unit does not look like it is walking in slow motion.
    void updateWalk(HierarchicalModel& model, float t, float speedScale = 1.0f);

    // Faster, wider-strided walk (used by the Hard enemy, US-37).
    void updateRun(HierarchicalModel& model, float t, float speedScale = 1.0f);

    // ---- actions ----------------------------------------------------
    // Attack swing.  phase runs 0 -> 1 across the whole swing and the
    // model decides what to move based on the parts it owns: a sword arm
    // (blade), a thrown shuriken (starN), a low staff (staff) or a fist
    // (fistR).  t is a free-running clock used for weapon spin.
    void updateAttack(HierarchicalModel& model, float t, float phase, bool ranged);

    // Jump tuck.  phase runs 0 -> 1 over the airborne arc; the vertical
    // travel itself is done by the entity, this only adds the pose.
    void updateJump(HierarchicalModel& model, float phase);

    // Duck.  amount is 0 (standing) .. 1 (fully crouched, US-30).
    void updateDuck(HierarchicalModel& model, float amount);

    // ---- feedback ---------------------------------------------------
    // Short recoil when a unit is hit (amount 0..1 fades out).
    void updateHit(HierarchicalModel& model, float amount);

    // Collapse when a unit is defeated (phase 0..1).  US-45 removes the
    // unit immediately, so this is used by the cutscene / optional fade.
    void updateDeath(HierarchicalModel& model, float phase);

} // namespace Animation
