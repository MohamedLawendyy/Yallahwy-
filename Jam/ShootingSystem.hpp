// ============================================================================
// File: src/ShootingSystem.hpp (UPDATED - Water bar integration)
// Single Responsibility: Handle shooting input and coordination
// ============================================================================
#pragma once
#include "PlayerController.hpp"
#include "BulletSystem.hpp"
#include <SFML/Graphics.hpp>

class ShootingSystem {
public:
    ShootingSystem(PlayerController& player, BulletSystem& bulletSystem);
    
    // Updated to accept and modify water level
    void handleInput(float& waterLevel, float deltaTime);
    
    // Water consumption rate (units per second)
    void setWaterConsumptionRate(float rate) { waterConsumptionRate_ = rate; }
    float getWaterConsumptionRate() const { return waterConsumptionRate_; }
    
private:
    PlayerController& player_;
    BulletSystem& bulletSystem_;
    
    float waterConsumptionRate_ = 2.0f; // Water consumed per second when spraying
    
    sf::Vector2f getDirectionVector(Direction dir) const;
};