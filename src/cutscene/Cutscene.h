#pragma once
// ====================================================================
// FILE:     src/cutscene/Cutscene.h
// OWNER:    Member 3 (story cutscene assigned in the project specification)
// TASKS:    M3-CUTSCENE
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
//
// PURPOSE:
//   The 10-20 second animated introduction.
//
// RESPONSIBILITIES:
//   - Timeline that triggers each beat
//   - At least one player and one enemy, animated and interacting
//   - Short dialogue with subtitles, plus visible animation
//   - Skip with Esc/Space; tells Game when it finishes
//
// PLANNED PUBLIC INTERFACE (names to agree on, declared here):
//   - update(dt)
//   - draw()
//   - isFinished()
//   - skip()
//
// INTERACTS WITH: Scene, CharacterModels, Animation, TextRenderer, Game
// ====================================================================

#pragma once

class Cutscene
{
public:
    void update(float dt);
    void draw();
    bool isFinished() const;
    void skip();
    void reset();

private:
    float elapsed_ = 0.0f;
    bool finished_ = false;
};
