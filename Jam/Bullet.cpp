// File: src/Bullet.cpp
#include "Bullet.h"
#include <cmath>

Bullet::Bullet(sf::Vector2f pos, sf::Vector2f dir, const AnimatedSprite& animTemplate)
    : position(pos)
{
    // Normalize direction vector
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len != 0) {
        dir /= len;
    }

    // velocity = dir * speed;

    // Copy animation template
    anim = animTemplate;
    anim.setPosition(position - sf::Vector2f(0, -128));
}

void Bullet::update(float dt)
{
    anim.update(dt);
}

void Bullet::draw(sf::RenderWindow& win) const
{
    win.draw(anim);
}

bool Bullet::isOffscreen(const sf::FloatRect& area) const
{
    return !area.contains(position);
}