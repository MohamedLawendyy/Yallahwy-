// File: src/GameOverScreen.cpp
#include "GameOverScreen.hpp"
#include "cmath"
#include <iostream>

GameOverScreen::GameOverScreen(sf::RenderWindow& window)
    : window_(window),
      currentResult_(GameResult::None),
      isVisible_(false),
      restart_(false),
      returnToMenu_(false),
      animationTime_(0.f)
{
}

GameOverScreen::~GameOverScreen() {
    sound_.stop();
}

bool GameOverScreen::loadAssets() {
    // Load font
    if (!font_.loadFromFile("fonts/DynaPuff-Bold.ttf")) {
        std::cerr << "Failed to load DynaPuff-Bold.ttf" << std::endl;
        return false;
    }
    
    // Load sound effects (optional)
    if (winSoundBuffer_.loadFromFile("sounds/win.ogg")) {
        std::cout << "✓ Win sound loaded" << std::endl;
    } else {
        std::cout << "! Win sound not found (optional)" << std::endl;
    }
    
    if (loseSoundBuffer_.loadFromFile("sounds/lose.ogg")) {
        std::cout << "✓ Lose sound loaded" << std::endl;
    } else {
        std::cout << "! Lose sound not found (optional)" << std::endl;
    }
    
    setupShapes();
    return true;
}

void GameOverScreen::setupShapes() {
    sf::Vector2u windowSize = window_.getSize();
    
    // Dark overlay (semi-transparent black background)
    darkOverlay_.setSize(sf::Vector2f(windowSize.x, windowSize.y));
    darkOverlay_.setFillColor(sf::Color(0, 0, 0, 180));
    darkOverlay_.setPosition(0, 0);
    
    // Panel (popup window)
    panel_.setSize(sf::Vector2f(800, 500));
    panel_.setFillColor(sf::Color(40, 40, 50, 255));
    panel_.setOutlineThickness(8);
    panel_.setOutlineColor(sf::Color(255, 200, 0, 255));
    panel_.setOrigin(400, 250);
    panel_.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
    
    // Title text
    titleText_.setFont(font_);
    titleText_.setCharacterSize(100);
    titleText_.setFillColor(sf::Color::White);
    titleText_.setOutlineThickness(5);
    titleText_.setOutlineColor(sf::Color::Black);
    
    // Message text
    messageText_.setFont(font_);
    messageText_.setCharacterSize(40);
    messageText_.setFillColor(sf::Color(200, 200, 200));
    messageText_.setOutlineThickness(2);
    messageText_.setOutlineColor(sf::Color::Black);
    
    // Restart button
    restartBtn_.setSize(sf::Vector2f(350, 80));
    restartBtn_.setFillColor(sf::Color(60, 180, 60, 255));
    restartBtn_.setOutlineThickness(4);
    restartBtn_.setOutlineColor(sf::Color::White);
    restartBtn_.setOrigin(175, 40);
    restartBtn_.setPosition(windowSize.x / 2.f - 200, windowSize.y / 2.f + 120);
    
    // Menu button
    menuBtn_.setSize(sf::Vector2f(350, 80));
    menuBtn_.setFillColor(sf::Color(180, 60, 60, 255));
    menuBtn_.setOutlineThickness(4);
    menuBtn_.setOutlineColor(sf::Color::White);
    menuBtn_.setOrigin(175, 40);
    menuBtn_.setPosition(windowSize.x / 2.f + 200, windowSize.y / 2.f + 120);
    
    // Button text
    restartText_.setFont(font_);
    restartText_.setString("Restart");
    restartText_.setCharacterSize(50);
    restartText_.setFillColor(sf::Color::White);
    restartText_.setOutlineThickness(3);
    restartText_.setOutlineColor(sf::Color::Black);
    restartText_.setOrigin(
        restartText_.getLocalBounds().width / 2,
        restartText_.getLocalBounds().height / 2
    );
    restartText_.setPosition(restartBtn_.getPosition());
    
    menuText_.setFont(font_);
    menuText_.setString("Menu");
    menuText_.setCharacterSize(50);
    menuText_.setFillColor(sf::Color::White);
    menuText_.setOutlineThickness(3);
    menuText_.setOutlineColor(sf::Color::Black);
    menuText_.setOrigin(
        menuText_.getLocalBounds().width / 2,
        menuText_.getLocalBounds().height / 2
    );
    menuText_.setPosition(menuBtn_.getPosition());
}

void GameOverScreen::show(GameResult result) {
    currentResult_ = result;
    isVisible_ = true;
    animationTime_ = 0.f;
    restart_ = false;
    returnToMenu_ = false;
    
    sf::Vector2u windowSize = window_.getSize();
    
    // Set title and message based on result
    if (result == GameResult::Win) {
        titleText_.setString("VICTORY!");
        titleText_.setFillColor(sf::Color(100, 255, 100));
        messageText_.setString("All fires extinguished!\nYou saved the building!");
        panel_.setOutlineColor(sf::Color(100, 255, 100, 255));
    } else {
        titleText_.setString("DEFEAT");
        titleText_.setFillColor(sf::Color(255, 100, 100));
        messageText_.setString("The fire spread everywhere!\nThe building is lost...");
        panel_.setOutlineColor(sf::Color(255, 100, 100, 255));
    }
    
    // Center title
    titleText_.setOrigin(
        titleText_.getLocalBounds().width / 2,
        titleText_.getLocalBounds().height / 2
    );
    titleText_.setPosition(windowSize.x / 2.f, windowSize.y / 2.f - 120);
    
    // Center message
    messageText_.setOrigin(
        messageText_.getLocalBounds().width / 2,
        messageText_.getLocalBounds().height / 2
    );
    messageText_.setPosition(windowSize.x / 2.f, windowSize.y / 2.f);
    
    // Play sound
    playResultSound(result);
    
    std::cout << "\n========================================" << std::endl;
    if (result == GameResult::Win) {
        std::cout << "🎉 VICTORY! All fires extinguished!" << std::endl;
    } else {
        std::cout << "💥 DEFEAT! Fire spread everywhere!" << std::endl;
    }
    std::cout << "========================================\n" << std::endl;
}

void GameOverScreen::update(float dt) {
    if (!isVisible_) return;
    
    animationTime_ += dt;
    updateHoverEffects();
    
    // Animate panel scale (bounce effect)
    float scale = 1.0f + 0.05f * std::sin(animationTime_ * 3.0f);
    panel_.setScale(scale, scale);
    
    // Also animate title slightly
    float titleScale = 1.0f + 0.03f * std::sin(animationTime_ * 4.0f);
    titleText_.setScale(titleScale, titleScale);
}

void GameOverScreen::updateHoverEffects() {
    sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window_));
    
    // Restart button hover
    if (restartBtn_.getGlobalBounds().contains(mousePos)) {
        restartBtn_.setFillColor(sf::Color(80, 220, 80, 255));
        restartText_.setScale(1.1f, 1.1f);
    } else {
        restartBtn_.setFillColor(sf::Color(60, 180, 60, 255));
        restartText_.setScale(1.f, 1.f);
    }
    
    // Menu button hover
    if (menuBtn_.getGlobalBounds().contains(mousePos)) {
        menuBtn_.setFillColor(sf::Color(220, 80, 80, 255));
        menuText_.setScale(1.1f, 1.1f);
    } else {
        menuBtn_.setFillColor(sf::Color(180, 60, 60, 255));
        menuText_.setScale(1.f, 1.f);
    }
}

void GameOverScreen::handleEvent(const sf::Event& event) {
    if (!isVisible_) return;
    
    if (event.type == sf::Event::MouseButtonPressed && 
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        handleMouseClick(mousePos);
    }
    
    // Keyboard shortcuts
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::R) {
            restart_ = true;
            isVisible_ = false;
        } else if (event.key.code == sf::Keyboard::Escape) {
            returnToMenu_ = true;
            isVisible_ = false;
        }
    }
}

void GameOverScreen::handleMouseClick(const sf::Vector2f& mousePos) {
    // Restart button
    if (restartBtn_.getGlobalBounds().contains(mousePos)) {
        std::cout << "🔄 Restart pressed" << std::endl;
        restart_ = true;
        isVisible_ = false;
        sound_.stop();
    }
    
    // Menu button
    if (menuBtn_.getGlobalBounds().contains(mousePos)) {
        std::cout << "📋 Menu pressed" << std::endl;
        returnToMenu_ = true;
        isVisible_ = false;
        sound_.stop();
    }
}

void GameOverScreen::render() {
    if (!isVisible_) return;
    
    // Draw dark overlay
    window_.draw(darkOverlay_);
    
    // Draw panel
    window_.draw(panel_);
    
    // Draw text
    window_.draw(titleText_);
    window_.draw(messageText_);
    
    // Draw buttons
    window_.draw(restartBtn_);
    window_.draw(menuBtn_);
    window_.draw(restartText_);
    window_.draw(menuText_);
}

void GameOverScreen::playResultSound(GameResult result) {
    if (result == GameResult::Win && winSoundBuffer_.getDuration() != sf::Time::Zero) {
        sound_.setBuffer(winSoundBuffer_);
        sound_.setVolume(50.f);
        sound_.play();
    } else if (result == GameResult::Lose && loseSoundBuffer_.getDuration() != sf::Time::Zero) {
        sound_.setBuffer(loseSoundBuffer_);
        sound_.setVolume(50.f);
        sound_.play();
    }
}

void GameOverScreen::reset() {
    isVisible_ = false;
    restart_ = false;
    returnToMenu_ = false;
    animationTime_ = 0.f;
    sound_.pause();
}