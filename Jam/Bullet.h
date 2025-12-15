// File: src/Bullet.h
#pragma once
#include <SFML/Graphics.hpp>
#include "AnimatedSprite.h"

class Bullet {
public:
    Bullet(sf::Vector2f pos, sf::Vector2f dir, const AnimatedSprite& animTemplate);

    void update(float dt);
    void draw(sf::RenderWindow& win) const;
    bool isOffscreen(const sf::FloatRect& area) const;
    
    // Position access for collision detection
    sf::Vector2f getPosition() const { return position; }

private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float speed = 500.f;
    AnimatedSprite anim;
};