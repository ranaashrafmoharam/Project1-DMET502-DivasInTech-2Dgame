#include "CollisionSystem.h"
#include "../core/Constants.h"
#include "../audio/AudioManager.h"
#include <algorithm>

CollisionSystem& CollisionSystem::instance() {
    static CollisionSystem cs;
    return cs;
}

void CollisionSystem::update(std::vector<std::shared_ptr<Player>>& players,
                             std::vector<std::shared_ptr<Enemy>>& enemies,
                             std::vector<std::shared_ptr<Projectile>>& projectiles,
                             int& coins) {
    m_baseBreached = false;

    // Reset combat state flags each tick
    for (auto& p : players) if (p) p->isFighting = false;
    for (auto& e : enemies) if (e) e->isFighting = false;

    // 1. Check Defense Shielding per lane (US-32)
    // Find defense player x in each lane
    float defenseX[Constants::NUM_LANES];
    for (int l = 0; l < Constants::NUM_LANES; ++l) defenseX[l] = -1.0f;

    for (const auto& p : players) {
        if (p && p->isAlive && p->type == EntityType::PLAYER_DEFENSE) {
            defenseX[p->lane] = std::max(defenseX[p->lane], p->x);
        }
    }

    for (auto& p : players) {
        if (p && p->isAlive) {
            // Player is shielded if standing behind (to the left of) a living defense player
            p->isShielded = (defenseX[p->lane] > -0.5f && p->x < defenseX[p->lane] && p->type != EntityType::PLAYER_DEFENSE);
        }
    }

    // 2. Melee Encounters between Players and Enemies (US-41)
    for (auto& p : players) {
        if (!p || !p->isAlive) continue;

        for (auto& e : enemies) {
            if (!e || !e->isAlive) continue;
            if (p->lane != e->lane) continue;

            // Collision check: player advances right, enemy advances left
            // Overlap or touching distance in lane
            if (p->x + p->width >= e->x && p->x <= e->x + e->width) {
                p->isFighting = true;
                e->isFighting = true;

                // Player attacks enemy
                if (p->canAttack() && p->type != EntityType::PLAYER_DEFENSE) {
                    p->resetAttackCooldown();
                    e->takeDamage(Constants::DAMAGE_MELEE_PLAYER);
                    AudioManager::instance().playSound(SoundID::ATTACK_SWORD);

                    if (!e->isAlive) {
                        AudioManager::instance().playSound(SoundID::UNIT_DEATH);
                        // Drop collectible shard at enemy location (US-46)
                    }
                }

                // Enemy attacks player (unless player is shielded by a Defense unit)
                if (e->canAttack() && e->isAlive) {
                    e->resetAttackCooldown();
                    if (!p->isShielded) {
                        p->takeDamage(e->getDamage());
                        AudioManager::instance().playSound(SoundID::UNIT_HIT);
                        if (!p->isAlive) {
                            AudioManager::instance().playSound(SoundID::UNIT_DEATH);
                        }
                    }
                }
            }
        }
    }

    // 3. Projectile Collisions (US-42)
    for (auto& proj : projectiles) {
        if (!proj || !proj->isAlive) continue;

        if (proj->isPlayerProjectile()) {
            // Zane's Ice Shuriken: collides with first living enemy in its lane
            for (auto& e : enemies) {
                if (!e || !e->isAlive || e->lane != proj->lane) continue;

                if (proj->getBounds().overlaps(e->getBounds())) {
                    e->takeDamage(Constants::DAMAGE_RANGED_PLAYER);
                    proj->isAlive = false;
                    AudioManager::instance().playSound(SoundID::UNIT_HIT);

                    if (!e->isAlive) {
                        AudioManager::instance().playSound(SoundID::UNIT_DEATH);
                    }
                    break;
                }
            }
        } else {
            // Enemy Projectiles: High (Acid) or Low (Sonic)
            for (auto& p : players) {
                if (!p || !p->isAlive || p->lane != proj->lane) continue;

                if (proj->getBounds().overlaps(p->getBounds())) {
                    // Shielding protects player from projectile
                    if (p->isShielded) {
                        proj->isAlive = false;
                        break;
                    }

                    // High attack dodge: if player is DUCKING, projectile misses! (US-30, US-35)
                    if (proj->isHighAttack() && p->isDucking()) {
                        continue; // dodged!
                    }

                    // Low attack dodge: if player is JUMPING, projectile misses! (US-29, US-36)
                    if (proj->isLowAttack() && p->isJumping()) {
                        continue; // dodged!
                    }

                    // Hit!
                    p->takeDamage(1);
                    proj->isAlive = false;
                    AudioManager::instance().playSound(SoundID::UNIT_HIT);
                    if (!p->isAlive) {
                        AudioManager::instance().playSound(SoundID::UNIT_DEATH);
                    }
                    break;
                }
            }
        }
    }

    // 5. Home Base Breach Check (US-50)
    for (const auto& e : enemies) {
        if (e && e->isAlive) {
            // If enemy reaches monastery gate at FIELD_X0
            if (e->x <= Constants::FIELD_X0 + 10.0f) {
                m_baseBreached = true;
                break;
            }
        }
    }

    // Cleanup dead entities (US-45: Defeated units removed from scene)
    players.erase(std::remove_if(players.begin(), players.end(),
        [](const std::shared_ptr<Player>& p) { return !p || !p->isAlive; }), players.end());

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const std::shared_ptr<Enemy>& e) { return !e || !e->isAlive; }), enemies.end());

    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
        [](const std::shared_ptr<Projectile>& pr) { return !pr || !pr->isAlive; }), projectiles.end());

}
