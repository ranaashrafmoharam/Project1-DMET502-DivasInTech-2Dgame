#pragma once
// ====================================================================
// FILE:     src/core/Renderer.h
// OWNER:    Member 1
// TASKS:    M1-01
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
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
// PLANNED PUBLIC INTERFACE (names to agree on, declared here):
//   - init()
//   - drawQuad(...)
//   - drawShape(...)
//   - pushTransform() / popTransform()
//   - setTint(...) / setGrayscale(...)
//
// INTERACTS WITH: Entities, Scene, UI, Cutscene
// ====================================================================
