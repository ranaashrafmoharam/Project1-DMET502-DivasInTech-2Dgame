#pragma once
// ====================================================================
// FILE:     src/scene/HomeBase.h
// The Monastery Gate safe zone on the left.
// ====================================================================

class HomeBase {
public:
    static HomeBase& instance();

    void draw();

private:
    HomeBase() = default;
};
