// File: src/GameUI.hpp
#pragma once
#include <SFML/Graphics.hpp>

class GameUI {
public:
    GameUI(sf::RenderWindow& window);
    
    bool loadAssets();
    void render();
    void setWaterLevel(float progress); // 0.0 to 1.0
    void setFireLevel(float progress);  // 0.0 to 1.0
    
private:
    sf::RenderWindow& window_;
    
    // Water tank
    sf::Texture waterTexture_;
    sf::Texture waterMaskTexture_;
    sf::RenderTexture waterProgressTex_;
    sf::Sprite waterContainer_;
    sf::Sprite waterFillSprite_;
    sf::Vector2f waterBarPos_;
    sf::Vector2f waterBarSize_;
    float waterProgress_;
    
    // Fire meter
    sf::Texture fireBarTexture_;
    sf::Texture fireBarMaskTexture_;
    sf::RenderTexture fireProgressTex_;
    sf::Sprite fireBar_;
    sf::Sprite fireFillSprite_;
    sf::Vector2f fireSize_;
    float fireProgress_;
    
    void setupWaterBar();
    void setupFireBar();
    void updateWaterBar();
    void updateFireBar();
};