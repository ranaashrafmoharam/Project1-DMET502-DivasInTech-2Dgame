#pragma once
// ====================================================================
// FILE:     src/world/CollisionSystem.h
// Resolves movement, melee fights, defense shielding, projectile
// impacts, collectible pickups, and home base breaches.
// ====================================================================

#include <vector>
#include <memory>
#include "../entities/Player.h"
#include "../entities/Enemy.h"
#include "../entities/Projectile.h"

class CollisionSystem {
public:
    static CollisionSystem& instance();

    void update(std::vector<std::shared_ptr<Player>>& players,
                std::vector<std::shared_ptr<Enemy>>& enemies,
                std::vector<std::shared_ptr<Projectile>>& projectiles,
                int& coins);

    bool hasEnemyReachedBase() const { return m_baseBreached; }
    void reset() { m_baseBreached = false; }

private:
    CollisionSystem() = default;

    bool m_baseBreached = false;
};
