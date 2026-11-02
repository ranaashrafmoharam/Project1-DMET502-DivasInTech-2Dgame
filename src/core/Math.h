#pragma once
// ====================================================================
// FILE:     src/core/Math.h
// OWNER:    Member 2 (minimal shared helper, agreed with Member 1)
// TASKS:    M2-01 (used by HierarchicalModel, CharacterModels, Animation)
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Tiny hand-written maths used by Member 2's character system so the
//   project does not need an external maths library (the course template
//   ships none).  Only what the 2D characters need:
//     - Vec2 / Vec3 / Vec4  (Vec4 is also used for colours)
//     - Mat4                (only for the orthographic projection uniform)
//     - Transform2D         (translate + rotate + uniform-scale, composed
//                            down the parent/child bone hierarchy)
//
// INTERACTS WITH: Entity.h, Renderer.h, HierarchicalModel, CharacterModels, Animation
// ====================================================================

#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------------------------------------------------------------- scalars
inline float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
inline float lerpf(float a, float b, float t)    { return a + (b - a) * t; }
inline float deg2rad(float d)                    { return d * (float)M_PI / 180.0f; }
inline float radians(float d)                    { return d * (float)M_PI / 180.0f; }

// ------------------------------------------------------------------- Vec2
struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float x_, float y_) : x(x_), y(y_) {}

    Vec2 operator+(const Vec2& o) const { return { x + o.x, y + o.y }; }
    Vec2 operator-(const Vec2& o) const { return { x - o.x, y - o.y }; }
    Vec2 operator*(float s)       const { return { x * s,   y * s   }; }
    Vec2 operator*(const Vec2& o) const { return { x * o.x, y * o.y }; }  // component-wise
    Vec2 operator/(float s)       const { return { x / s,   y / s   }; }
    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }

    float length() const        { return std::sqrt(x * x + y * y); }
    Vec2  normalized() const    { float l = length(); return l > 1e-6f ? Vec2{ x / l, y / l } : Vec2{ 0, 0 }; }
    float dot(const Vec2& o) const { return x * o.x + y * o.y; }
};

inline Vec2 lerp(const Vec2& a, const Vec2& b, float t) { return { lerpf(a.x, b.x, t), lerpf(a.y, b.y, t) }; }

// ------------------------------------------------------------------- Vec3
struct Vec3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;
    Vec3() = default;
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

// ------------------------------------------------------------------- Vec4
// Also used for colours: x=r, y=g, z=b, w=a (all 0..1).
struct Vec4 {
    float x = 1.0f, y = 1.0f, z = 1.0f, w = 1.0f;
    Vec4() = default;
    Vec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
    Vec4 operator*(float s) const { return { x * s, y * s, z * s, w }; }
};

inline Vec4 lerp(const Vec4& a, const Vec4& b, float t) {
    return { lerpf(a.x, b.x, t), lerpf(a.y, b.y, t), lerpf(a.z, b.z, t), lerpf(a.w, b.w, t) };
}

// ------------------------------------------------------------------- Mat4
// Column-major, exactly like OpenGL expects (m[col * 4 + row]).
struct Mat4 {
    float m[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

    static Mat4 identity() { return Mat4{}; }

    // a * b : applies b first, then a (standard matrix composition).
    Mat4 operator*(const Mat4& b) const {
        Mat4 r;
        for (int c = 0; c < 4; ++c)
            for (int row = 0; row < 4; ++row) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k)
                    sum += m[k * 4 + row] * b.m[c * 4 + k];
                r.m[c * 4 + row] = sum;
            }
        return r;
    }

    static Mat4 translate(float tx, float ty, float tz = 0.0f) {
        Mat4 r; r.m[12] = tx; r.m[13] = ty; r.m[14] = tz; return r;
    }
    static Mat4 scale(float sx, float sy, float sz = 1.0f) {
        Mat4 r; r.m[0] = sx; r.m[5] = sy; r.m[10] = sz; return r;
    }
    static Mat4 rotateZ(float radians_) {
        Mat4 r; float c = std::cos(radians_), s = std::sin(radians_);
        r.m[0] = c; r.m[1] = s; r.m[4] = -s; r.m[5] = c; return r;
    }
    // 2D orthographic projection, because the game is viewed from the side.
    static Mat4 ortho(float left, float right, float bottom, float top, float nearZ = -1.0f, float farZ = 1.0f) {
        Mat4 r;
        r.m[0]  =  2.0f / (right - left);
        r.m[5]  =  2.0f / (top - bottom);
        r.m[10] = -2.0f / (farZ - nearZ);
        r.m[12] = -(right + left) / (right - left);
        r.m[13] = -(top + bottom) / (top - bottom);
        r.m[14] = -(farZ + nearZ) / (farZ - nearZ);
        r.m[15] =  1.0f;
        return r;
    }

    // Transform a 2D point (z = 0) and return the transformed 2D point.
    Vec2 transformPoint(const Vec2& p) const {
        return { m[0] * p.x + m[4] * p.y + m[12],
                 m[1] * p.x + m[5] * p.y + m[13] };
    }
};

// -------------------------------------------------------------- Transform2D
// A lightweight 2D bone transform: scale -> rotate -> translate.
// Child bones are composed with their parent so limbs follow the body.
struct Transform2D {
    Vec2  translation{ 0.0f, 0.0f };
    float rotation = 0.0f;          // radians
    Vec2  scale{ 1.0f, 1.0f };

    Transform2D() = default;
    Transform2D(const Vec2& t, float rot = 0.0f, const Vec2& s = { 1.0f, 1.0f })
        : translation(t), rotation(rot), scale(s) {}

    // Local point -> transformed point.
    Vec2 apply(const Vec2& p) const {
        Vec2 s{ p.x * scale.x, p.y * scale.y };
        float c = std::cos(rotation), sn = std::sin(rotation);
        return { translation.x + s.x * c - s.y * sn,
                 translation.y + s.x * sn + s.y * c };
    }

    // Compose: the returned transform is "parent then child".
    static Transform2D compose(const Transform2D& parent, const Transform2D& child) {
        Transform2D out;
        out.rotation    = parent.rotation + child.rotation;
        out.scale       = parent.scale * child.scale;
        out.translation = parent.apply(child.translation);
        return out;
    }

    Mat4 toMat4() const {
        return Mat4::translate(translation.x, translation.y, 0.0f)
             * Mat4::rotateZ(rotation)
             * Mat4::scale(scale.x, scale.y, 1.0f);
    }
};
