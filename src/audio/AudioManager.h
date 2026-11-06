#pragma once
// ====================================================================
// FILE:     src/audio/AudioManager.h
// OWNER:    Member 4
// TASKS:    M4-11
// STATUS:   MINIMAL STUB IMPLEMENTED BY MEMBER 2 (agreed) so the combat
//           code (US-56) has something to call. Member 4 replaces the
//           no-op bodies with real audio.
//
// PURPOSE:
//   OPTIONAL BONUS. Background music, sound effects and volume
//   settings.
//
// RESPONSIBILITIES:
//   - Play music and sound effects
//   - Volume up, down and mute (Sound Settings menu)
//   - Used by Members 1 and 2 to trigger effects
//
// PLANNED PUBLIC INTERFACE (names agreed by the team):
//   - init()
//   - playMusic()
//   - playSound(id)
//   - setVolume(v)
//   - mute()
//
// INTERACTS WITH: Menus, Game, Player, Enemy
// ====================================================================

// The interactions Member 2 needs sounds for (US-56: attacks, hits, deaths).
// Member 4 maps each id to a real sound file.
enum class SoundId {
    SwordSwing,     // Melee (Kai) attack
    ShurikenThrow,  // Ranged (Zane) attack
    IceBlast,       // Ranged projectile impact
    SnakeHiss,      // an enemy attacks
    VenomSpit,      // Easy enemy fires from above
    SonicBlast,     // Medium enemy fires from below
    MeleeCrush,     // Hard enemy attacks
    Hit,            // any unit takes damage
    Death,          // any unit is defeated
    OrbPickup       // collectible picked up
};

class AudioManager {
public:
    virtual ~AudioManager() = default;

    virtual void init() {}
    virtual void playMusic() {}
    virtual void playSound(SoundId /*id*/) {}   // no-op stub
    virtual void setVolume(float /*v*/) {}
    virtual void mute() {}
};

// Global accessor so Members 1 and 2 can call getAudio().playSound(...)
// without owning the audio system.
inline AudioManager*& audioSlot() {
    static AudioManager* current = nullptr;
    return current;
}
inline AudioManager& getAudio() {
    if (!audioSlot()) {
        static AudioManager fallback;
        audioSlot() = &fallback;
    }
    return *audioSlot();
}
inline void setAudio(AudioManager* a) { audioSlot() = a; }
