#pragma once
// ====================================================================
// FILE:     src/entities/HierarchicalModel.h
// OWNER:    Member 2
// TASKS:    M2-01
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Reusable parent-child transform system for building characters
//   from parts (body, head, arms, legs, weapon) that can move
//   independently.
//
// RESPONSIBILITIES:
//   - Hold a tree of parts with local transforms
//   - Draw the tree using the Renderer's transform stack
//   - Let animations set each part's pose
//   - Expose part sockets (e.g. the weapon tip) so projectiles spawn
//     from the right place
//
// PLANNED PUBLIC INTERFACE (names agreed by the team):
//   - addPart(name, parent, shape)
//   - setPose(name, transform)
//   - draw()
//
// HOW IT WORKS:
//   Every part has a "rest pose" (its built-in position in the body) and
//   an animated "pose" delta that Animation writes each frame. The two
//   are composed, then composed again with the parent's world transform,
//   which is exactly the parent-child behaviour limbs need.
//
// INTERACTS WITH: CharacterModels, Animation, Renderer
// ====================================================================

#include "../core/Math.h"
#include "../core/Renderer.h"
#include <string>
#include <vector>
#include <unordered_map>

// Which primitive a part is drawn as.
enum class PartShape { Rect, Circle, Triangle, Trapezoid, Blade };

struct PartShapeDef {
    PartShape shape = PartShape::Rect;
    Vec2      size{ 20.0f, 20.0f };   // local bounding size
    Vec4      colour{ 1, 1, 1, 1 };
    float     topWidthScale = 0.6f;   // Trapezoid only (narrow top)
    int       segments      = 20;     // Circle only
};

// One bone / part of a character.
struct Part {
    std::string name;
    int         parent = -1;          // index into HierarchicalModel::parts, -1 = attached to the root
    PartShapeDef shape;
    Transform2D restPose;             // where the part sits when nothing animates
    Transform2D pose;                 // animated delta written by Animation each frame
    bool        visible = true;
};

class HierarchicalModel {
public:
    HierarchicalModel() = default;

    // ---- building ---------------------------------------------------
    // parentName = "" attaches the part to the model root.
    // Returns the part index, or -1 if a part with that name already exists.
    int addPart(const std::string& name,
                const std::string& parentName,
                const PartShapeDef& shape,
                const Vec2& offset = { 0.0f, 0.0f },
                float rotation = 0.0f,
                const Vec2& scale = { 1.0f, 1.0f });

    // ---- posing -----------------------------------------------------
    // Replace a part's animated delta.
    void setPose(const std::string& name, const Transform2D& t);
    // Compose a delta on top of the part's current animated pose
    // (lets Animation layer, e.g. a walk sway plus an attack swing).
    void addPose(const std::string& name, const Transform2D& t);
    // Clear every animated delta back to the rest pose.
    void resetPoses();

    void setVisible(const std::string& name, bool v);
    void setColour(const std::string& name, const Vec4& c);

    // Whole-model transform (position in the lane + facing). The owning
    // Player/Enemy sets this every frame.
    void setRootTransform(const Transform2D& t) { rootTransform = t; }
    const Transform2D& getRootTransform() const { return rootTransform; }

    // ---- queries ----------------------------------------------------
    bool     hasPart(const std::string& name) const { return lookup.find(name) != lookup.end(); }
    // How many parts (bones) the model is built from.
    int      partCount() const { return (int)parts.size(); }
    int      indexOf(const std::string& name) const;
    const Part* part(const std::string& name) const;
    Vec2     size() const { return modelSize; }
    void     setSize(const Vec2& s) { modelSize = s; }

    // Absolute (world) transform of a part, including the root transform.
    Transform2D worldTransform(int index) const;
    // World position of a part's origin - used as a weapon/muzzle socket.
    Vec2 partWorldPosition(const std::string& name) const;

    // ---- drawing ----------------------------------------------------
    void draw() { draw(getRenderer()); }
    void draw(Renderer& r);

private:
    void drawPart(Renderer& r, int index);

    std::vector<Part>                   parts;
    std::vector<std::vector<int>>       children;   // index -> child indices
    std::vector<int>                    roots;      // parts attached to the root
    std::unordered_map<std::string, int> lookup;
    Transform2D rootTransform;
    Vec2        modelSize{ C::CHARACTER_WIDTH, C::CHARACTER_HEIGHT };
};
