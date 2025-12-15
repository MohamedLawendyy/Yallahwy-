// File: src/BulletSystem.cpp
#include "BulletSystem.hpp"
#include <iostream>
#include <cmath>

BulletSystem::BulletSystem()
{
    bulletAnimTemplate_ = std::make_unique<AnimatedSprite>();
    worldBounds_ = sf::FloatRect(0, 0, Map::WIDTH * Map::TILE_SIZE, Map::HEIGHT * Map::TILE_SIZE);
}

void BulletSystem::checkCollisions(const Map& map, FireSimulator& fireSim)
{
    for (auto it = bullets_.begin(); it != bullets_.end(); )
    {
        sf::Vector2f bulletPos = it->getPosition();
        auto [gridX, gridY] = map.worldToGrid(bulletPos);

        // Check if bullet hit a burning tile
        if (map.isValidPosition(gridX, gridY) && 
            map.getTileState(static_cast<size_t>(gridX), static_cast<size_t>(gridY)) == TileState::Burning)
        {
            // Extinguish the fire
            fireSim.extinguishAt(static_cast<size_t>(gridX), static_cast<size_t>(gridY));
            
            // Remove the bullet
            it = bullets_.erase(it);
            
            std::cout << "Bullet extinguished fire at (" << gridX << ", " << gridY << ")" << std::endl;
        }
        else
        {
            ++it;
        }
    }
}