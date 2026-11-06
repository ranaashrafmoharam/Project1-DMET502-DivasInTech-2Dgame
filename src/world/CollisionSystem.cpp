// ====================================================================
// FILE:     src/world/CollisionSystem.cpp
// OWNER:    Member 1
// TASKS:    M1-08, M1-09, M1-10
// STATUS:   IMPLEMENTED
// ====================================================================

#include "CollisionSystem.h"
#include "../entities/Player.h"
#include "../entities/Enemy.h"
#include "../entities/Projectile.h"
#include <iostream>

CollisionSystem::CollisionSystem()
    : m_enemyReachedBase(false)
{
}

void CollisionSystem::reset() {
    m_enemyReachedBase = false;
}

void CollisionSystem::update(
    std::vector<Entity*>& players,
    std::vector<Entity*>& enemies,
    std::vector<Entity*>& projectiles,
    std::vector<Entity*>& collectibles,
    int& currentCoins,
    float dt
) {
    if (m_enemyReachedBase) return;

    // 1. Update Defense Player shielding state for each lane
    updateShielding(players);

    // 2. Resolve melee combat and movement halts
    updateCombat(players, enemies, dt);

    // 3. Trigger ranged shots
    updateRangedAttacks(players, enemies, projectiles);

    // 4. Resolve projectile collisions
    updateProjectiles(projectiles, players, enemies);

    // 5. Check collectible pickups by advancing players
    updateCollectibles(collectibles, players, currentCoins);

    // 6. Check if any enemy reached the Home Base
    checkHomeBaseBreach(enemies, players);
}

void CollisionSystem::updateShielding(std::vector<Entity*>& players) {
    // Reset shielded status for all players first
    for (auto* player : players) {
        if (player && player->alive) {
            player->shielded = false;
        }
    }

    // In each lane, if a living Defense player exists, players to its left (behind it) are shielded
    for (int lane = 0; lane < Constants::LANE_COUNT; ++lane) {
        float maxDefenseX = -1.0f;
        bool hasLivingDefense = false;

        for (const auto* p : players) {
            if (p && p->alive && p->laneIndex == lane && p->type == EntityType::PLAYER_DEFENSE) {
                if (p->position.x > maxDefenseX) {
                    maxDefenseX = p->position.x;
                    hasLivingDefense = true;
                }
            }
        }

        if (hasLivingDefense) {
            for (auto* p : players) {
                if (p && p->alive && p->laneIndex == lane && p->type != EntityType::PLAYER_DEFENSE) {
                    // Protected while strictly behind (to the left of) the Defense Player
                    if (p->position.x <= maxDefenseX) {
                        p->shielded = true;
                    }
                }
            }
        }
    }
}

void CollisionSystem::updateCombat(std::vector<Entity*>& players, std::vector<Entity*>& enemies, float /*dt*/) {
    // Reset isFighting flag each frame
    for (auto* p : players) {
        if (p) p->isFighting = false;
    }
    for (auto* e : enemies) {
        if (e) e->isFighting = false;
    }

    // Check overlaps per lane
    for (auto* p : players) {
        if (!p || !p->alive) continue;

        Bounds pBounds = p->getBounds();

        for (auto* e : enemies) {
            if (!e || !e->alive) continue;
            if (p->laneIndex != e->laneIndex) continue;

            Bounds eBounds = e->getBounds();

            if (pBounds.intersects(eBounds)) {
                // Both units halt movement
                p->isFighting = true;
                e->isFighting = true;

                // Player attacks enemy
                Player* characterPlayer = dynamic_cast<Player*>(p);
                Enemy* characterEnemy = dynamic_cast<Enemy*>(e);
                if (characterPlayer) {
                    if (!characterPlayer->isRanged() && !characterPlayer->isStationary() &&
                        characterPlayer->claimAttack()) {
                        e->takeDamage(static_cast<int>(characterPlayer->attackDamage()));
                    }
                } else if (p->attackTimer <= 0.0f) {
                    if (p->type != EntityType::PLAYER_DEFENSE) {
                        e->takeDamage(p->attackDamage);
                    }
                    p->attackTimer = p->attackCooldown;
                }

                // Enemy attacks player (or Defense player)
                if (characterEnemy) {
                    if (!characterEnemy->isRanged() && characterEnemy->claimAttack() &&
                        !p->shielded) {
                        p->takeDamage(static_cast<int>(characterEnemy->attackDamage()));
                    }
                } else if (e->attackTimer <= 0.0f) {
                    if (!p->shielded) {
                        p->takeDamage(e->attackDamage);
                    }
                    e->attackTimer = e->attackCooldown;
                }
            }
        }
    }
}

void CollisionSystem::updateRangedAttacks(
    std::vector<Entity*>& players,
    std::vector<Entity*>& enemies,
    std::vector<Entity*>& projectiles
) {
    // 1. Ranged players shoot at enemies ahead of them in their lane
    for (auto* p : players) {
        if (!p || !p->alive || p->type != EntityType::PLAYER_RANGED) continue;
        if (Player* characterPlayer = dynamic_cast<Player*>(p)) {
            if (!characterPlayer->claimAttack()) continue;
            bool enemyInRange = false;
            for (const auto* e : enemies) {
                if (!e || !e->alive || e->laneIndex != p->laneIndex) continue;
                const float distance = e->position.x - p->position.x;
                if (distance > 0.0f && distance <= characterPlayer->attackRange()) {
                    enemyInRange = true;
                    break;
                }
            }
            if (enemyInRange) {
                Projectile* projectile = new Projectile();
                projectile->spawn(characterPlayer->muzzle(), p->laneIndex, Faction::Player,
                                  UnitType::RangedPlayer, characterPlayer->attackHeight(),
                                  characterPlayer->facing(), characterPlayer->attackDamage());
                projectiles.push_back(projectile);
            }
            continue;
        }
        if (p->attackTimer > 0.0f) continue;

        bool enemyInLane = false;
        for (const auto* e : enemies) {
            if (e && e->alive && e->laneIndex == p->laneIndex && e->position.x > p->position.x) {
                enemyInLane = true;
                break;
            }
        }

        if (enemyInLane) {
            p->attackTimer = p->attackCooldown;
            // Spawn player projectile (center height)
            Entity* proj = new Entity(
                EntityType::PROJECTILE_PLAYER,
                p->laneIndex,
                p->position.x + p->size.x * 0.5f,
                p->position.y,
                1,
                Constants::SPEED_PROJECTILE
            );
            proj->size = glm::vec2(Constants::PROJECTILE_WIDTH, Constants::PROJECTILE_HEIGHT);
            proj->attackDamage = Constants::DMG_PLAYER_RANGED;
            projectiles.push_back(proj);
        }
    }

    // 2. Ranged enemies attack as soon as they appear:
    // - Easy Enemy: launches HIGH attacks (head level -> duck under)
    // - Medium Enemy: launches LOW attacks (foot level -> jump over)
    for (auto* e : enemies) {
        if (!e || !e->alive || e->attackTimer > 0.0f) continue;

        if (Enemy* characterEnemy = dynamic_cast<Enemy*>(e)) {
            if (!characterEnemy->isRanged() || !characterEnemy->claimAttack()) continue;
            Projectile* projectile = new Projectile();
            projectile->spawn(characterEnemy->muzzle(), e->laneIndex, Faction::Enemy,
                              characterEnemy->type == EntityType::ENEMY_EASY
                                  ? UnitType::EasyEnemy : UnitType::MediumEnemy,
                              characterEnemy->attackHeight(), characterEnemy->facing(),
                              characterEnemy->attackDamage());
            projectiles.push_back(projectile);
            continue;
        }

        if (e->type == EntityType::ENEMY_EASY || e->type == EntityType::ENEMY_MEDIUM) {
            e->attackTimer = e->attackCooldown;

            bool isHigh = (e->type == EntityType::ENEMY_EASY);
            EntityType pType = isHigh ? EntityType::PROJECTILE_HIGH : EntityType::PROJECTILE_LOW;

            // HIGH attacks travel at the top of the lane; LOW attacks travel at the bottom
            float spawnY = isHigh ? (e->position.y - 32.0f) : (e->position.y + 32.0f);

            Entity* proj = new Entity(
                pType,
                e->laneIndex,
                e->position.x - e->size.x * 0.5f,
                spawnY,
                1,
                Constants::SPEED_PROJECTILE
            );
            proj->size = glm::vec2(Constants::PROJECTILE_WIDTH, Constants::PROJECTILE_HEIGHT);
            proj->attackDamage = e->attackDamage;
            projectiles.push_back(proj);
        }
    }
}

void CollisionSystem::updateProjectiles(
    std::vector<Entity*>& projectiles,
    std::vector<Entity*>& players,
    std::vector<Entity*>& enemies
) {
    for (auto* proj : projectiles) {
        if (!proj || !proj->alive) continue;

        Bounds projBounds = proj->getBounds();

        // Offscreen check
        if (proj->position.x < Constants::HOME_BASE_X || proj->position.x > Constants::RIGHT_EDGE_X + 50.0f) {
            proj->alive = false;
            continue;
        }

        if (proj->type == EntityType::PROJECTILE_PLAYER) {
            // Check hit against enemies
            for (auto* e : enemies) {
                if (!e || !e->alive || e->laneIndex != proj->laneIndex) continue;
                if (Projectile* characterProjectile = dynamic_cast<Projectile*>(proj)) {
                    if (!characterProjectile->canHit(*e)) continue;
                }
                if (projBounds.intersects(e->getBounds())) {
                    if (Projectile* characterProjectile = dynamic_cast<Projectile*>(proj)) {
                        e->takeDamage(static_cast<int>(characterProjectile->getDamage()));
                        characterProjectile->onHit();
                    } else {
                        e->takeDamage(proj->attackDamage);
                        proj->alive = false;
                    }
                    break;
                }
            }
        } else {
            // Enemy projectile (HIGH or LOW) interacting with players
            for (auto* p : players) {
                if (!p || !p->alive || p->laneIndex != proj->laneIndex) continue;
                Projectile* characterProjectile = dynamic_cast<Projectile*>(proj);
                if (characterProjectile && !characterProjectile->canHit(*p)) continue;

                // Check horizontal overlap
                float projLeft = proj->position.x - proj->size.x * 0.5f;
                float projRight = proj->position.x + proj->size.x * 0.5f;
                float pLeft = p->position.x - p->size.x * 0.5f;
                float pRight = p->position.x + p->size.x * 0.5f;

                if (projLeft <= pRight && projRight >= pLeft) {
                    if (proj->type == EntityType::PROJECTILE_HIGH) {
                        if (p->isDucking) {
                            // Successfully ducked under! Flies safely past over head!
                            continue;
                        } else if (p->shielded) {
                            // Blocked by living Defense Player
                            proj->alive = false;
                            break;
                        } else {
                            // Player is standing or jumping -> gets hit!
                            p->takeDamage(characterProjectile
                                ? static_cast<int>(characterProjectile->getDamage())
                                : proj->attackDamage);
                            if (characterProjectile) characterProjectile->onHit();
                            else proj->alive = false;
                            break;
                        }
                    } else if (proj->type == EntityType::PROJECTILE_LOW) {
                        if (p->isJumping) {
                            // Successfully jumped over! Flies safely past underneath feet!
                            continue;
                        } else if (p->shielded) {
                            // Blocked by living Defense Player
                            proj->alive = false;
                            break;
                        } else {
                            // Player is standing or ducking -> gets hit!
                            p->takeDamage(characterProjectile
                                ? static_cast<int>(characterProjectile->getDamage())
                                : proj->attackDamage);
                            if (characterProjectile) characterProjectile->onHit();
                            else proj->alive = false;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void CollisionSystem::updateCollectibles(
    std::vector<Entity*>& collectibles,
    std::vector<Entity*>& players,
    int& currentCoins
) {
    for (auto* c : collectibles) {
        if (!c || !c->alive) continue;

        Bounds cBounds = c->getBounds();

        for (auto* p : players) {
            if (!p || !p->alive || p->laneIndex != c->laneIndex) continue;

            if (p->getBounds().intersects(cBounds)) {
                c->alive = false;
                currentCoins += Constants::COIN_DROP_VALUE;
                std::cout << "[CollisionSystem] Coin collected! Total coins: " << currentCoins << std::endl;
                break;
            }
        }
    }
}

void CollisionSystem::checkHomeBaseBreach(
    const std::vector<Entity*>& enemies,
    const std::vector<Entity*>& players
) {
    for (const auto* e : enemies) {
        if (!e || !e->alive) continue;

        float enemyLeftX = e->position.x - e->size.x * 0.5f;

        // If enemy touches or passes Home Base boundary
        if (enemyLeftX <= Constants::HOME_BASE_X) {
            // Check if there is a living Defense player blocking this lane before the base
            bool blockedByDefense = false;
            for (const auto* p : players) {
                if (p && p->alive && p->laneIndex == e->laneIndex && p->type == EntityType::PLAYER_DEFENSE) {
                    if (p->position.x >= Constants::HOME_BASE_X && p->position.x <= e->position.x) {
                        blockedByDefense = true;
                        break;
                    }
                }
            }

            if (!blockedByDefense) {
                m_enemyReachedBase = true;
                std::cout << "[CollisionSystem] HOME BASE BREACHED in lane " << e->laneIndex << "!" << std::endl;
                return;
            }
        }
    }
}
