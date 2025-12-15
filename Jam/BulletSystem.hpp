// File: src/BulletSystem.hpp
#pragma once
#include "IUpdatable.hpp"
#include "IDrawable.hpp"
#include "Bullet.h"
#include "AnimatedSprite.h"
#include "Map.hpp"
#include "FireSimulator.hpp"
#include <vector>
#include <memory>

class BulletSystem
{
public:
    BulletSystem();

    // Collision detection with fire tiles
    void checkCollisions(const Map& map, FireSimulator& fireSim);

    // Query
    const std::vector<Bullet> &getBullets() const { return bullets_; }
    void clear() { bullets_.clear(); }

private:
    std::vector<Bullet> bullets_;
    std::unique_ptr<AnimatedSprite> bulletAnimTemplate_;
    sf::FloatRect worldBounds_;
};