#include "Cutscene.h"
// ====================================================================
// FILE:     src/cutscene/Cutscene.cpp
// OWNER:    Member 3 (story cutscene assigned in the project specification)
// TASKS:    M3-CUTSCENE, US-06 to US-10, US-51
// STATUS:   MEMBER 3 IMPLEMENTATION
//
// PURPOSE:
//   A fully animated, timeline-driven intro cutscene in classic Game Boy /
//   retro Pokemon RPG style:
//   - Exact 9 dialogues preserved with full punctuation & word wrap
//   - Pokemon-style dialogue box with speaker plates & high-detail portraits
//   - Animated pixel art characters: Master Wu, Kai, Zane, Cole, Garmadon, Venomari
//   - Golden Weapons Altar and a non-combat confrontation
//   - Space / Esc to skip, R to replay, seamlessly transitions to gameplay
// ====================================================================

#include "Cutscene.h"
#include "../scene/Scene.h"
#include "../scene/CharacterSprites.h"
#include <cmath>
#include <cstring>
#include <vector>
#include <string>
#include <GLFW/glfw3.h>

namespace
{
// =========================================================================
// 1. HIGH-DETAIL 32x32 RETRO SPEAKER PORTRAITS (Pokemon Game Boy RPG Style)
// =========================================================================

void drawPortraitFrame(float x, float y, float size)
{
    // Outer wooden lacquer box
    M3Draw::quad(x, y, size, size, 0.10f, 0.08f, 0.12f);
    M3Draw::quad(x + 2, y + 2, size - 4, size - 4, 0.88f, 0.74f, 0.32f); // Gold rim
    M3Draw::quad(x + 5, y + 5, size - 10, size - 10, 0.14f, 0.12f, 0.18f); // Inner bezel
    M3Draw::quad(x + 7, y + 7, size - 14, size - 14, 0.18f, 0.22f, 0.32f); // Portrait background
}

void drawWuPortrait(float x, float y, float size)
{
    drawPortraitFrame(x, y, size);

    // Pixel scale inside portrait
    float ps = (size - 16.0f) / 32.0f;
    float ox = x + 8.0f;
    float oy = y + 8.0f;

    auto px = [ox, oy, ps](int col, int row, float r, float g, float b, float a = 1.0f) {
        M3Draw::quad(ox + col * ps, oy + row * ps, ps, ps, r, g, b, a);
    };

    // --- Master Wu Straw Hat (Kasa) ---
    // Wide conical woven bamboo hat with thatch shading
    for (int r = 2; r <= 10; ++r)
    {
        int w = (r - 2) * 3 + 4;
        int startC = 16 - w / 2;
        for (int c = startC; c < startC + w; ++c)
        {
            float shade = ((c + r) % 2 == 0) ? 0.95f : 0.78f;
            px(c, r, 0.88f * shade, 0.72f * shade, 0.32f * shade);
        }
    }
    // Wide dark brim rim
    for (int c = 2; c <= 29; ++c)
    {
        px(c, 11, 0.32f, 0.20f, 0.10f);
        px(c, 12, 0.18f, 0.12f, 0.06f);
    }
    // Hat center woven crest
    px(15, 3, 0.98f, 0.88f, 0.45f);
    px(16, 3, 0.98f, 0.88f, 0.45f);

    // --- Face & Skin ---
    for (int r = 13; r <= 19; ++r)
    {
        for (int c = 9; c <= 22; ++c)
        {
            px(c, r, 0.98f, 0.82f, 0.64f); // Warm peach skin
        }
    }

    // Bushy White Arched Eyebrows
    for (int c = 10; c <= 13; ++c) px(c, 14, 1.0f, 1.0f, 1.0f);
    for (int c = 18; c <= 21; ++c) px(c, 14, 1.0f, 1.0f, 1.0f);
    px(9, 15, 0.9f, 0.9f, 0.9f);
    px(22, 15, 0.9f, 0.9f, 0.9f);

    // Wise Eyes (with glint)
    px(11, 16, 0.10f, 0.10f, 0.14f);
    px(12, 16, 0.10f, 0.10f, 0.14f);
    px(12, 16, 1.0f, 1.0f, 1.0f, 0.5f); // Glint

    px(19, 16, 0.10f, 0.10f, 0.14f);
    px(20, 16, 0.10f, 0.10f, 0.14f);
    px(20, 16, 1.0f, 1.0f, 1.0f, 0.5f);

    // Nose
    px(15, 17, 0.88f, 0.70f, 0.52f);
    px(16, 17, 0.88f, 0.70f, 0.52f);

    // --- Flowing White Beard ---
    for (int r = 18; r <= 29; ++r)
    {
        int bw = 16 - (r - 18);
        if (bw < 4) bw = 4;
        int sc = 16 - bw / 2;
        for (int c = sc; c < sc + bw; ++c)
        {
            float bShade = (c == sc || c == sc + bw - 1 || r > 26) ? 0.86f : 1.0f;
            px(c, r, bShade, bShade, bShade + 0.02f);
        }
    }

    // Kimono collar (Stone grey / white)
    for (int c = 8; c <= 13; ++c)
    {
        px(c, 30, 0.85f, 0.84f, 0.80f);
        px(c, 31, 0.70f, 0.68f, 0.64f);
    }
    for (int c = 18; c <= 23; ++c)
    {
        px(c, 30, 0.85f, 0.84f, 0.80f);
        px(c, 31, 0.70f, 0.68f, 0.64f);
    }
}

void drawKaiPortrait(float x, float y, float size)
{
    drawPortraitFrame(x, y, size);

    float ps = (size - 16.0f) / 32.0f;
    float ox = x + 8.0f;
    float oy = y + 8.0f;

    auto px = [ox, oy, ps](int col, int row, float r, float g, float b, float a = 1.0f) {
        M3Draw::quad(ox + col * ps, oy + row * ps, ps, ps, r, g, b, a);
    };

    // --- Spiky Anime Reddish-Brown / Crimson Hair ---
    // Hair spikes top
    px(14, 2, 0.68f, 0.16f, 0.14f); px(15, 2, 0.85f, 0.22f, 0.18f);
    px(18, 3, 0.68f, 0.16f, 0.14f); px(19, 3, 0.85f, 0.22f, 0.18f); px(20, 4, 0.85f, 0.22f, 0.18f);
    px(9, 4, 0.85f, 0.22f, 0.18f);  px(10, 4, 0.68f, 0.16f, 0.14f);
    px(23, 6, 0.85f, 0.22f, 0.18f); px(24, 7, 0.68f, 0.16f, 0.14f);

    // Hair mass
    for (int r = 5; r <= 11; ++r)
    {
        for (int c = 7; c <= 24; ++c)
        {
            float highlight = (c >= 12 && c <= 17 && r <= 8) ? 0.95f : 0.75f;
            px(c, r, 0.82f * highlight, 0.18f * highlight, 0.14f);
        }
    }

    // --- Red Ninja Headband (with Gold Dragon Crest) ---
    for (int c = 7; c <= 24; ++c)
    {
        px(c, 12, 0.88f, 0.12f, 0.14f);
        px(c, 13, 0.72f, 0.08f, 0.10f);
    }
    // Gold dragon insignia on headband
    px(15, 12, 1.00f, 0.88f, 0.25f);
    px(16, 12, 1.00f, 0.88f, 0.25f);
    px(15, 13, 0.95f, 0.75f, 0.18f);
    px(16, 13, 0.95f, 0.75f, 0.18f);

    // --- Face & Features ---
    for (int r = 14; r <= 24; ++r)
    {
        for (int c = 9; c <= 22; ++c)
        {
            px(c, r, 0.98f, 0.82f, 0.64f);
        }
    }

    // Fierce Angled Eyebrows
    px(10, 15, 0.35f, 0.08f, 0.08f);
    px(11, 16, 0.35f, 0.08f, 0.08f);
    px(12, 16, 0.35f, 0.08f, 0.08f);

    px(19, 16, 0.35f, 0.08f, 0.08f);
    px(20, 16, 0.35f, 0.08f, 0.08f);
    px(21, 15, 0.35f, 0.08f, 0.08f);

    // Focused Eyes with catchlight
    px(11, 17, 0.10f, 0.10f, 0.14f);
    px(12, 17, 0.10f, 0.10f, 0.14f);
    px(12, 17, 1.00f, 1.00f, 1.00f, 0.7f);

    px(19, 17, 0.10f, 0.10f, 0.14f);
    px(20, 17, 0.10f, 0.10f, 0.14f);
    px(20, 17, 1.00f, 1.00f, 1.00f, 0.7f);

    // Determined smirk mouth
    px(14, 22, 0.50f, 0.25f, 0.20f);
    px(15, 22, 0.50f, 0.25f, 0.20f);
    px(16, 21, 0.50f, 0.25f, 0.20f);
    px(17, 21, 0.50f, 0.25f, 0.20f);

    // Red Gi High Collar & Black Undershirt
    for (int r = 25; r <= 31; ++r)
    {
        for (int c = 6; c <= 25; ++c)
        {
            if (c >= 14 && c <= 17 && r <= 28)
                px(c, r, 0.14f, 0.14f, 0.16f); // Black undershirt
            else
                px(c, r, 0.85f, 0.14f, 0.14f); // Red ninja tunic
        }
    }
}

// =========================================================================
// 2. RETRO POKEMON DIALOGUE BOX WITH NAMEPLATE & WORD WRAP
// =========================================================================

void drawSuppliedPortrait(const char* const sprite[27], float centerX, float baseline,
                          float scale)
{
    static const float colours[10][3] = {
        {0.16f, 0.09f, 0.33f}, {0.31f, 0.16f, 0.51f}, {0.43f, 0.22f, 0.58f},
        {0.57f, 0.31f, 0.62f}, {0.75f, 0.47f, 0.69f}, {0.90f, 0.67f, 0.77f},
        {0.96f, 0.82f, 0.85f}, {1.0f, 1.0f, 1.0f}, {0.09f, 0.06f, 0.21f},
        {0.86f, 0.59f, 0.24f}
    };
    for (int row = 0; row < 27; ++row)
    {
        for (int col = 0; col < 18; ++col)
        {
            char pixel = sprite[row][col];
            if (pixel == '.') continue;
            int colour = pixel - 'A';
            M3Draw::quad(centerX + (col - 9) * scale,
                         baseline + (row - 26) * scale,
                         scale, scale,
                         colours[colour][0], colours[colour][1], colours[colour][2]);
        }
    }
}

void drawLargePortrait(const char* const sprite[24], float centerX, float baseline,
                       float scale)
{
    static const float colours[12][3] = {
        {0.31f, 0.08f, 0.06f}, {0.67f, 0.14f, 0.14f}, {0.92f, 0.18f, 0.18f},
        {1.0f, 0.75f, 0.0f}, {0.18f, 0.10f, 0.08f}, {0.49f, 0.35f, 0.22f},
        {0.84f, 0.75f, 0.57f}, {0.96f, 0.86f, 0.67f}, {0.67f, 0.67f, 0.67f},
        {0.92f, 0.92f, 0.92f}, {0.24f, 0.24f, 0.24f}, {0.08f, 0.08f, 0.08f}
    };
    for (int row = 0; row < 24; ++row)
    {
        for (int col = 0; col < 24; ++col)
        {
            char pixel = sprite[row][col];
            if (pixel == '.') continue;
            int colour = pixel - 'A';
            M3Draw::quad(centerX + (col - 12) * scale,
                         baseline + (row - 23) * scale,
                         scale, scale,
                         colours[colour][0], colours[colour][1], colours[colour][2]);
        }
    }
}

void drawOrnatePortraitFrame(float x, float y, float size)
{
    M3Draw::quad(x, y, size, size, 0.07f, 0.05f, 0.04f);
    M3Draw::quad(x + 4, y + 4, size - 8, size - 8, 0.56f, 0.31f, 0.20f);
    M3Draw::quad(x + 8, y + 8, size - 16, size - 16, 0.18f, 0.11f, 0.09f);
    M3Draw::quad(x + 14, y + 14, size - 28, size - 28, 0.10f, 0.12f, 0.18f);
    M3Draw::quad(x + 8, y + 8, 18, 6, 0.72f, 0.43f, 0.27f);
    M3Draw::quad(x + size - 26, y + 8, 18, 6, 0.72f, 0.43f, 0.27f);
    M3Draw::quad(x + 8, y + size - 14, 18, 6, 0.72f, 0.43f, 0.27f);
    M3Draw::quad(x + size - 26, y + size - 14, 18, 6, 0.72f, 0.43f, 0.27f);
}

static const char* LARGE_KAI[24] = {
    "..........EE.EE.........", "..........ELEEEA........", ".........EEEEEEEE.......",
    ".........EEEELEEE.......", "........AEEEEELLLE......", "........EEEEEEEEELE.....",
    "........EEAAFAEAEEL.....", "........LEFFFFFFFELE....", "........LADDFDFDAALE....",
    ".......AEFDDDDFDFFEA....", ".......EEFDDDDDDDFE.....", ".........FFDDDDDDAE.....",
    "........AAAFFFFFFA......", ".....AABAAAABBBBBABAA...", "..AABBBBBBAABBBBBBBBBBA.",
    ".ABBBCCBBBBAAAAABBBBBBBA", "ABBBBBBBBBBAAABABBBBBBBB", "ABBBBAAAABBBAFABBBBBABBB",
    "AABBBA.AABBBFFFBBBBBEBBA", ".ABBBALAABBAFFFAABBALLB.", ".AAELELLABBAKABBAAALLL.",
    "..ALLLLLLEABAAABBBBBLLL.", "...LLLLLLABBABBABBBBLLA.", "....LLLLABBBBBAAAAAB...."
};

static const char* LARGE_WU[24] = {
    "........FGGGGGGGG.......", "......IGGGGGGGGGGGG.....", "....GGGGGGGGGGGGGGGGG...",
    "..FIIIIFFFGGGGGGGGGGGGG.", "..FFKFFFFFIGDDGGFFFFFFFF", "..KKKKKFFFDGGGGDFKKFFFKK",
    "......KKFFDFDDFDDKK.....", "........FFDFDDDDD.......", "........FFIIGGIDD.......",
    "........FFIIIIIIF.......", "........FIIIIIIII.......", "........FIIIIIIII.......",
    ".......FFIIIIIIIIK......", "......FFFIIIIIIIIFII....", ".....FFFIIIIIIIIGIHG....",
    ".....FFIIIIIIGGIHHHHI...", ".....FFIIIIIGGHIHHHHH...", "....FFIIIIIGHHHIHIHHHI..",
    "....FIIIIIIGHHHIHIGHHG..", "....IGHGIIIHHJHIHFIHHH..", "....IHHHGIIHJJHIHGHHHH..",
    "....IGHHHHGHHHGIHHHHHH..", "....FIGHHHHDDDDGHHHHHG..", "....FFIGHHGDDDDDHHHHII.."
};

static const char* SUPPLIED_KAI[27] = {
    "......IIIIIA......", "....AIIIIIIII.....", "..IIAIIIIIAIIAA...",
    ".IIIIIIIIAIAIII...", ".IIAAIIIIIIIAIIB..", ".IIAIIIIAAAIAAII..",
    "IIIAAAIAAAAAAAIA..", "AIIIIIJIAAAIIIIII.", "AIIAIJJJIIIJIIIII.",
    ".IIAAJJJAIJJAIIIA.", ".IIIAJJJJJJJAIIII.", ".IIIAJAJJJAJJAIII.",
    ".IIAJJJJJJJJJJIII.", ".AIIJJJJJJJJJJIIA.", "..IAJAJJJJJIAAAA..",
    "...IIIAJAAJIIAB...", "..IIIAAJIIJAIII...", "..IIAIJAIJAIIII...",
    "..IIIIJAAJAIIII...", "...IIIIIIIIIII....", "....AIAIIIAII.....",
    "....AIAAIIAII.....", "....AIIIIIIII.....", ".....IIIBIIA......",
    "..................", "..................", ".................."
};

static const char* SUPPLIED_WU[27] = {
    "........BABBA.....", "..................", ".........CCDC.....",
    ".........AIIA.....", "......CCIICCIIDB..", "......AIDDJJDDAA..",
    ".....CIJDJDJDJDID.", "....CIDDDDJJEJJDIC", "...DICJJJJJJJDEFEI",
    "...IBCCJDJJJJJJDDA", "....IIIJJJJJJJJIII", ".....CIJJAJJIJJIC.",
    ".....CIJJJFFJJJI..", "......IAJFFFFJAI..", ".......IDGEEFEI...",
    "......BIDGEFFEIB..", "......ABFFFEFECI..", ".....BAAAFFEEIBI..",
    "....BIDBIIIIAII...", "....ICACIDIIEI....", ".....IDDAFDDGI....",
    ".....CIIIIIIID....", "......DDDD........", "..................",
    "..................", "..................", ".................."
};

void drawRetroDialogueBox(const char* speaker, const char* line, bool isKai,
                          CharacterSprites::Character portrait)
{
    float boxX = 40.0f;
    float boxY = 525.0f;
    float boxW = 1200.0f;
    float boxH = 175.0f;
    float t = M3Draw::getTime();

    // 1. Dialogue Box Frame (Classic Pokemon double border & parchment)
    M3Draw::quad(boxX, boxY, boxW, boxH, 0.08f, 0.07f, 0.12f, 0.98f);            // Outer black
    M3Draw::quad(boxX + 3, boxY + 3, boxW - 6, boxH - 6, 0.88f, 0.76f, 0.40f);   // Gold outer rim
    M3Draw::quad(boxX + 6, boxY + 6, boxW - 12, boxH - 12, 0.16f, 0.14f, 0.20f); // Dark bezel
    M3Draw::quad(boxX + 8, boxY + 8, boxW - 16, boxH - 16, 0.10f, 0.13f, 0.22f, 0.95f); // Deep navy parchment

    // 2. High-Detail Speaker Portrait
    float portraitX = boxX + 16.0f;
    float portraitY = boxY + 16.0f;
    float portraitSize = 156.0f;

    // Use the supplied character-style sprites in the dialogue portrait.
    // The portrait follows the actual speaker.
    float portraitCenterX = portraitX + portraitSize * 0.5f;
    float portraitBaseline = portraitY + portraitSize - 8.0f;
    switch (portrait)
    {
    case CharacterSprites::Character::WU:
        drawOrnatePortraitFrame(portraitX, portraitY, portraitSize);
        M3Draw::image("assets/textures/wu_portrait.bmp", portraitX + 12.0f, portraitY + 12.0f,
                      portraitSize - 24.0f, portraitSize - 24.0f);
        break;
    case CharacterSprites::Character::KAI:
        drawOrnatePortraitFrame(portraitX, portraitY, portraitSize);
        M3Draw::image("assets/textures/kai_portrait.bmp", portraitX + 12.0f, portraitY + 12.0f,
                      portraitSize - 24.0f, portraitSize - 24.0f);
        break;
    case CharacterSprites::Character::ZANE:
        CharacterSprites::drawZane(portraitCenterX, portraitBaseline, 2.8f,
                                   CharacterSprites::State::IDLE, M3Draw::getTime(), true);
        break;
    case CharacterSprites::Character::COLE:
        CharacterSprites::drawCole(portraitCenterX, portraitBaseline, 2.8f,
                                   CharacterSprites::State::IDLE, M3Draw::getTime(), true);
        break;
    case CharacterSprites::Character::VENOMARI:
        CharacterSprites::drawVenomari(portraitCenterX, portraitBaseline, 2.8f,
                                       CharacterSprites::State::IDLE, M3Draw::getTime(), false);
        break;
    default:
        CharacterSprites::drawKai(portraitCenterX, portraitBaseline, 2.8f,
                                  CharacterSprites::State::IDLE, M3Draw::getTime(), true);
        break;
    }

    // 3. Speaker Name Plate
    float nameplateX = boxX + 175.0f;
    float nameplateY = boxY + 16.0f;
    float nameplateW = 190.0f;
    float nameplateH = 28.0f;

    if (isKai)
    {
        M3Draw::quad(nameplateX, nameplateY, nameplateW, nameplateH, 0.78f, 0.14f, 0.12f);
        M3Draw::quad(nameplateX + 1, nameplateY + 1, nameplateW - 2, nameplateH - 2, 0.92f, 0.22f, 0.18f);
        M3Draw::text(nameplateX + 14.0f, nameplateY + 6.0f, speaker, 2.0f, 1.0f, 0.95f, 0.75f);
    }
    else
    {
        M3Draw::quad(nameplateX, nameplateY, nameplateW, nameplateH, 0.65f, 0.48f, 0.16f);
        M3Draw::quad(nameplateX + 1, nameplateY + 1, nameplateW - 2, nameplateH - 2, 0.85f, 0.68f, 0.24f);
        M3Draw::text(nameplateX + 14.0f, nameplateY + 6.0f, speaker, 2.0f, 0.12f, 0.08f, 0.04f);
    }

    // 4. Word-Wrapped Dialogue Text
    float textStartX = boxX + 180.0f;
    float textY = boxY + 62.0f;
    float textMaxW = boxW - 220.0f;
    float textScale = 2.4f;

    // Word wrapping
    std::string textStr = line;
    std::vector<std::string> words;
    size_t pos = 0;
    while (pos < textStr.length())
    {
        while (pos < textStr.length() && textStr[pos] == ' ')
            ++pos;
        if (pos >= textStr.length()) break;
        size_t end = textStr.find(' ', pos);
        if (end == std::string::npos) end = textStr.length();
        words.push_back(textStr.substr(pos, end - pos));
        pos = end;
    }

    std::string currentLine = "";
    float curLineW = 0.0f;
    int lineIndex = 0;
    for (const auto& w : words)
    {
        float wordW = M3Draw::textWidth(w.c_str(), textScale) + 6.0f * textScale;
        if (curLineW + wordW > textMaxW && !currentLine.empty())
        {
            M3Draw::text(textStartX, textY + lineIndex * 34.0f, currentLine.c_str(), textScale,
                         1.0f, 0.98f, 0.90f);
            currentLine = w;
            curLineW = wordW;
            ++lineIndex;
        }
        else
        {
            if (!currentLine.empty()) currentLine += " ";
            currentLine += w;
            curLineW += wordW;
        }
    }
    if (!currentLine.empty())
    {
        M3Draw::text(textStartX, textY + lineIndex * 34.0f, currentLine.c_str(), textScale,
                     1.0f, 0.98f, 0.90f);
    }

    // 5. Classic Pokemon Animated Bouncing Arrow Cursor at Bottom-Right
    float arrowBob = std::sin(t * 8.0f) * 3.5f;
    float arrowX = boxX + boxW - 36.0f;
    float arrowY = boxY + boxH - 24.0f + arrowBob;
    M3Draw::triangle(arrowX, arrowY, arrowX + 16, arrowY, arrowX + 8, arrowY + 12,
                     0.98f, 0.82f, 0.22f);
    M3Draw::triangle(arrowX + 2, arrowY + 2, arrowX + 14, arrowY + 2, arrowX + 8, arrowY + 10,
                     1.00f, 0.96f, 0.65f);
}

// =========================================================================
// 3. PIXEL ART CHARACTERS ON SCENE
// =========================================================================

void drawPixelWu(float x, float y, float scale, bool gesturing)
{
    float p = scale;
    // Robe body
    M3Draw::quad(x - 6*p, y + 8*p, 12*p, 18*p, 0.86f, 0.84f, 0.80f);
    M3Draw::quad(x - 5*p, y + 16*p, 10*p, 3*p, 0.24f, 0.22f, 0.26f); // Black sash

    // Face & Beard
    M3Draw::quad(x - 4*p, y + 4*p, 8*p, 5*p, 0.98f, 0.82f, 0.64f);
    M3Draw::quad(x - 3*p, y + 5*p, 2*p, 2*p, 0.12f, 0.10f, 0.14f); // Eyes
    M3Draw::quad(x + 1*p, y + 5*p, 2*p, 2*p, 0.12f, 0.10f, 0.14f);
    // Flowing beard
    M3Draw::triangle(x - 5*p, y + 9*p, x + 5*p, y + 9*p, x, y + 21*p, 1.0f, 1.0f, 1.0f);

    // Conical Straw Hat
    M3Draw::triangle(x - 14*p, y + 4*p, x + 14*p, y + 4*p, x, y - 10*p, 0.88f, 0.72f, 0.32f);
    M3Draw::quad(x - 15*p, y + 3*p, 30*p, 2*p, 0.32f, 0.20f, 0.10f); // Hat brim

    // Wooden Staff (held or raised gesturing)
    if (gesturing)
    {
        M3Draw::line(x + 4*p, y + 14*p, x + 18*p, y - 6*p, 3*p, 0.48f, 0.28f, 0.14f);
        M3Draw::circle(x + 18*p, y - 6*p, 3*p, 0.95f, 0.82f, 0.32f); // Golden head
    }
    else
    {
        M3Draw::line(x + 7*p, y - 2*p, x + 7*p, y + 27*p, 3*p, 0.48f, 0.28f, 0.14f);
        M3Draw::circle(x + 7*p, y - 2*p, 3*p, 0.95f, 0.82f, 0.32f);
    }
}

void drawPixelKai(float x, float y, float scale, int frame, bool swordOut)
{
    float p = scale;
    float step = (frame % 2 == 1) ? 2.0f * p : 0.0f;

    // Body / Red Gi
    M3Draw::quad(x - 6*p, y + 8*p, 12*p, 13*p, 0.86f, 0.16f, 0.14f);
    M3Draw::quad(x - 5*p, y + 14*p, 10*p, 2*p, 0.18f, 0.16f, 0.18f); // Black belt
    M3Draw::quad(x - 2*p, y + 14*p, 4*p, 2*p, 1.00f, 0.88f, 0.25f);  // Gold crest

    // Legs / Boots
    M3Draw::quad(x - 5*p, y + 21*p + step, 4*p, 6*p, 0.72f, 0.12f, 0.12f);
    M3Draw::quad(x + 1*p, y + 21*p - step, 4*p, 6*p, 0.72f, 0.12f, 0.12f);

    // Head, Face & Spiky Hair
    M3Draw::quad(x - 5*p, y + 2*p, 10*p, 6*p, 0.98f, 0.82f, 0.64f);
    M3Draw::quad(x - 6*p, y - 1*p, 12*p, 3*p, 0.88f, 0.14f, 0.14f); // Red headband
    // Spiky Hair
    M3Draw::quad(x - 6*p, y - 6*p, 12*p, 5*p, 0.78f, 0.18f, 0.14f);
    M3Draw::triangle(x - 6*p, y - 6*p, x - 2*p, y - 12*p, x, y - 6*p, 0.78f, 0.18f, 0.14f);
    M3Draw::triangle(x, y - 6*p, x + 4*p, y - 14*p, x + 7*p, y - 6*p, 0.88f, 0.22f, 0.18f);

    // Eyes
    M3Draw::quad(x - 4*p, y + 3*p, 2*p, 2*p, 0.10f, 0.10f, 0.14f);
    M3Draw::quad(x + 2*p, y + 3*p, 2*p, 2*p, 0.10f, 0.10f, 0.14f);

    // Flaming Sword of Fire
    if (swordOut)
    {
        float t = M3Draw::getTime();
        float swordTipX = x + 24.0f * p;
        float swordTipY = y + 2.0f * p;
        M3Draw::line(x + 6*p, y + 12*p, swordTipX, swordTipY, 4*p, 0.90f, 0.92f, 0.95f);
        M3Draw::circle(x + 6*p, y + 12*p, 3*p, 0.95f, 0.78f, 0.22f); // Guard

        // Fire flames licking along the blade
        for (int f = 1; f <= 4; ++f)
        {
            float fx = x + 6*p + (swordTipX - (x + 6*p)) * (f * 0.24f);
            float fy = y + 12*p + (swordTipY - (y + 12*p)) * (f * 0.24f);
            float fOff = std::sin(t * 14.0f + f * 1.8f) * 4.0f * p;
            M3Draw::quad(fx - 2*p, fy - 2*p + fOff, 4*p, 4*p, 1.0f, 0.45f, 0.10f, 0.9f);
            M3Draw::quad(fx - 1*p, fy - 1*p + fOff, 2*p, 2*p, 1.0f, 0.92f, 0.35f, 0.95f);
        }
    }
}

void drawPixelZane(float x, float y, float scale)
{
    float p = scale;
    // White Ninja Gi
    M3Draw::quad(x - 6*p, y + 8*p, 12*p, 13*p, 0.95f, 0.96f, 0.98f);
    M3Draw::quad(x - 5*p, y + 14*p, 10*p, 2*p, 0.38f, 0.82f, 0.95f); // Cyan belt
    M3Draw::quad(x - 5*p, y + 21*p, 4*p, 6*p, 0.88f, 0.90f, 0.94f);
    M3Draw::quad(x + 1*p, y + 21*p, 4*p, 6*p, 0.88f, 0.90f, 0.94f);

    // Face & Flat-Top Platinum Blonde Hair
    M3Draw::quad(x - 5*p, y + 2*p, 10*p, 6*p, 0.98f, 0.85f, 0.70f);
    M3Draw::quad(x - 6*p, y - 6*p, 12*p, 7*p, 0.96f, 0.94f, 0.80f); // Blonde hair
    // Cyan Ice Eyes
    M3Draw::quad(x - 4*p, y + 3*p, 2*p, 2*p, 0.35f, 0.88f, 0.98f);
    M3Draw::quad(x + 2*p, y + 3*p, 2*p, 2*p, 0.35f, 0.88f, 0.98f);

    // Ice Shuriken in hand
    M3Draw::circle(x + 8*p, y + 10*p, 4*p, 0.45f, 0.85f, 1.0f);
}

void drawPixelCole(float x, float y, float scale)
{
    float p = scale;
    // Charcoal Black Gi
    M3Draw::quad(x - 7*p, y + 8*p, 14*p, 13*p, 0.18f, 0.18f, 0.20f);
    M3Draw::quad(x - 6*p, y + 14*p, 12*p, 2*p, 0.92f, 0.52f, 0.12f); // Earth orange sash
    M3Draw::quad(x - 6*p, y + 21*p, 5*p, 6*p, 0.14f, 0.14f, 0.16f);
    M3Draw::quad(x + 1*p, y + 21*p, 5*p, 6*p, 0.14f, 0.14f, 0.16f);

    // Face & Black Hair
    M3Draw::quad(x - 5*p, y + 2*p, 10*p, 6*p, 0.95f, 0.78f, 0.60f);
    M3Draw::quad(x - 6*p, y - 6*p, 12*p, 7*p, 0.10f, 0.10f, 0.12f); // Black hair
    M3Draw::quad(x - 4*p, y + 3*p, 2*p, 2*p, 0.10f, 0.10f, 0.12f);
    M3Draw::quad(x + 2*p, y + 3*p, 2*p, 2*p, 0.10f, 0.10f, 0.12f);
}

void drawPixelVenomari(float x, float y, float scale, float time, bool hit)
{
    float p = scale;
    float sway = std::sin(time * 6.0f) * 4.0f * p;

    float r = hit ? 1.0f : 0.32f;
    float g = hit ? 0.3f : 0.82f;
    float b = hit ? 0.3f : 0.26f;

    // Slithering Serpentine Lower Body (Tail)
    M3Draw::quad(x - 6*p + sway * 0.5f, y + 14*p, 12*p, 8*p, r, g, b);
    M3Draw::quad(x - 4*p + sway, y + 22*p, 8*p, 7*p, r * 0.8f, g * 0.8f, b * 0.8f);

    // Hooded Cobra Head & Neck
    M3Draw::quad(x - 8*p, y + 2*p, 16*p, 12*p, r, g, b);
    // Yellow venom markings
    M3Draw::quad(x - 5*p, y + 4*p, 3*p, 3*p, 0.95f, 0.90f, 0.25f);
    M3Draw::quad(x + 2*p, y + 4*p, 3*p, 3*p, 0.95f, 0.90f, 0.25f);

    // Glowing Ruby Snake Eyes
    M3Draw::quad(x - 5*p, y + 6*p, 2*p, 3*p, 1.00f, 0.10f, 0.10f);
    M3Draw::quad(x + 3*p, y + 6*p, 2*p, 3*p, 1.00f, 0.10f, 0.10f);

    // White Fangs
    M3Draw::quad(x - 3*p, y + 12*p, 2*p, 3*p, 1.0f, 1.0f, 1.0f);
    M3Draw::quad(x + 1*p, y + 12*p, 2*p, 3*p, 1.0f, 1.0f, 1.0f);
}

void drawUploadedGarmadon(float x, float y, float scale)
{
    float t = M3Draw::getTime();
    // Use the supplied four-armed sprite instead of the old procedural
    // placeholder.  The small phase-shifted sway makes the four lightning
    // arms read as a wave while preserving the original pixel art.
    const float wave = std::sin(t * 4.0f);
    const float waveSide = std::sin(t * 4.0f + 1.5708f);
    const int attackFrame = static_cast<int>(t * 6.0f) % 2;
    const float attackPose = attackFrame == 0 ? -3.0f : 4.0f;
    const float size = 190.0f * scale;
    const float drawX = x - size * 0.5f + wave * 2.0f * scale + attackPose * scale;
    const float drawY = y - size + waveSide * 2.0f * scale - attackPose * 0.35f * scale;
    M3Draw::image("assets/textures/garmadon_uploaded.bmp", drawX, drawY, size, size);

    // Two-frame attack accents: frame 0 pulls the lightning arms back and
    // frame 1 snaps them forward, giving the cutscene a clear attack beat.
    const float leftWave = (attackFrame == 0 ? -5.0f : 7.0f) +
                           std::sin(t * 4.0f) * 2.0f;
    const float rightWave = (attackFrame == 0 ? 7.0f : -5.0f) +
                            std::sin(t * 4.0f + 1.5708f) * 2.0f;
    M3Draw::line(drawX + 10.0f * scale, drawY + 112.0f * scale,
                 drawX + 28.0f * scale, drawY + (104.0f + leftWave) * scale,
                 2.0f * scale, 1.0f, 0.82f, 0.15f, 0.65f);
    M3Draw::line(drawX + size - 10.0f * scale, drawY + 112.0f * scale,
                 drawX + size - 28.0f * scale, drawY + (104.0f + rightWave) * scale,
                 2.0f * scale, 1.0f, 0.82f, 0.15f, 0.65f);

    if (attackFrame == 1)
    {
        // Lord Garmadon's red eye lasers fire during the second attack frame.
        const float eyeY = drawY + size * 0.40f;
        const float leftEyeX = drawX + size * 0.43f;
        const float rightEyeX = drawX + size * 0.57f;
        const float laserEndY = eyeY + 92.0f * scale;
        const float laserEndX = drawX - 110.0f * scale;
        M3Draw::line(leftEyeX, eyeY, laserEndX, laserEndY - 8.0f * scale,
                      7.0f * scale, 0.95f, 0.05f, 0.06f, 0.80f);
        M3Draw::line(rightEyeX, eyeY + 2.0f * scale,
                      laserEndX + 12.0f * scale, laserEndY + 8.0f * scale,
                      7.0f * scale, 0.95f, 0.05f, 0.06f, 0.80f);
        M3Draw::line(leftEyeX, eyeY, laserEndX, laserEndY - 8.0f * scale,
                      2.0f * scale, 1.0f, 0.65f, 0.65f, 1.0f);
        M3Draw::line(rightEyeX, eyeY + 2.0f * scale,
                      laserEndX + 12.0f * scale, laserEndY + 8.0f * scale,
                      2.0f * scale, 1.0f, 0.65f, 0.65f, 1.0f);

    }
}

void drawGoldenWeaponsAltar(float x, float y)
{
    float t = M3Draw::getTime();
    float pulse = 0.85f + 0.15f * std::sin(t * 5.0f);

    // Carved Stone Pedestal
    M3Draw::quad(x - 28, y + 20, 56, 16, 0.28f, 0.26f, 0.28f);
    M3Draw::quad(x - 22, y,      44, 20, 0.38f, 0.36f, 0.38f);
    M3Draw::quad(x - 32, y - 6,  64,  8, 0.24f, 0.22f, 0.24f);

    // Sacred Golden Aura Glow
    M3Draw::circle(x, y - 18, 38.0f * pulse, 1.00f, 0.85f, 0.20f, 0.30f * pulse, 24);

    // Four Golden Weapons Displayed on Altar
    // 1. Sword of Fire (left)
    M3Draw::line(x - 14, y - 4, x - 22, y - 28, 4, 1.0f, 0.82f, 0.18f);
    // 2. Scythe of Quakes (right)
    M3Draw::line(x + 14, y - 4, x + 22, y - 28, 4, 1.0f, 0.82f, 0.18f);
    // 3. Shurikens of Ice (center left)
    M3Draw::circle(x - 5, y - 18, 6, 1.0f, 0.95f, 0.45f);
    // 4. Nunchucks of Lightning (center right)
    M3Draw::line(x + 4, y - 22, x + 8, y - 14, 3, 1.0f, 0.90f, 0.30f);
}
}

// =========================================================================
// 4. CUTSCENE CORE METHODS
// =========================================================================

void Cutscene::reset()
{
    elapsed_ = 0.0f;
    finished_ = false;
}

void Cutscene::update(float dt)
{
    GLFWwindow* window = glfwGetCurrentContext();
    if (window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            skip();
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            reset();
        }
    }

    if (!finished_)
    {
        elapsed_ += dt;
        M3Draw::advanceTime(dt);

        // The Member 3 preview ends after the introductory dialogue. The
        // End with the characters holding their ground; a full fight is not
        // part of the Member 3 environment/story deliverable.
        if (elapsed_ >= 18.0f)
            finished_ = true;
    }
}

void Cutscene::skip()
{
    elapsed_ = 18.0f;
    finished_ = true;
}

bool Cutscene::isFinished() const
{
    return finished_;
}

void Cutscene::draw()
{
    // =========================================================================
    // 1. RENDER BACKGROUND ENVIRONMENT (Monastery grounds, sky, moon, pagodas)
    // =========================================================================
    // Base sky and ground
    M3Draw::quad(0, 0, 1280, 720, 0.09f, 0.08f, 0.18f);
    M3Draw::quad(0, 390, 1280, 330, 0.22f, 0.20f, 0.19f);

    // Call shared Scene draw elements so cutscene feels 100% part of the game world!
    Scene scene;
    // Keep the complete shared scene during dialogue so the lanterns and
    // torches match the background shown before and after the cutscene.
    // Hide only the overlapping left stone lantern during dialogue.  Keep the
    // right lantern visible so it matches the normal gameplay background.
    scene.draw(true, false);

    // =========================================================================
    // 2. TIMELINE BEATS & EXACT DIALOGUE LINES (1 to 9)
    // =========================================================================
    const char* speaker = "Master Wu";
    const char* dialogue = "";
    bool isKaiSpeaker = false;
    CharacterSprites::Character portrait = CharacterSprites::Character::WU;

    // Keep the cutscene actors on distinct visible lane baselines.  The
    // dialogue panel begins at y=525, so these three baselines preserve the
    // upper, middle and lower lane read without hiding the characters behind
    // the subtitles.  The animation states below use the same RUN/ATTACK
    // cycles as gameplay, matching the supplied running reference.
    const float laneY[3] = {365.0f, 430.0f, 495.0f};

    // Master Wu standing guard near the gate

    // --- BEAT 1 (0.0s - 2.0s) ---
    // Master Wu: "For generations, the Serpentine were sealed away beneath the earth."
    if (elapsed_ < 2.0f)
    {
        speaker = "Master Wu";
        dialogue = "\"For generations, the Serpentine were sealed away beneath the earth.\"";
        isKaiSpeaker = false;
        portrait = CharacterSprites::Character::WU;
    }
    // --- BEAT 2 (2.0s - 4.0s) ---
    // Master Wu: "Now they have returned to find the Golden Weapons."
    else if (elapsed_ < 4.0f)
    {
        speaker = "Master Wu";
        dialogue = "\"Now they have returned to find the Golden Weapons.\"";
        isKaiSpeaker = false;
        portrait = CharacterSprites::Character::WU;
    }
    // --- BEAT 3 (4.0s - 6.0s) ---
    // Master Wu: "They have crossed the forest and approached our monastery."
    else if (elapsed_ < 6.0f)
    {
        speaker = "Master Wu";
        dialogue = "\"They have crossed the forest and approached our monastery.\"";
        isKaiSpeaker = false;
        portrait = CharacterSprites::Character::WU;

        // Serpentine glowing eyes peering through trees on the right
        for (int i = 0; i < 4; ++i)
        {
            float eyX = 1050.0f + i * 45.0f;
            float eyY = 320.0f + (i % 2) * 20.0f;
            M3Draw::quad(eyX, eyY, 4, 3, 1.0f, 0.1f, 0.1f);
            M3Draw::quad(eyX + 8, eyY, 4, 3, 1.0f, 0.1f, 0.1f);
        }
    }
    // --- BEAT 4 (6.0s - 8.0s) ---
    // Master Wu: "Lord Garmadon watches beyond their ranks, waiting for the weapons to fall into his hands."
    else if (elapsed_ < 8.0f)
    {
        speaker = "Master Wu";
        dialogue = "\"Lord Garmadon watches beyond their ranks, waiting for the weapons to fall into his hands.\"";
        isKaiSpeaker = false;
        portrait = CharacterSprites::Character::WU;

        // Lord Garmadon appears high on the mountain cliff!
        // Lord Garmadon stands on the first battle lane instead of floating
        // above the monastery.  The purple villain is intentionally omitted
        // from this cutscene; it remains available in normal gameplay.
        drawUploadedGarmadon(940.0f, 490.0f, 1.0f);
    }
    // --- BEAT 5 (8.0s - 10.0s) ---
    // Master Wu: "A Venomari warrior has found a path into our home."
    else if (elapsed_ < 10.0f)
    {
        speaker = "Master Wu";
        dialogue = "\"A Venomari warrior has found a path into our home.\"";
        isKaiSpeaker = false;
        portrait = CharacterSprites::Character::WU;

        // Venomari slithers into the middle lane from the right with running/slither animation
        float progress = (elapsed_ - 8.0f) / 2.0f;
        float vX = 1200.0f - progress * 320.0f;
        CharacterSprites::drawVenomari(vX, laneY[1], 3.2f, CharacterSprites::State::RUN, elapsed_, false);
    }
    // --- BEAT 6 (10.0s - 12.0s) ---
    // Master Wu: "Kai, hold your ground. Do not let it reach the monastery gate."
    else if (elapsed_ < 12.0f)
    {
        speaker = "Master Wu";
        dialogue = "\"Kai, hold your ground. Do not let it reach the monastery gate.\"";
        isKaiSpeaker = false;
        portrait = CharacterSprites::Character::WU;

        // Kai dashes in with full running animation and plants his feet!
        float progress = std::fmin((elapsed_ - 10.0f) / 1.2f, 1.0f);
        float kX = 220.0f + progress * 240.0f;
        CharacterSprites::drawKai(kX, laneY[1], 3.2f, (progress < 1.0f) ? CharacterSprites::State::RUN : CharacterSprites::State::IDLE, elapsed_, true);

        // Keep the other defenders on their own lanes while Kai holds the
        // middle lane against the intruder.
        CharacterSprites::drawZane(320.0f, laneY[1], 3.2f, CharacterSprites::State::IDLE, elapsed_, true);
        CharacterSprites::drawCole(300.0f, laneY[2], 3.2f, CharacterSprites::State::IDLE, elapsed_, true);

        // Venomari stands threatening
        CharacterSprites::drawVenomari(880.0f, laneY[1], 3.2f, CharacterSprites::State::IDLE, elapsed_, false);
    }
    // --- BEAT 7 (12.0s - 14.0s) ---
    // Kai: "You won’t get past us!"
    else if (elapsed_ < 14.0f)
    {
        speaker = "Kai";
        dialogue = "\"You won't get past us!\"";
        isKaiSpeaker = true;
        portrait = CharacterSprites::Character::KAI;

        // Kai and the Venomari confront each other in the middle lane.  The
        // attack animation is intentionally limited to this dialogue beat;
        // there is no separate post-dialogue fight in Member 3's cutscene.
        CharacterSprites::drawKai(460.0f, laneY[1], 3.2f, CharacterSprites::State::ATTACK, elapsed_, true);
        CharacterSprites::drawVenomari(880.0f, laneY[1], 3.2f, CharacterSprites::State::ATTACK, elapsed_, false);
        CharacterSprites::drawZane(320.0f, laneY[1], 3.2f, CharacterSprites::State::RUN, elapsed_, true);
        CharacterSprites::drawCole(300.0f, laneY[2], 3.2f, CharacterSprites::State::RUN, elapsed_, true);
    }
    // --- BEAT 8 (14.0s - 16.0s) ---
    // Master Wu: "Stand together, ninja. Courage and unity will protect our weapons."
    else if (elapsed_ < 16.0f)
    {
        speaker = "Master Wu";
        dialogue = "\"Stand together, ninja. Courage and unity will protect our weapons.\"";
        isKaiSpeaker = false;
        portrait = CharacterSprites::Character::WU;

        // Each lane gets its own moving pair instead of stacking everyone on
        // one baseline.  Their RUN cycles visibly use the supplied guide's
        // contact/squash/passing/stretch rhythm.
        CharacterSprites::drawCole(280.0f, laneY[2], 3.2f, CharacterSprites::State::RUN, elapsed_, true);
        CharacterSprites::drawKai(460.0f, laneY[1], 3.2f, CharacterSprites::State::RUN, elapsed_, true);

        CharacterSprites::drawZane(370.0f, laneY[1], 3.2f, CharacterSprites::State::RUN, elapsed_, true);
    }
    // --- BEAT 9 (16.0s - 18.0s) ---
    // Master Wu: "The first wave is broken, but the battle has only begun. Defend our home together."
    else
    {
        speaker = "Master Wu";
        dialogue = "\"The first wave is broken, but the battle has only begun. Defend our home together.\"";
        isKaiSpeaker = false;

        // The final beat is a separated defensive formation, not a second
        // fight after the dialogue.  Characters remain animated but settle
        // into their lanes while the story transitions to gameplay.
        CharacterSprites::drawCole(280.0f, laneY[2], 3.2f, CharacterSprites::State::IDLE, elapsed_, true);
        CharacterSprites::drawKai(520.0f, laneY[1], 3.2f, CharacterSprites::State::IDLE, elapsed_, true);

        CharacterSprites::drawZane(370.0f, laneY[1], 3.2f, CharacterSprites::State::IDLE, elapsed_, true);

    }

    // =========================================================================
    // 3. DRAW RETRO POKEMON DIALOGUE BOX AT THE BOTTOM
    // =========================================================================
    drawRetroDialogueBox(speaker, dialogue, isKaiSpeaker, portrait);

    // =========================================================================
    // 4. RETRO SKIP & REPLAY CONTROL PILL (Top Right Corner)
    // =========================================================================
    M3Draw::quad(960.0f, 20.0f, 280.0f, 30.0f, 0.08f, 0.08f, 0.14f, 0.85f);
    M3Draw::quad(961.0f, 21.0f, 278.0f, 28.0f, 0.75f, 0.65f, 0.28f, 0.85f);
    M3Draw::quad(963.0f, 23.0f, 274.0f, 24.0f, 0.12f, 0.12f, 0.18f, 0.95f);
    M3Draw::text(975.0f, 29.0f, "SPACE / ESC : Skip  |  R : Replay", 1.4f, 1.0f, 0.92f, 0.45f);

    M3Draw::flush();
}
