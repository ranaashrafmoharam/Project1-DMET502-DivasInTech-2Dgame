#pragma once
// ====================================================================
// FILE:     src/entities/HierarchicalModel.h
// OWNER:    Member 2
// TASKS:    M2-01
// STATUS:   PLACEHOLDER (documentation only, no implementation yet)
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
//   - Attach weapon models from Member 3
//
// PLANNED PUBLIC INTERFACE (names to agree on, declared here):
//   - addPart(name, parent, shape)
//   - setPose(name, transform)
//   - draw()
//
// INTERACTS WITH: CharacterModels, Animation, Renderer, PropModels
// ====================================================================
