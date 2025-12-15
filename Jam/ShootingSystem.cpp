// ============================================================================
// File: src/ShootingSystem.cpp (UPDATED - Water bar integration)
// ============================================================================
#include "ShootingSystem.hpp"
#include <iostream>

ShootingSystem::ShootingSystem(PlayerController &player, BulletSystem &bulletSystem)
    : player_(player), bulletSystem_(bulletSystem) {}

sf::Vector2f ShootingSystem::getDirectionVector(Direction dir) const
{
    switch (dir)
    {
    case Direction::Up:
        return sf::Vector2f(0, -1);
    case Direction::Down:
        return sf::Vector2f(0, 1);
    case Direction::Left:
        return sf::Vector2f(-1, 0);
    case Direction::Right:
        return sf::Vector2f(1, 0);
    }
    return sf::Vector2f(0, 0);
}

void ShootingSystem::handleInput(float &waterLevel, float deltaTime)
{
    // Check if player wants to spray water (B key pressed)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
    {

        // Check if player has water remaining
        if (waterLevel > 0.0f)
        {
            // Enable water animation
            player_.setFiring(true);

            // Consume water over time (continuous consumption while spraying)
            float waterConsumed = waterConsumptionRate_ * deltaTime;
            waterLevel -= waterConsumed;

            // Clamp water level to valid range [0, 1]
            if (waterLevel < 0.0f)
            {
                waterLevel = 0.0f;
                player_.setFiring(false); // Stop firing when water runs out

                // Optional: Notify player they're out of water
                static bool outOfWaterMessageShown = false;
                if (!outOfWaterMessageShown)
                {
                    std::cout << "💧 OUT OF WATER! Find a refill station!" << std::endl;
                    outOfWaterMessageShown = true;
                }
            }
        }
        else
        {
            // No water left - can't spray
            player_.setFiring(false);
        }
    }
    else
    {
        // B key not pressed - stop spraying
        player_.setFiring(false);
    }
}