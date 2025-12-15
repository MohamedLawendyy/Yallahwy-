// File: src/GameUI.cpp
#include "GameUI.hpp"
#include <iostream>

GameUI::GameUI(sf::RenderWindow &window)
    : window_(window),
      waterProgress_(0.75f),
      fireProgress_(0.5f)
{
    waterBarPos_ = sf::Vector2f(window.getSize().x - 220, 50);
    waterBarSize_ = sf::Vector2f(100, 200);
    fireSize_ = sf::Vector2f(1024, 256);
}

bool GameUI::loadAssets()
{
    // Load water tank textures
    if (!waterTexture_.loadFromFile("textures/UIWaterTank.png"))
    {
        std::cerr << "Failed to load UIWaterTank.png" << std::endl;
        return false;
    }
    if (!waterMaskTexture_.loadFromFile("textures/UIWaterTank_Mask.png"))
    {
        std::cerr << "Failed to load UIWaterTank_Mask.png" << std::endl;
        return false;
    }

    // Load fire meter textures
    if (!fireBarTexture_.loadFromFile("textures/UIFireMeter.png"))
    {
        std::cerr << "Failed to load UIFireMeter.png" << std::endl;
        return false;
    }
    if (!fireBarMaskTexture_.loadFromFile("textures/UIFireMeter_Mask.png"))
    {
        std::cerr << "Failed to load UIFireMeter_Mask.png" << std::endl;
        return false;
    }

    setupWaterBar();
    setupFireBar();

    return true;
}

void GameUI::setupWaterBar()
{
    // Create render texture for water fill
    waterProgressTex_.create(
        static_cast<unsigned int>(waterBarSize_.x),
        static_cast<unsigned int>(waterBarSize_.y));

    // Setup container sprite
    waterContainer_.setTexture(waterTexture_);
    waterContainer_.setPosition(waterBarPos_);
    waterContainer_.setScale(
        waterBarSize_.x / waterTexture_.getSize().x,
        waterBarSize_.y / waterTexture_.getSize().y);

    // Setup fill sprite
    waterFillSprite_.setTexture(waterProgressTex_.getTexture());
    waterFillSprite_.setPosition(waterBarPos_);
}

void GameUI::setupFireBar()
{
    // Create render texture for fire fill
    fireProgressTex_.create(fireSize_.x, fireSize_.y);

    // Setup fire bar sprite
    fireBar_.setTexture(fireBarTexture_);
    fireBar_.setScale(0.8f, 0.8f);
    fireBar_.setPosition(
        window_.getSize().x / 2.0f - (fireSize_.x * 0.8f) / 2.0f, 10.f);

    // Setup fire fill sprite
    fireFillSprite_.setTexture(fireProgressTex_.getTexture());
    fireFillSprite_.setScale(0.8f, 0.8f);
    fireFillSprite_.setPosition(fireBar_.getPosition());
}

void GameUI::updateWaterBar()
{
    waterProgressTex_.clear(sf::Color::Transparent);

    // Draw blue fill (bottom-up)
    sf::RectangleShape fill(sf::Vector2f(waterBarSize_.x, waterBarSize_.y * waterProgress_));
    fill.setFillColor(sf::Color(0, 130, 255, 255));
    fill.setPosition(0, waterBarSize_.y * (1.f - waterProgress_));
    waterProgressTex_.draw(fill);

    // Apply mask
    sf::Sprite mask(waterMaskTexture_);
    mask.setScale(
        waterBarSize_.x / waterMaskTexture_.getSize().x,
        waterBarSize_.y / waterMaskTexture_.getSize().y);
    waterProgressTex_.draw(mask, sf::BlendMode(
                                     sf::BlendMode::Zero,
                                     sf::BlendMode::SrcAlpha));

    waterProgressTex_.display();
}

void GameUI::updateFireBar()
{
    fireProgressTex_.clear(sf::Color::Transparent);

    // Draw fire fill (left to right)
    float fireProgressWidth = fireSize_.x * fireProgress_;
    sf::RectangleShape fireFill(sf::Vector2f(fireProgressWidth, fireSize_.y));
    fireFill.setFillColor(sf::Color(255, 120, 80, 255));
    fireFill.setPosition(0, 0);
    fireProgressTex_.draw(fireFill);

    // Apply mask
    sf::Sprite fireMask(fireBarMaskTexture_);
    fireMask.setPosition(0, 0);
    fireProgressTex_.draw(fireMask, sf::BlendMode(
                                        sf::BlendMode::Zero,
                                        sf::BlendMode::SrcAlpha));

    fireProgressTex_.display();
}

void GameUI::setWaterLevel(float progress)
{
    waterProgress_ = std::max(0.f, std::min(1.f, progress));
}

void GameUI::setFireLevel(float progress)
{
    fireProgress_ = std::max(0.f, std::min(1.f, progress));
}

void GameUI::render()
{
    // Update both bars
    updateWaterBar();
    updateFireBar();

    // Draw water tank
    window_.draw(waterFillSprite_);
    window_.draw(waterContainer_);

    // Draw fire meter
    window_.draw(fireFillSprite_);
    window_.draw(fireBar_);
}