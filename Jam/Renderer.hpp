// File: src/Renderer.hpp (UPDATED - Editor ID mapping)
#pragma once
#include <SFML/Graphics.hpp>
#include "Map.hpp"
#include "TileType.hpp"
#include <memory>
#include <unordered_map>
#include <string>

class Renderer
{
public:
    Renderer(sf::RenderWindow &window, const Map &map);

    void render();
    void updateView();

    // Camera control
    void setZoom(float zoom);
    void panView(float dx, float dy);
    void resetView();

    const sf::View &getCamera() const { return camera_; }
    bool loadAssets();
    // In Renderer.hpp, add this public method:
    // void debugTileAtPosition(int worldX, int worldY, const Map &map);

private:
    sf::RenderWindow &window_;
    const Map &map_;
    sf::View camera_;
    float zoomLevel_ = 1.0f;

    // Texture storage - simple map
    std::unordered_map<std::string, sf::Texture> textures_;

    // Fire animation
    sf::Texture fireAnimationSheet_;
    sf::Sprite fireSprite_;
    bool hasFireAnimation_ = false;
    int fireFrameWidth_ = 64;
    int fireFrameHeight_ = 64;

    // Sprites
    sf::Sprite tileSprite_;
    sf::RectangleShape tileShape_;
    sf::Clock animationClock_;

    // Rendering
    void renderTile(size_t x, size_t y, const Tile &tile);
    void renderFireAnimation(sf::Vector2f position, float burnTime, size_t tileX, size_t tileY);

    // Texture management
    bool loadTexture(const std::string &name, const std::string &filename);
    bool loadFireAnimation(const std::string &filename);
    sf::Texture *getTexture(const std::string &name);

    // NEW: Editor ID to texture name mapping
    std::string getTextureNameForEditorId(int editorId);
    std::string getFallbackTextureName(TileState state);

    // Drawing helpers
    void drawSprite(sf::Texture &texture, sf::Vector2f position, sf::Color tint);
    sf::Color getFallbackColor(TileState state) const;
};