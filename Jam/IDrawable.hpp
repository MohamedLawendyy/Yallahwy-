// ============================================================================
// File: src/IDrawable.hpp
// Interface Segregation Principle - separate drawing concern
// ============================================================================
#pragma once
#include <SFML/Graphics.hpp>

class IDrawable {
public:
    virtual ~IDrawable() = default;
    virtual void draw(sf::RenderWindow& window) = 0;
};