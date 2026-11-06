#pragma once
// ====================================================================
// FILE:     src/world/CollisionSystem.h
// OWNER:    Member 1
// TASKS:    M1-08, M1-09, M1-10
// STATUS:   IMPLEMENTED
//
// PURPOSE:
//   Movement rules and all combat interactions.
//
// RESPONSIBILITIES:
//   - Per-lane box collision
//   - Stop both units when a player meets an enemy; both keep attacking
//   - Players pass through each other and Defense Players
//   - Defense Player shielding for players behind it
//   - Projectile hits and collectible pickups
//   - Detect an enemy reaching the Home Base
//
// PLANNED PUBLIC INTERFACE:
//   - update(players, enemies, projectiles, collectibles, coins)
//   - hasEnemyReachedBase()
// ====================================================================

#include <vector>
#include "../entities/Entity.h"

class CollisionSystem {
public:
    CollisionSystem();

    void reset();

    // Resolves all collisions, engagements, shielding, projectiles, pickups, and base breach
    void update(
        std::vector<Entity*>& players,
        std::vector<Entity*>& enemies,
        std::vector<Entity*>& projectiles,
        std::vector<Entity*>& collectibles,
        int& currentCoins,
        float dt
    );

    bool hasEnemyReachedBase() const { return m_enemyReachedBase; }

private:
    bool m_enemyReachedBase;

    void updateShielding(std::vector<Entity*>& players);
    void updateCombat(std::vector<Entity*>& players, std::vector<Entity*>& enemies, float dt);
    void updateRangedAttacks(std::vector<Entity*>& players, std::vector<Entity*>& enemies, std::vector<Entity*>& projectiles);
    void updateProjectiles(std::vector<Entity*>& projectiles, std::vector<Entity*>& players, std::vector<Entity*>& enemies);
    void updateCollectibles(std::vector<Entity*>& collectibles, std::vector<Entity*>& players, int& currentCoins);
    void checkHomeBaseBreach(const std::vector<Entity*>& enemies, const std::vector<Entity*>& players);
};
