// File: src/MainMenu.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class MainMenu
{
public:
    // CHANGE: Accepts persistent music reference
    MainMenu(sf::RenderWindow &window, sf::Music &music);
    ~MainMenu();

    bool loadAssets();
    void update(float dt);
    void render();
    void handleEvent(const sf::Event &event);

    bool shouldStartGame() const { return startGame_; }
    bool shouldExit() const { return exitGame_; }
    void reset();

private:
    sf::RenderWindow &window_;
    sf::Music &music_; // Reference to Game's music

    // Textures & Shapes
    sf::Texture menuBGTexture_, menuTexture_, logoTexture_, volTexture_, volFireTexture_;
    sf::RectangleShape menuBG_, menu_, logo_, vol_, volFire_, startBtn_, exitBtn_;

    // Font & Text
    sf::Font font_;
    sf::Text startText_, exitText_;

    // Animation
    float bgOffset_;
    float bgSpeed_;

    // State
    bool mute_;
    bool startGame_;
    bool exitGame_;

    // Helper methods
    void setupShapes();
    void updateBackground(float dt);
    void updateHoverEffects();
    void handleMouseClick(const sf::Vector2f &mousePos);
};