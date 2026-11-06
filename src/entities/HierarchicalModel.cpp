#include "HierarchicalModel.h"
// ====================================================================
// FILE:     src/entities/HierarchicalModel.cpp
// OWNER:    Member 2
// TASKS:    M2-01
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Reusable parent-child transform system for building characters
//   from parts (body, head, arms, legs, weapon) that can move
//   independently.
//
// INTERACTS WITH: CharacterModels, Animation, Renderer
// ====================================================================

// ---------------------------------------------------------------- helper
// Turns one part's shape description into the matching Renderer call.
// Parts are always drawn centred on their own origin; the transform
// stack has already placed that origin in the world.
static void drawPartShape(Renderer& r, const PartShapeDef& s) {
    switch (s.shape) {
        case PartShape::Rect:
            r.drawQuad({ 0.0f, 0.0f }, s.size, s.colour);
            break;

        case PartShape::Circle:
            r.drawCircle({ 0.0f, 0.0f }, s.size.x * 0.5f, s.colour, s.segments);
            break;

        case PartShape::Triangle:
            r.drawTriangle({ -s.size.x * 0.5f, -s.size.y * 0.5f },
                           {  s.size.x * 0.5f, -s.size.y * 0.5f },
                           {  0.0f,             s.size.y * 0.5f },
                           s.colour);
            break;

        case PartShape::Trapezoid:
            r.drawTrapezoid({ 0.0f, 0.0f }, s.size, s.topWidthScale, s.colour);
            break;

        case PartShape::Blade:
            // Tall thin triangle: a sword / blade that points "up" in local space.
            r.drawTriangle({ -s.size.x * 0.5f, -s.size.y * 0.5f },
                           {  s.size.x * 0.5f, -s.size.y * 0.5f },
                           {  0.0f,             s.size.y * 0.5f },
                           s.colour);
            break;
    }
}

// ---------------------------------------------------------------- building
int HierarchicalModel::addPart(const std::string& name,
                               const std::string& parentName,
                               const PartShapeDef& shape,
                               const Vec2& offset,
                               float rotation,
                               const Vec2& scale) {
    // One part per name keeps the Animation lookup simple.
    if (name.empty() || lookup.find(name) != lookup.end()) return -1;

    const int parent = parentName.empty() ? -1 : indexOf(parentName);
    if (!parentName.empty() && parent < 0) return -1;

    Part p;
    p.name     = name;
    p.shape    = shape;
    p.restPose = Transform2D(offset, rotation, scale);
    p.parent   = parent;

    const int index = (int)parts.size();
    parts.push_back(p);
    children.push_back({});
    lookup[name] = index;

    if (p.parent >= 0) children[p.parent].push_back(index);
    else               roots.push_back(index);

    return index;
}

// ---------------------------------------------------------------- posing
int HierarchicalModel::indexOf(const std::string& name) const {
    auto it = lookup.find(name);
    return it == lookup.end() ? -1 : it->second;
}

const Part* HierarchicalModel::part(const std::string& name) const {
    int i = indexOf(name);
    return i < 0 ? nullptr : &parts[i];
}

void HierarchicalModel::setPose(const std::string& name, const Transform2D& t) {
    int i = indexOf(name);
    if (i >= 0) parts[i].pose = t;
}

void HierarchicalModel::addPose(const std::string& name, const Transform2D& t) {
    int i = indexOf(name);
    if (i >= 0) parts[i].pose = Transform2D::compose(parts[i].pose, t);
}

void HierarchicalModel::resetPoses() {
    for (Part& p : parts) p.pose = Transform2D{};
}

void HierarchicalModel::setVisible(const std::string& name, bool v) {
    int i = indexOf(name);
    if (i >= 0) parts[i].visible = v;
}

void HierarchicalModel::setColour(const std::string& name, const Vec4& c) {
    int i = indexOf(name);
    if (i >= 0) parts[i].shape.colour = c;
}

// ---------------------------------------------------------------- queries
Transform2D HierarchicalModel::worldTransform(int index) const {
    if (index < 0 || index >= (int)parts.size()) return rootTransform;

    const Part& p = parts[index];
    // restPose then pose, exactly like a bone's base plus its animation.
    Transform2D local = Transform2D::compose(p.restPose, p.pose);

    Transform2D parentWorld = (p.parent >= 0) ? worldTransform(p.parent) : rootTransform;
    return Transform2D::compose(parentWorld, local);
}

Vec2 HierarchicalModel::partWorldPosition(const std::string& name) const {
    int i = indexOf(name);
    if (i < 0) return rootTransform.translation;
    return worldTransform(i).translation;
}

// ---------------------------------------------------------------- drawing
void HierarchicalModel::drawPart(Renderer& r, int index) {
    Part& p = parts[index];
    if (!p.visible) return;

    // Local = rest pose + animation delta.
    Transform2D local = Transform2D::compose(p.restPose, p.pose);
    Transform2D screenLocal({ local.translation.x, -local.translation.y },
                            -local.rotation, local.scale);

    r.pushTransform(screenLocal);    // this makes the part's children follow it
    r.pushTransform(Transform2D({ 0.0f, 0.0f }, 0.0f, { 1.0f, -1.0f }));
    drawPartShape(r, p.shape);
    r.popTransform();

    for (int c : children[index]) drawPart(r, c);

    r.popTransform();
}

void HierarchicalModel::draw(Renderer& r) {
    Transform2D screenRoot({ rootTransform.translation.x,
                             C::WORLD_HEIGHT - rootTransform.translation.y },
                           -rootTransform.rotation, rootTransform.scale);
    r.pushTransform(screenRoot);     // feet position + facing of the whole unit
    for (int root : roots) drawPart(r, root);
    r.popTransform();
}
