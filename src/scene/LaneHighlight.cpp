#include "LaneHighlight.h"
// ====================================================================
// FILE:     src/scene/LaneHighlight.cpp
// OWNER:    Member 3
// TASKS:    M3-11, US-13, US-14
// STATUS:   MEMBER 3 IMPLEMENTATION
//
// PURPOSE:
//   Draws the hover highlight and selected-lane indicators:
//   - Hovering highlights the entire horizontal lane with a golden glow
//   - Selecting locks the highlight with glowing golden double-borders
//     and dragon corner brackets. Multiple lanes can be selected.
// ====================================================================

#include "Scene.h"
#include <cmath>

namespace
{
constexpr float laneTop = 390.0f;
constexpr float laneHeight = 110.0f;
}

void LaneHighlight::draw(int lane, bool hovered, bool selected)
{
    if (lane < 0 || lane > 2)
        return;

    float y = laneTop + lane * laneHeight;
    float startX = 254.0f;
    float endX = 1280.0f;
    float width = endX - startX;
    float t = M3Draw::getTime();

    // 1. HOVER HIGHLIGHT: Soft warm golden wash across the entire lane
    if (hovered && !selected)
    {
        float hoverPulse = 0.85f + 0.15f * std::sin(t * 8.0f);
        // Translucent golden fill
        M3Draw::quad(startX, y + 2, width, laneHeight - 4, 1.0f, 0.88f, 0.35f, 0.16f * hoverPulse);
        // Top and bottom guide lines
        M3Draw::quad(startX, y + 2, width, 2, 1.0f, 0.88f, 0.40f, 0.60f * hoverPulse);
        M3Draw::quad(startX, y + laneHeight - 4, width, 2, 1.0f, 0.88f, 0.40f, 0.60f * hoverPulse);

        // Hover indicator arrow on the left
        float arrowX = startX + 12.0f + std::sin(t * 10.0f) * 4.0f;
        float arrowY = y + laneHeight * 0.5f;
        M3Draw::triangle(arrowX, arrowY - 8, arrowX + 12, arrowY, arrowX, arrowY + 8, 1.0f, 0.92f, 0.45f, 0.9f);
    }

    // 2. SELECTED LANE: Locked highlight with glowing double border & dragon brackets
    if (selected)
    {
        float selPulse = 0.88f + 0.12f * std::sin(t * 6.0f);

        // Rich golden amber tint across the lane
        M3Draw::quad(startX, y + 2, width, laneHeight - 4, 1.0f, 0.78f, 0.18f, 0.22f * selPulse);

        // Glowing double borders along top and bottom
        // Top outer & inner border
        M3Draw::quad(startX, y + 2, width, 3, 1.0f, 0.92f, 0.45f, 0.95f * selPulse);
        M3Draw::quad(startX, y + 7, width, 1, 1.0f, 0.80f, 0.25f, 0.65f * selPulse);

        // Bottom outer & inner border
        M3Draw::quad(startX, y + laneHeight - 5, width, 3, 1.0f, 0.92f, 0.45f, 0.95f * selPulse);
        M3Draw::quad(startX, y + laneHeight - 8, width, 1, 1.0f, 0.80f, 0.25f, 0.65f * selPulse);

        // Dragon Corner Brackets (Top-left & Bottom-left)
        M3Draw::quad(startX + 6, y + 8, 14, 3, 1.0f, 0.95f, 0.50f, 0.9f);
        M3Draw::quad(startX + 6, y + 8, 3, 14, 1.0f, 0.95f, 0.50f, 0.9f);

        M3Draw::quad(startX + 6, y + laneHeight - 11, 14, 3, 1.0f, 0.95f, 0.50f, 0.9f);
        M3Draw::quad(startX + 6, y + laneHeight - 22, 3, 14, 1.0f, 0.95f, 0.50f, 0.9f);

        // Active Lane Crest Badge on the left
        float badgeX = startX + 22.0f;
        float badgeY = y + laneHeight * 0.5f;
        M3Draw::circle(badgeX, badgeY, 10.0f, 1.0f, 0.85f, 0.20f, 0.9f, 16);
        M3Draw::circle(badgeX, badgeY, 7.0f, 0.20f, 0.15f, 0.08f, 1.0f, 14);
        M3Draw::quad(badgeX - 3, badgeY - 3, 6, 6, 1.0f, 0.95f, 0.45f, 1.0f);
    }
}
