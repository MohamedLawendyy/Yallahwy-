// File: src/GameOverScreen.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

enum class GameResult {
    None,
    Win,
    Lose
};

class GameOverScreen {
public:
    GameOverScreen(sf::RenderWindow& window);
    ~GameOverScreen();
    
    bool loadAssets();
    void show(GameResult result);
    void update(float dt);
    void render();
    void handleEvent(const sf::Event& event);
    
    bool shouldRestart() const { return restart_; }
    bool shouldReturnToMenu() const { return returnToMenu_; }
    void reset();
    
private:
    sf::RenderWindow& window_;
    
    // Textures
    sf::Texture overlayTexture_;
    sf::Texture winPanelTexture_;
    sf::Texture losePanelTexture_;
    
    // Shapes
    sf::RectangleShape darkOverlay_;
    sf::RectangleShape panel_;
    sf::RectangleShape restartBtn_;
    sf::RectangleShape menuBtn_;
    
    // Font & Text
    sf::Font font_;
    sf::Text titleText_;
    sf::Text messageText_;
    sf::Text restartText_;
    sf::Text menuText_;
    
    // Sound effects
    sf::SoundBuffer winSoundBuffer_;
    sf::SoundBuffer loseSoundBuffer_;
    sf::Sound sound_;
    
    // State
    GameResult currentResult_;
    bool isVisible_;
    bool restart_;
    bool returnToMenu_;
    float animationTime_;
    
    // Helper methods
    void setupShapes();
    void updateHoverEffects();
    void handleMouseClick(const sf::Vector2f& mousePos);
    void playResultSound(GameResult result);
};