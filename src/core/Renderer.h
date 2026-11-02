#pragma once
// ====================================================================
// FILE:     src/core/Renderer.h
// OWNER:    Member 1
// TASKS:    M1-01
// STATUS:   MINIMAL STUB IMPLEMENTED BY MEMBER 2 (agreed). The shared
//           state (projection, transform stack, tint, grayscale) is
//           implemented here. The four draw primitives are pure virtual
//           so Member 1 supplies the OpenGL version in Renderer.cpp
//           (class OpenGLRenderer) and Member 2 can run a headless
//           version for testing without a GPU/window.
//
// PURPOSE:
//   Common drawing helpers used by everybody: projection setup and
//   basic draw calls with colour, tint and grayscale options.
//
// RESPONSIBILITIES:
//   - Set up the 2D orthographic projection
//   - Draw coloured/textured quads and shapes
//   - Draw with a red damage tint or grayscale (locked cards)
//   - Provide a transform stack used by hierarchical models
//
// PLANNED PUBLIC INTERFACE (names agreed by the team):
//   - init()
//   - drawQuad(...)  drawCircle(...)  drawTriangle(...)  drawTrapezoid(...)
//   - pushTransform() / popTransform()
//   - setTint(...) / setGrayscale(...)
//
// INTERACTS WITH: Entities, Scene, UI, Cutscene
// ====================================================================

#include "Math.h"
#include "Constants.h"
#include <vector>

class Renderer {
public:
    virtual ~Renderer() = default;

    // ------------------------------------------------------------ frame
    // Clear the transform stack and the style before drawing a frame.
    void beginFrame() {
        stack.clear();
        stack.push_back(Transform2D{});
        resetStyle();
    }

    // 2D orthographic projection over the whole world, origin bottom-left.
    void setOrtho(float left, float right, float bottom, float top) {
        projection = Mat4::ortho(left, right, bottom, top);
    }
    void setDefaultOrtho() { setOrtho(0.0f, C::WORLD_WIDTH, 0.0f, C::WORLD_HEIGHT); }

    const Mat4& getProjection() const { return projection; }

    // --------------------------------------------------- transform stack
    // Push a transform relative to the current top of the stack. The
    // HierarchicalModel walks its part tree with this so limbs follow
    // their parents.
    void pushTransform(const Transform2D& t) {
        stack.push_back(Transform2D::compose(stack.back(), t));
    }
    void popTransform() {
        if (stack.size() > 1) stack.pop_back();
    }
    const Transform2D& top() const { return stack.back(); }
    // World-space position of a local point, using the current transform.
    Vec2 toWorld(const Vec2& localPoint) const { return stack.back().apply(localPoint); }

    // ------------------------------------------------- tint and grayscale
    // Damage feedback (US-43 / US-45): tintAmount 0..1 blends the whole
    // model towards red as health drops.
    void  setTint(const Vec4& c) { tintColour = c; }
    const Vec4& getTint()      const { return tintColour; }
    void  setTintAmount(float a) { tintAmount = clampf(a, 0.0f, 1.0f); }
    float getTintAmount()      const { return tintAmount; }
    void  setGrayscale(bool on) { grayscale = on; }
    bool  isGrayscale()        const { return grayscale; }
    void  resetStyle() { tintColour = Vec4{ 1,1,1,1 }; tintAmount = 0.0f; grayscale = false; }

    // --------------------------------------------------------- drawing
    // All positions/sizes are in the CURRENT LOCAL SPACE (the transform
    // stack is applied by the renderer / model matrix).
    virtual void drawQuad(const Vec2& center, const Vec2& size, const Vec4& colour, float rotation = 0.0f) = 0;
    virtual void drawCircle(const Vec2& center, float radius, const Vec4& colour, int segments = 20) = 0;
    virtual void drawTriangle(const Vec2& a, const Vec2& b, const Vec2& c, const Vec4& colour) = 0;
    // Trapezoid: used for hoods, snake heads and chest plates.
    virtual void drawTrapezoid(const Vec2& center, const Vec2& size, float topWidthScale,
                               const Vec4& colour, float rotation = 0.0f) = 0;

    // Preferred window size (used by Member 1's Game when creating the window).
    virtual void init() {}

protected:
    std::vector<Transform2D> stack{ Transform2D{} };
    Mat4  projection = Mat4::identity();
    Vec4  tintColour{ 1, 1, 1, 1 };
    float tintAmount = 0.0f;
    bool  grayscale  = false;
};

// ---------------------------------------------------------------------
// A do-nothing renderer so code never crashes when nothing was set up
// (e.g. before Member 1's OpenGL renderer is installed).
// ---------------------------------------------------------------------
class NullRenderer : public Renderer {
public:
    void drawQuad(const Vec2&, const Vec2&, const Vec4&, float) override {}
    void drawCircle(const Vec2&, float, const Vec4&, int) override {}
    void drawTriangle(const Vec2&, const Vec2&, const Vec2&, const Vec4&) override {}
    void drawTrapezoid(const Vec2&, const Vec2&, float, const Vec4&, float) override {}
};

// Global accessor, matching the skeleton's argument-less draw() calls.
inline Renderer*& rendererSlot() {
    static Renderer* current = nullptr;
    return current;
}
inline Renderer& getRenderer() {
    if (!rendererSlot()) {
        static NullRenderer fallback;
        rendererSlot() = &fallback;
    }
    return *rendererSlot();
}
inline void setRenderer(Renderer* r) { rendererSlot() = r; }
