// ====================================================================
// FILE:     dev/m2_verify.cpp
// OWNER:    Member 2 (development-only test harness)
// PURPOSE:
//   A standalone, headless check of everything Member 2 owns.  It needs
//   no GPU and no window: it installs a software Renderer that
//   rasterises the same draw calls an OpenGL renderer would receive, so
//   the six characters can actually be SEEN (as ASCII art and as PPM
//   images in dev/out/) and every user story can be asserted numerically.
//
//   This file is NOT part of the game and is NOT part of the submission
//   source set - it exists so Member 2 can prove the work runs.  Build
//   it with dev/build_m2_verify.ps1 (or the g++ line inside it).
//
// INTERACTS WITH: every src/entities file Member 2 owns
// ====================================================================

#include "../src/core/Renderer.h"
#include "../src/core/Constants.h"
#include "../src/entities/Player.h"
#include "../src/entities/Enemy.h"
#include "../src/entities/Projectile.h"
#include "../src/entities/CharacterModels.h"
#include "../src/entities/Animation.h"
#include "../src/audio/AudioManager.h"

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <filesystem>

// --------------------------------------------------------------------
// A software Renderer: turns every draw call into pixels / ASCII so the
// characters can be verified without OpenGL.  It applies the damage
// tint and grayscale exactly like the real shader is meant to.
// --------------------------------------------------------------------
class RasterRenderer : public Renderer {
public:
    RasterRenderer(int w, int h) : W(w), H(h), px(size_t(w) * h * 3, 0) {}

    void clear(const Vec4& c) {
        unsigned char r = b(c.x), g = b(c.y), bl = b(c.z);
        for (size_t i = 0; i < px.size(); i += 3) { px[i] = r; px[i + 1] = g; px[i + 2] = bl; }
    }

    void drawQuad(const Vec2& center, const Vec2& size, const Vec4& colour, float rotation = 0.0f) override {
        const float hx = size.x * 0.5f, hy = size.y * 0.5f;
        std::vector<Vec2> pts{ { -hx, -hy }, { hx, -hy }, { hx, hy }, { -hx, hy } };
        fillLocal(pts, colour, rotation, center);
    }
    void drawCircle(const Vec2& center, float radius, const Vec4& colour, int segments = 20) override {
        int n = segments < 10 ? 10 : segments;
        std::vector<Vec2> pts;
        pts.reserve(n);
        for (int i = 0; i < n; ++i) {
            float a = 2.0f * 3.14159265f * (float)i / (float)n;
            pts.push_back({ std::cos(a) * radius, std::sin(a) * radius });
        }
        fillLocal(pts, colour, 0.0f, center);
    }
    void drawTriangle(const Vec2& a, const Vec2& bb, const Vec2& c, const Vec4& colour) override {
        fillLocal({ a, bb, c }, colour, 0.0f, { 0, 0 });
    }
    void drawTrapezoid(const Vec2& center, const Vec2& size, float topWidthScale,
                       const Vec4& colour, float rotation = 0.0f) override {
        const float hx = size.x * 0.5f, hy = size.y * 0.5f;
        const float tx = hx * topWidthScale;
        std::vector<Vec2> pts{ { -hx, -hy }, { hx, -hy }, { tx, hy }, { -tx, hy } };
        fillLocal(pts, colour, rotation, center);
    }

    // ---- output ------------------------------------------------------
    void writePPM(const std::string& path) const {
        FILE* f = std::fopen(path.c_str(), "wb");
        if (!f) return;
        std::fprintf(f, "P6\n%d %d\n255\n", W, H);
        std::fwrite(px.data(), 1, px.size(), f);
        std::fclose(f);
    }

    // Brightness-ramp ASCII so shape and colour differences both show.
    std::string toAscii(int cols, int rows) const {
        static const char* ramp = " .:-=+*#%@";
        const int rampLen = 10;
        std::string out;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                int x = (int)((c + 0.5f) / cols * W);
                int y = (int)((r + 0.5f) / rows * H);
                if (x < 0 || x >= W || y < 0 || y >= H) { out += ' '; continue; }
                size_t i = (size_t)(y * W + x) * 3;
                float lum = (0.299f * px[i] + 0.587f * px[i + 1] + 0.114f * px[i + 2]) / 255.0f;
                int k = (int)(lum * (rampLen - 1) + 0.5f);
                out += ramp[k];
            }
            out += '\n';
        }
        return out;
    }

private:
    static unsigned char b(float v) {
        int i = (int)(v * 255.0f + 0.5f);
        return (unsigned char)(i < 0 ? 0 : (i > 255 ? 255 : i));
    }

    Vec4 resolve(const Vec4& c) const {
        Vec4 o = c;
        if (isGrayscale()) {
            float l = 0.299f * o.x + 0.587f * o.y + 0.114f * o.z;
            o = { l, l, l, o.w };
        }
        float a = getTintAmount();
        if (a > 0.0f) o = lerp(o, getTint(), a);
        return o;
    }

    // local part space -> world (through the transform stack) -> pixels
    void fillLocal(const std::vector<Vec2>& pts, const Vec4& colour, float rot, const Vec2& offset) {
        if (pts.size() < 3) return;
        std::vector<Vec2> world;
        world.reserve(pts.size());
        for (const Vec2& p : pts) {
            Vec2 q = p;
            if (rot != 0.0f) {
                float c = std::cos(rot), s = std::sin(rot);
                q = { p.x * c - p.y * s, p.x * s + p.y * c };
            }
            q = q + offset;
            world.push_back(top().apply(q));
        }
        fillWorldPolygon(world, resolve(colour));
    }

    static bool inside(const std::vector<Vec2>& p, float x, float y) {
        bool in = false;
        for (size_t i = 0, j = p.size() - 1; i < p.size(); j = i++) {
            if (((p[i].y > y) != (p[j].y > y)) &&
                (x < (p[j].x - p[i].x) * (y - p[i].y) / (p[j].y - p[i].y) + p[i].x))
                in = !in;
        }
        return in;
    }

    void setPx(int x, int y, const Vec4& c) {
        if (x < 0 || x >= W || y < 0 || y >= H) return;
        size_t i = (size_t)(y * W + x) * 3;
        px[i] = b(c.x); px[i + 1] = b(c.y); px[i + 2] = b(c.z);
    }

    void fillWorldPolygon(const std::vector<Vec2>& wpts, const Vec4& col) {
        std::vector<Vec2> pp;
        pp.reserve(wpts.size());
        float minx = 1e9f, maxx = -1e9f, miny = 1e9f, maxy = -1e9f;
        for (const Vec2& w : wpts) {
            float x = (w.x / C::WORLD_WIDTH) * (float)W;
            float y = (1.0f - w.y / C::WORLD_HEIGHT) * (float)H;
            pp.push_back({ x, y });
            minx = std::min(minx, x); maxx = std::max(maxx, x);
            miny = std::min(miny, y); maxy = std::max(maxy, y);
        }
        int x0 = std::max(0, (int)std::floor(minx));
        int x1 = std::min(W - 1, (int)std::ceil(maxx));
        int y0 = std::max(0, (int)std::floor(miny));
        int y1 = std::min(H - 1, (int)std::ceil(maxy));
        for (int y = y0; y <= y1; ++y)
            for (int x = x0; x <= x1; ++x)
                if (inside(pp, x + 0.5f, y + 0.5f)) setPx(x, y, col);
    }

    int W, H;
    std::vector<unsigned char> px;
};

// --------------------------------------------------------------------
// A recording AudioManager so US-56 can be asserted (no real audio here).
// --------------------------------------------------------------------
class RecordingAudio : public AudioManager {
public:
    std::vector<SoundId> played;
    void playSound(SoundId id) override { played.push_back(id); }
    int count(SoundId id) const {
        return (int)std::count(played.begin(), played.end(), id);
    }
    void reset() { played.clear(); }
};

// --------------------------------------------------------------------
// Test bookkeeping
// --------------------------------------------------------------------
static int g_pass = 0, g_fail = 0;
static void check(bool ok, const std::string& what) {
    std::printf("   [%s] %s\n", ok ? "PASS" : "FAIL", what.c_str());
    if (ok) ++g_pass; else ++g_fail;
}
static void section(const std::string& s) {
    std::printf("\n=== %s ===\n", s.c_str());
}

static const char* typeName(UnitType t) { return unitTypeName(t); }

// Step a player forward by `seconds` at 60 fps.
static void run(Player& p, float seconds) {
    const float dt = 1.0f / 60.0f;
    for (float t = 0; t < seconds; t += dt) p.update(dt);
}
static void run(Enemy& e, float seconds) {
    const float dt = 1.0f / 60.0f;
    for (float t = 0; t < seconds; t += dt) e.update(dt);
}
static void run(Projectile& p, float seconds) {
    const float dt = 1.0f / 60.0f;
    for (float t = 0; t < seconds; t += dt) p.update(dt);
}

// ====================================================================
int main() {
    std::filesystem::create_directories("dev/out");

    RasterRenderer rr(960, 540);
    setRenderer(&rr);
    RecordingAudio audio;
    setAudio(&audio);

    // ----------------------------------------------------------------
    section("US-28 / US-38  Distinct, multi-part designs");
    {
        HierarchicalModel hierarchy;
        PartShapeDef shape;
        check(hierarchy.addPart("orphan", "missing", shape) == -1 &&
              hierarchy.partCount() == 0, "hierarchy rejects a missing parent instead of making a root part");

        const UnitType types[6] = {
            UnitType::MeleePlayer, UnitType::RangedPlayer, UnitType::DefensePlayer,
            UnitType::EasyEnemy,   UnitType::MediumEnemy, UnitType::HardEnemy
        };
        std::set<std::string> signatures;
        bool allMultiPart = true;
        bool playersOk = true, enemiesOk = true;

        for (UnitType t : types) {
            HierarchicalModel m = CharacterModels::buildForType(t);

            std::printf("   %-22s parts=%d\n", typeName(t), m.partCount());
            if (m.partCount() < 6) allMultiPart = false;

            // Player vs enemy structural difference.
            bool isPlayerSide = (t == UnitType::MeleePlayer || t == UnitType::RangedPlayer || t == UnitType::DefensePlayer);
            bool hasMaskish = m.hasPart("mask") || m.hasPart("helmet");
            bool hasSnakeish = m.hasPart("tailBase") || m.hasPart("fangL") || m.hasPart("hornL") || m.hasPart("jaw");
            if (isPlayerSide) { if (!hasMaskish) playersOk = false; }
            else              { if (!hasSnakeish || hasMaskish) enemiesOk = false; }
        }

        check(allMultiPart, "all six models are built from multiple parts (US-28/US-38)");
        check(playersOk,    "every player has ninja headgear (mask/helmet)");
        check(enemiesOk,    "every enemy is serpentine (tail/fangs/horns) and shares no player headgear");

        // The six models must have different part counts / part sets.
        std::set<int> counts;
        for (UnitType t : types) counts.insert(CharacterModels::buildForType(t).partCount());
        check(counts.size() >= 3, "models differ in composition (not just colour)");
    }

    // ----------------------------------------------------------------
    section("US-25 / US-26 / US-27 / US-35 / US-36 / US-37  Stats");
    {
        struct Row { UnitType t; float hp, dmg, spd; bool ranged; AttackHeight h; };
        const Row rows[6] = {
            { UnitType::MeleePlayer,   5, 1, C::PLAYER_SPEED,     false, AttackHeight::High },
            { UnitType::RangedPlayer,  5, 1, C::PLAYER_SPEED,     true,  AttackHeight::High },
            { UnitType::DefensePlayer, 7, 0, C::DEFENSE_SPEED,    false, AttackHeight::High },
            { UnitType::EasyEnemy,     5, 1, C::ENEMY_SPEED,      true,  AttackHeight::High },
            { UnitType::MediumEnemy,   5, 1, C::ENEMY_SPEED,      true,  AttackHeight::Low  },
            { UnitType::HardEnemy,     7, 2, C::HARD_ENEMY_SPEED, false, AttackHeight::High },
        };

        bool ok = true;
        for (const Row& r : rows) {
            if (r.t == UnitType::DefensePlayer) {
                Player p; p.init(r.t, 0, C::PLAYER_SPAWN_X);
                ok &= (p.maxHealth == r.hp && p.attackDamage() == r.dmg && p.isStationary());
            } else if (r.t == UnitType::MeleePlayer || r.t == UnitType::RangedPlayer) {
                Player p; p.init(r.t, 0, C::PLAYER_SPAWN_X);
                ok &= (p.maxHealth == r.hp && p.attackDamage() == r.dmg && p.isRanged() == r.ranged);
            } else {
                Enemy e; e.init(r.t, 0, C::ENEMY_SPAWN_X);
                ok &= (e.maxHealth == r.hp && e.attackDamage() == r.dmg &&
                       e.moveSpeedValue() == r.spd && e.isRanged() == r.ranged &&
                       e.attackHeight() == r.h);
            }
            std::printf("   %-22s hp=%.0f dmg=%.0f speed=%.0f ranged=%d\n",
                        typeName(r.t), r.hp, r.dmg, r.spd, (int)r.ranged);
        }
        check(ok, "health / damage / speed / ranged / attack-height all match the spec");

        // Hard enemy is strictly faster than the other two (US-37).
        Enemy easy, med, hard;
        easy.init(UnitType::EasyEnemy, 0); med.init(UnitType::MediumEnemy, 1); hard.init(UnitType::HardEnemy, 2);
        check(hard.moveSpeedValue() > easy.moveSpeedValue() && hard.moveSpeedValue() > med.moveSpeedValue(),
              "Hard enemy runs faster than Easy and Medium (US-37)");
    }

    // ----------------------------------------------------------------
    section("US-25 / US-26  Automatic advance");
    {
        Player melee; melee.init(UnitType::MeleePlayer, 0, C::PLAYER_SPAWN_X);
        Player ranged; ranged.init(UnitType::RangedPlayer, 1, C::PLAYER_SPAWN_X);
        Player def; def.init(UnitType::DefensePlayer, 2, C::PLAYER_SPAWN_X);

        float m0 = melee.position.x, r0 = ranged.position.x, d0 = def.position.x;
        run(melee, 2.0f); run(ranged, 2.0f); run(def, 2.0f);

        check(melee.position.x > m0 + 50.0f, "Melee player advances to the right (US-25)");
        check(ranged.position.x > r0 + 50.0f, "Ranged player advances to the right (US-26)");
        check(std::fabs(def.position.x - d0) < 0.001f, "Defense player never moves after placement (US-27)");

        // They attack on their own, repeatedly, with no enemies present.
        // Count the swings each one starts by itself over three seconds.
        const float dt = 1.0f / 60.0f;
        Player soloM; soloM.init(UnitType::MeleePlayer, 0, C::PLAYER_SPAWN_X);
        Player soloR; soloR.init(UnitType::RangedPlayer, 1, C::PLAYER_SPAWN_X);
        int mSwings = 0, rSwings = 0;
        for (float t = 0; t < 3.0f; t += dt) {
            soloM.update(dt); soloR.update(dt);
            if (soloM.claimAttack()) ++mSwings;   // true once per swing
            if (soloR.claimAttack()) ++rSwings;
        }
        check(mSwings >= 2, "Melee keeps attacking on its own (US-25)");
        check(rSwings >= 2, "Ranged keeps attacking on its own (US-26)");
        check(!def.isAttacking(), "Defense never attacks (US-27)");
    }

    // ----------------------------------------------------------------
    section("Attack timing and projectile team checks");
    {
        Player melee; melee.init(UnitType::MeleePlayer, 0, C::PLAYER_SPAWN_X);
        melee.update(1.0f / 60.0f);
        check(!melee.claimAttack(), "damage is not claimed during the attack wind-up");
        run(melee, 0.16f);
        check(melee.claimAttack(), "damage is claimed during the weapon strike");
        check(!melee.claimAttack(), "one attack swing can only deal damage once");

        Enemy enemy; enemy.init(UnitType::HardEnemy, 0, C::ENEMY_SPAWN_X);
        enemy.update(1.0f / 60.0f);
        check(!enemy.claimAttack(), "enemy damage is not claimed during the attack wind-up");
        run(enemy, 0.16f);
        check(enemy.claimAttack(), "enemy damage is claimed during the weapon strike");
        check(!enemy.claimAttack(), "one enemy attack can only deal damage once");

        Player ranged; ranged.init(UnitType::RangedPlayer, 0, C::PLAYER_SPAWN_X);
        Player teammate; teammate.init(UnitType::MeleePlayer, 0, C::PLAYER_SPAWN_X + 40.0f);
        Enemy target; target.init(UnitType::EasyEnemy, 0, C::ENEMY_SPAWN_X);
        Projectile shot;
        shot.spawn(ranged.muzzle(), ranged.laneIndex, Faction::Player, UnitType::RangedPlayer,
                   ranged.attackHeight(), ranged.facing(), ranged.attackDamage());
        check(ranged.attackHeight() == AttackHeight::High, "Ranged player attack has a HIGH dodge tag");
        check(!shot.canHit(ranged) && !shot.canHit(teammate),
              "player projectiles cannot hit their owner or other players");
        check(shot.canHit(target), "player projectiles can hit an enemy");
    }

    // ----------------------------------------------------------------
    section("US-29  Jump  /  US-30  Duck  /  US-31  Defense ignores both");
    {
        Player p; p.init(UnitType::MeleePlayer, 0, C::PLAYER_SPAWN_X);
        p.jump();
        check(p.isJumping(), "jump() starts a jump");
        float peak = 0.0f;
        const float dt = 1.0f / 60.0f;
        for (int i = 0; i < 60; ++i) { p.update(dt); peak = std::max(peak, p.jumpHeight()); }
        check(peak > C::JUMP_HEIGHT * 0.9f, "jump arc rises to roughly JUMP_HEIGHT (visible arc)");
        check(!p.isJumping() && p.jumpHeight() == 0.0f, "jump ends back on the ground");

        p.duck();
        p.update(dt);
        check(p.isDucking(), "duck() puts the player in the duck pose");
        float duckHeight = p.size.y;
        check(duckHeight < C::CHARACTER_HEIGHT * 0.95f, "ducking shrinks the hitbox (a high attack misses)");

        Player lockout; lockout.init(UnitType::MeleePlayer, 0, C::PLAYER_SPAWN_X);
        lockout.jump();
        run(lockout, C::JUMP_DURATION + 0.05f);
        lockout.jump();
        check(!lockout.isJumping(), "jump cannot be restarted during its lockout");
        run(lockout, 0.30f);
        lockout.jump();
        check(lockout.isJumping(), "jump can be used again after its lockout");

        Player duckLockout; duckLockout.init(UnitType::MeleePlayer, 0, C::PLAYER_SPAWN_X);
        duckLockout.duck();
        run(duckLockout, C::DUCK_DURATION + 0.05f);
        duckLockout.duck();
        check(!duckLockout.isDucking(), "duck cannot be restarted during its lockout");
        run(duckLockout, 0.30f);
        duckLockout.duck();
        check(duckLockout.isDucking(), "duck can be used again after its lockout");

        // Defense ignores jump and duck entirely (US-31).
        Player d; d.init(UnitType::DefensePlayer, 0, C::PLAYER_SPAWN_X);
        d.jump(); d.duck();
        run(d, 0.3f);
        check(!d.isJumping() && !d.isDucking(), "Defense ignores jump and duck (US-31)");
    }

    // ----------------------------------------------------------------
    section("US-26 / US-35 / US-36  Projectiles: travel, tag, disappear");
    {
        // Ranged player's shot.
        Player zane; zane.init(UnitType::RangedPlayer, 0, C::PLAYER_SPAWN_X);
        run(zane, 0.2f);
        Vec2 muzzle = zane.muzzle();
        Projectile shot;
        shot.spawn(muzzle, 0, Faction::Player, UnitType::RangedPlayer, AttackHeight::High, +1.0f, zane.attackDamage());
        float x0 = shot.position.x;
        run(shot, 0.5f);
        check(shot.position.x > x0 + 100.0f, "player shot travels to the right (US-26)");
        check(shot.getBounds().width > 0.0f && shot.getBounds().height > 0.0f, "shot exposes a hitbox (US-42)");
        Player teammate; teammate.init(UnitType::MeleePlayer, 0, C::PLAYER_SPAWN_X);
        Enemy target; target.init(UnitType::EasyEnemy, 0, C::ENEMY_SPAWN_X);
        check(!shot.canHit(teammate), "a Ranged Player shot cannot harm another player");
        check(shot.canHit(target), "a Ranged Player shot can harm an enemy");
        shot.onHit();
        check(!shot.isAlive(), "shot disappears on collision (US-26 / US-42)");

        // Easy enemy: HIGH, from above.
        Enemy easy; easy.init(UnitType::EasyEnemy, 0, C::ENEMY_SPAWN_X);
        easy.update(1.0f / 60.0f);
        Vec2 hi = easy.muzzle();
        check(easy.attackHeight() == AttackHeight::High, "Easy enemy attack is tagged HIGH (US-35)");

        // Medium enemy: LOW, from below.  Must be lower than the HIGH shot.
        Enemy med; med.init(UnitType::MediumEnemy, 0, C::ENEMY_SPAWN_X);
        med.update(1.0f / 60.0f);
        Vec2 lo = med.muzzle();
        check(med.attackHeight() == AttackHeight::Low, "Medium enemy attack is tagged LOW (US-36)");
        check(hi.y > lo.y + 30.0f, "HIGH shot starts above the head, LOW shot skims the ground");

        Projectile eshot;
        eshot.spawn(hi, 0, Faction::Enemy, UnitType::EasyEnemy, AttackHeight::High, -1.0f, 1.0f);
        float ex0 = eshot.position.x;
        run(eshot, 0.5f);
        check(eshot.position.x < ex0 - 100.0f, "enemy shot travels to the left");
    }

    // ----------------------------------------------------------------
    section("US-41  Combat  /  US-45  Removal  /  US-46  Orb drop  /  US-56  Sounds");
    {
        Player kai; kai.init(UnitType::MeleePlayer, 0, C::PLAYER_SPAWN_X);
        Enemy hard; hard.init(UnitType::HardEnemy, 0, C::PLAYER_SPAWN_X + 40.0f);

        audio.reset();
        const float dt = 1.0f / 60.0f;

        int hits = 0;
        float swingAcc = 0.0f;
        // Simulate a fight: one damage per swing until the brute falls.
        for (int i = 0; i < 2000 && hard.isAlive(); ++i) {
            kai.update(dt);
            hard.update(dt);
            swingAcc += dt;
            if (swingAcc >= dt) {               // every frame we may claim one hit
                if (kai.claimAttack()) { hard.takeDamage(kai.attackDamage()); ++hits; }
            }
        }
        check(hard.health == 0.0f && !hard.isAlive(), "Hard enemy (7 lives) is defeated after 7 hits of 1");
        check(hits == 7, "exactly 7 damage events landed on the 7-life brute");
        check(hard.isDead(), "US-45: a defeated enemy reports dead so the Game removes it immediately");
        check(hard.consumeOrbDrop(), "US-46: the defeated enemy drops a collectible (once)");
        check(!hard.consumeOrbDrop(), "the drop is only reported once");

        check(audio.count(SoundId::SwordSwing) > 0, "US-56: melee attack sound played");
        check(audio.count(SoundId::Hit) > 0,        "US-56: hit sound played");
        check(audio.count(SoundId::Death) > 0,      "US-56: death sound played");

        // A shielded player shrugs off damage (US-32), then becomes
        // vulnerable again once the shield is gone.
        Player guarded; guarded.init(UnitType::MeleePlayer, 1, C::PLAYER_SPAWN_X);
        guarded.setShielded(true);
        guarded.takeDamage(3);
        check(guarded.health == guarded.maxHealth, "US-32: a shielded player takes no damage");
        guarded.setShielded(false);
        guarded.takeDamage(2);
        check(guarded.health == guarded.maxHealth - 2.0f, "once unprotected the player is vulnerable again");

        // Defense absorbs 7 hits and is then removed (US-27).
        Player cole; cole.init(UnitType::DefensePlayer, 2, C::PLAYER_SPAWN_X);
        for (int i = 0; i < 7; ++i) cole.takeDamage(1);
        check(!cole.isAlive(), "US-27: Defense absorbs 7 hits and is removed");
    }

    // ----------------------------------------------------------------
    section("Visual 1: the six characters side by side");
    {
        const UnitType types[6] = {
            UnitType::MeleePlayer, UnitType::RangedPlayer, UnitType::DefensePlayer,
            UnitType::EasyEnemy,   UnitType::MediumEnemy, UnitType::HardEnemy
        };
        rr.beginFrame();
        rr.setDefaultOrtho();
        rr.clear(Vec4{ 0.09f, 0.11f, 0.16f, 1.0f });

        // A ground line so the feet line is obvious.
        rr.drawQuad({ 640.0f, 300.0f }, { 1280.0f, 6.0f }, Vec4{ 0.30f, 0.32f, 0.38f, 1.0f });

        for (int i = 0; i < 6; ++i) {
            HierarchicalModel m = CharacterModels::buildForType(types[i]);
            float x = 150.0f + (float)i * 200.0f;
            bool enemy = (i >= 3);
            m.setRootTransform(Transform2D({ x, 300.0f }, 0.0f, { enemy ? -1.0f : 1.0f, 1.0f }));
            m.draw(rr);
        }
        rr.writePPM("dev/out/01_lineup.ppm");
        std::printf("%s", rr.toAscii(150, 40).c_str());
        std::printf("   -> wrote dev/out/01_lineup.ppm\n");
    }

    // ----------------------------------------------------------------
    section("Visual 2: Kai's melee attack swing (5 frames, left to right)");
    {
        rr.beginFrame();
        rr.setDefaultOrtho();
        rr.clear(Vec4{ 0.09f, 0.11f, 0.16f, 1.0f });
        rr.drawQuad({ 640.0f, 300.0f }, { 1280.0f, 6.0f }, Vec4{ 0.30f, 0.32f, 0.38f, 1.0f });

        for (int i = 0; i < 5; ++i) {
            HierarchicalModel m = CharacterModels::buildMeleePlayer();
            float phase = (float)i / 4.0f;
            Animation::updateAttack(m, 0.0f, phase, false);
            m.setRootTransform(Transform2D({ 180.0f + (float)i * 230.0f, 300.0f }, 0.0f, { 1.0f, 1.0f }));
            m.draw(rr);
        }
        rr.writePPM("dev/out/02_attack_strip.ppm");
        std::printf("%s", rr.toAscii(150, 40).c_str());
        std::printf("   -> wrote dev/out/02_attack_strip.ppm\n");
    }

    // ----------------------------------------------------------------
    section("Visual 3: walk (top row) / jump / duck (bottom row)");
    {
        rr.beginFrame();
        rr.setDefaultOrtho();
        rr.clear(Vec4{ 0.09f, 0.11f, 0.16f, 1.0f });
        rr.drawQuad({ 640.0f, 300.0f }, { 1280.0f, 6.0f }, Vec4{ 0.30f, 0.32f, 0.38f, 1.0f });

        // Walk cycle frames.
        for (int i = 0; i < 4; ++i) {
            HierarchicalModel m = CharacterModels::buildMeleePlayer();
            Animation::updateWalk(m, (float)i * 0.26f);
            m.setRootTransform(Transform2D({ 200.0f + (float)i * 240.0f, 300.0f }, 0.0f, { 1.0f, 1.0f }));
            m.draw(rr);
        }
        // Jump arc frames.
        for (int i = 0; i < 3; ++i) {
            HierarchicalModel m = CharacterModels::buildRangedPlayer();
            float phase = (float)i / 2.0f;
            Animation::updateJump(m, phase);
            float lift = C::JUMP_HEIGHT * std::sin(3.14159265f * phase);
            m.setRootTransform(Transform2D({ 200.0f + (float)i * 240.0f, 150.0f + lift }, 0.0f, { 1.0f, 1.0f }));
            m.draw(rr);
        }
        // Duck poses, barely above the lower ground line.
        for (int i = 0; i < 3; ++i) {
            HierarchicalModel m = CharacterModels::buildRangedPlayer();
            Animation::updateDuck(m, 1.0f);
            (void)i;
            m.setRootTransform(Transform2D({ 920.0f + (float)i * 120.0f, 150.0f }, 0.0f, { 1.0f, 1.0f }));
            m.draw(rr);
        }
        rr.writePPM("dev/out/03_locomotion.ppm");
        std::printf("%s", rr.toAscii(150, 40).c_str());
        std::printf("   -> wrote dev/out/03_locomotion.ppm\n");
    }

    // ----------------------------------------------------------------
    section("Visual 4: a small battle scene with a shot in flight");
    {
        rr.beginFrame();
        rr.setDefaultOrtho();
        rr.clear(Vec4{ 0.09f, 0.11f, 0.16f, 1.0f });

        // Four lane strips.
        for (int lane = 0; lane < C::LANE_COUNT; ++lane) {
            float y = C::laneCenterY(lane);
            Vec4 c = (lane % 2 == 0) ? Vec4{ 0.16f, 0.18f, 0.24f, 1.0f } : Vec4{ 0.13f, 0.15f, 0.20f, 1.0f };
            rr.drawQuad({ 640.0f, y }, { 1280.0f, C::LANE_HEIGHT - 4.0f }, c);
        }

        Player kai; kai.init(UnitType::MeleePlayer, 1, 380.0f);
        Player zane; zane.init(UnitType::RangedPlayer, 2, 300.0f);
        Player cole; cole.init(UnitType::DefensePlayer, 0, 260.0f);
        Enemy easy; easy.init(UnitType::EasyEnemy, 2, 900.0f);
        Enemy med; med.init(UnitType::MediumEnemy, 1, 760.0f);
        Enemy hard; hard.init(UnitType::HardEnemy, 3, 980.0f);

        // Advance them all a moment so poses differ.
        run(kai, 0.35f); run(zane, 0.5f); run(cole, 0.2f);
        run(easy, 0.4f); run(med, 0.7f); run(hard, 0.3f);

        Projectile shot;
        shot.spawn(zane.muzzle(), zane.laneIndex, Faction::Player, UnitType::RangedPlayer,
                   AttackHeight::High, +1.0f, 1.0f);
        run(shot, 0.25f);

        kai.draw(); zane.draw(); cole.draw(); easy.draw(); med.draw(); hard.draw();
        shot.draw();

        rr.writePPM("dev/out/04_battle.ppm");
        std::printf("%s", rr.toAscii(150, 40).c_str());
        std::printf("   -> wrote dev/out/04_battle.ppm\n");
        check(shot.isAlive(), "the shot in the scene is still in flight");
    }

    // ----------------------------------------------------------------
    section("Poses actually move the bones");
    {
        HierarchicalModel m = CharacterModels::buildMeleePlayer();
        m.setRootTransform(Transform2D({ 400.0f, 300.0f }, 0.0f, { 1.0f, 1.0f }));

        // A bone's own origin does not move when the bone merely rotates
        // about it, so we sample a point offset from the origin (the foot
        // below the hip, the blade tip) to see real, visible movement.
        auto sample = [&](const char* part, float localX, float localY) {
            int i = m.indexOf(part);
            return m.worldTransform(i).apply({ localX, localY });
        };

        // Walk: legL swings to its widest at phase ~pi/2 (t ~= 0.26 s).
        Animation::updateWalk(m, 0.0f);
        Vec2 a = sample("legL", 0.0f, -15.0f);
        Animation::updateWalk(m, 0.26f);
        Vec2 b = sample("legL", 0.0f, -15.0f);
        check((a - b).length() > 1.0f, "the walk cycle moves the leg bone (visible animation)");

        // Attack: sample the sword at rest (phase 0) and at the strike peak.
        m.resetPoses();
        Animation::updateAttack(m, 0.0f, 0.0f, false);
        Vec2 w1 = sample("blade", 0.0f, 0.0f);
        Animation::updateAttack(m, 0.0f, 0.55f, false);
        Vec2 w2 = sample("blade", 0.0f, 0.0f);
        check((w1 - w2).length() > 10.0f, "the attack swing moves the sword in the hand (US-25)");

        m.resetPoses();
        Vec2 h1 = m.partWorldPosition("head");
        Animation::updateDuck(m, 1.0f);
        Vec2 h2 = m.partWorldPosition("head");
        check(h2.y < h1.y - 5.0f, "ducking lowers the head (US-30)");
    }

    // ----------------------------------------------------------------
    std::printf("\n============================================\n");
    std::printf("  Member 2 verification: %d passed, %d failed\n", g_pass, g_fail);
    std::printf("  Images written to dev/out/*.ppm (open with any image viewer,\n");
    std::printf("  or convert with: magick dev/out/01_lineup.ppm 01_lineup.png)\n");
    std::printf("============================================\n");
    return g_fail == 0 ? 0 : 1;
}
