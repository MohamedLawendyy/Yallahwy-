// File: src/Renderer.cpp (UPDATED - Fix Right Edge Upside Down Issue)
#include "Renderer.hpp"
#include "TileType.hpp"
#include <cmath>
#include <iostream>

Renderer::Renderer(sf::RenderWindow &window, const Map &map)
    : window_(window), map_(map)
{
    loadAssets();
}

bool Renderer::loadAssets()
{
    std::cout << "Loading sprites from tilemap editor paths..." << std::endl;

    // BASE LAYER
    loadTexture("floor", "pics/Floor.png");
    loadTexture("wall_mid_top", "pics/wmid.png");
    loadTexture("wall_mid_bottom", "pics/wbottom.png");
    loadTexture("wall_class_top", "pics/classwmid.png");
    loadTexture("wall_class_bottom", "pics/classwbottom.png");
    loadTexture("wall_sep_left", "pics/LW.png");
    loadTexture("wall_sep_right", "pics/RW.png");
    loadTexture("wall_sep_bottom", "pics/BW.png");
    loadTexture("wall_corner_bl1", "pics/LBC.png");
    loadTexture("wall_corner_bl2", "pics/LBC2.png");
    loadTexture("wall_corner_br1", "pics/RBC.png");
    loadTexture("wall_corner_br2", "pics/RBC2.png");
    loadTexture("wall_corner_tl", "pics/LTC.png");
    loadTexture("wall_corner_tr", "pics/RTC.png");

    // OVERLAY LAYER
    loadTexture("entrance", "pics/Overlay/Entrance.png");
    loadTexture("door_top", "pics/Overlay/Door_Top.png");
    loadTexture("door_bottom", "pics/Overlay/Door_Bottom.png");
    loadTexture("door_open_top", "pics/Overlay/Door_Opened_Top.png");
    loadTexture("door_open_bottom", "pics/Overlay/Door_Opened_Bottom.png");
    loadTexture("clock", "pics/Overlay/Clock.png");
    loadTexture("desk_tl", "pics/Overlay/DeskTL.png");
    loadTexture("desk_tr", "pics/Overlay/DeskTR.png");
    loadTexture("desk_bl", "pics/Overlay/DeskBL.png");
    loadTexture("desk_br", "pics/Overlay/DeskBR.png");
    loadTexture("plant_top", "pics/Overlay/PlantPot_Top.png");
    loadTexture("plant_bottom", "pics/Overlay/PlantPot_Bottom.png");
    loadTexture("chair", "pics/Overlay/Chair.png");
    loadTexture("locker_top", "pics/Overlay/Locker_Top.png");
    loadTexture("locker_bottom", "pics/Overlay/Locker_Bottom.png");
    loadTexture("bookshelf_tl", "pics/Overlay/Bookshelf_TL.png");
    loadTexture("bookshelf_tr", "pics/Overlay/Bookshelf_TR.png");
    loadTexture("bookshelf_bl", "pics/Overlay/Bookshelf_BL.png");
    loadTexture("bookshelf_br", "pics/Overlay/Bookshelf_BR.png");
    loadTexture("box", "pics/Overlay/Box.png");
    loadTexture("vent", "pics/Overlay/Vent.png");
    loadTexture("water_top", "pics/Overlay/WaterTop.png");
    loadTexture("water_bottom", "pics/Overlay/WaterBottom.png");
    loadTexture("board_tl", "pics/Overlay/Board_TL.png");
    loadTexture("board_tr", "pics/Overlay/Board_TR.png");
    loadTexture("board_bl", "pics/Overlay/Board_BL.png");
    loadTexture("board_br", "pics/Overlay/Board_BR.png");

    // BACKGROUND & FX
    if (loadTexture("background", "pics/background.jpg"))
    {
        std::cout << "✓ background loaded" << std::endl;
    }
    if (textures_.count("floor"))
        textures_["grass"] = textures_["floor"];
    if (textures_.count("floor"))
        textures_["burnt"] = textures_["floor"];
    loadFireAnimation("pics/effects/fire_spritesheet.png");

    return true;
}

bool Renderer::loadTexture(const std::string &name, const std::string &filename)
{
    sf::Texture texture;
    if (texture.loadFromFile(filename))
    {
        texture.setSmooth(true);
        textures_[name] = texture;
        return true;
    }
    std::cerr << "✗ Failed to load: " << filename << std::endl;
    return false;
}

bool Renderer::loadFireAnimation(const std::string &filename)
{
    sf::Texture spritesheet;
    if (!spritesheet.loadFromFile(filename))
        return false;
    spritesheet.setSmooth(true);
    fireAnimationSheet_ = spritesheet;
    hasFireAnimation_ = true;
    fireFrameWidth_ = spritesheet.getSize().x / 10;
    fireFrameHeight_ = spritesheet.getSize().y;
    return true;
}

sf::Texture *Renderer::getTexture(const std::string &name)
{
    auto it = textures_.find(name);
    return (it != textures_.end()) ? &it->second : nullptr;
}

// ========================================================
// FIXED: ID MAPPING (Reverted Top/Sides, Kept Bottom/Mids)
// ========================================================
std::string Renderer::getTextureNameForEditorId(int editorId)
{
    switch (editorId)
    {
    case 1:
        return "floor";

    // Middle Walls (Keep Swapped - this was correct)
    case 2:
        return "wall_mid_bottom";
    case 3:
        return "wall_mid_top";

    case 4:
        return "wall_class_top";
    case 5:
        return "wall_class_bottom";

    // REVERT 1: Left/Right Walls (Restore original)
    case 6:
        return "wall_sep_left"; // Left is Left
    case 7:
        return "wall_sep_right"; // Right is Right

    case 8:
        return "wall_sep_bottom";

    // Bottom Corners (Keep Swapped - this fixed the previous bug)
    case 9:
        return "wall_corner_br2";
    case 10:
        return "wall_corner_br1";
    case 11:
        return "wall_corner_bl2";
    case 12:
        return "wall_corner_bl1";

    // REVERT 2: Top Corners (Restore original)
    case 13:
        return "wall_corner_tl"; // Top Left is TL
    case 14:
        return "wall_corner_tr"; // Top Right is TR

    // Overlay Objects
    case 15:
        return "entrance";
    case 16:
        return "door_top";
    case 17:
        return "door_bottom";
    case 18:
        return "door_open_top";
    case 19:
        return "door_open_bottom";
    case 20:
        return "clock";
    case 21:
        return "chair";
    case 22:
        return "desk_tl";
    case 23:
        return "desk_tr";
    case 24:
        return "desk_bl";
    case 25:
        return "desk_br";
    case 26:
        return "plant_top";
    case 27:
        return "plant_bottom";
    case 28:
        return "locker_top";
    case 29:
        return "locker_bottom";
    case 30:
        return "bookshelf_tl";
    case 31:
        return "bookshelf_tr";
    case 32:
        return "bookshelf_bl";
    case 33:
        return "bookshelf_br";
    case 34:
        return "floor";
    case 35:
        return "floor";
    case 36:
        return "box";
    case 37:
        return "vent";
    case 38:
        return "water_top";
    case 39:
        return "water_bottom";
    case 40:
        return "board_tl";
    case 41:
        return "board_tr";
    case 42:
        return "board_bl";
    case 43:
        return "board_br";

    default:
        return "floor";
    }
}

std::string Renderer::getFallbackTextureName(TileState state)
{
    switch (state)
    {
    case TileState::Wall:
        return "wall_mid_top";
    case TileState::Floor:
        return "floor";
    case TileState::Flammable:
        return "grass";
    case TileState::Burnt:
        return "burnt";
    case TileState::Prop:
        return "chair";
    case TileState::Door:
        return "door_top";
    default:
        return "floor";
    }
}

void Renderer::render()
{
    if (sf::Texture *bgTex = getTexture("background"))
    {
        sf::Sprite bgSprite(*bgTex);
        sf::Vector2u texSize = bgTex->getSize();
        for (float y = 0; y < Map::HEIGHT * Map::TILE_SIZE; y += texSize.y)
        {
            for (float x = 0; x < Map::WIDTH * Map::TILE_SIZE; x += texSize.x)
            {
                bgSprite.setPosition(x, y);
                window_.draw(bgSprite);
            }
        }
    }
    else
    {
        window_.clear(sf::Color(20, 20, 30));
    }

    const auto &grid = map_.getGrid();
    for (size_t y = 0; y < Map::HEIGHT; ++y)
    {
        for (size_t x = 0; x < Map::WIDTH; ++x)
        {
            renderTile(x, y, grid[y][x]);
        }
    }
}

void Renderer::renderTile(size_t x, size_t y, const Tile &tile)
{
    sf::Vector2f position(x * Map::TILE_SIZE, y * Map::TILE_SIZE);

    if (tile.state == TileState::Empty || (tile.state == TileState::Floor && tile.editorTileId == 0))
        return;

    std::string baseTexName;
    sf::Color baseTint = sf::Color::White;

    if (tile.state == TileState::Burnt)
    {
        if (tile.editorTileId != 0)
            baseTexName = getTextureNameForEditorId(tile.editorTileId);
        else
            baseTexName = "burnt";
        baseTint = sf::Color(60, 60, 60);
    }
    else if (tile.state == TileState::Burning)
    {
        if (tile.editorTileId != 0)
            baseTexName = getTextureNameForEditorId(tile.editorTileId);
        else
            baseTexName = "grass";
        baseTint = sf::Color(255, 100, 100);
    }
    else
    {
        if (tile.editorTileId != 0)
            baseTexName = getTextureNameForEditorId(tile.editorTileId);
        else
            baseTexName = getFallbackTextureName(tile.state);
        if (tile.state == TileState::Flammable && tile.editorTileId == 0)
            baseTint = sf::Color(100, 200, 100);
    }

    if (sf::Texture *tex = getTexture(baseTexName))
        drawSprite(*tex, position, baseTint);
    else
    {
        tileShape_.setPosition(position);
        tileShape_.setSize(sf::Vector2f(Map::TILE_SIZE, Map::TILE_SIZE));
        tileShape_.setFillColor(getFallbackColor(tile.state));
        window_.draw(tileShape_);
    }

    if (tile.overlayTileId != 0)
    {
        std::string overlayTexName = getTextureNameForEditorId(tile.overlayTileId);
        if (sf::Texture *tex = getTexture(overlayTexName))
        {
            sf::Color overlayTint = (tile.state == TileState::Burnt) ? sf::Color(60, 60, 60) : sf::Color::White;
            drawSprite(*tex, position, overlayTint);
        }
    }

    if (tile.state == TileState::Burning)
        renderFireAnimation(position, tile.burnTime, x, y);
}

void Renderer::drawSprite(sf::Texture &texture, sf::Vector2f position, sf::Color tint)
{
    tileSprite_.setTexture(texture);
    tileSprite_.setPosition(position);
    tileSprite_.setColor(tint);
    sf::Vector2u texSize = texture.getSize();
    tileSprite_.setScale(Map::TILE_SIZE / (float)texSize.x, Map::TILE_SIZE / (float)texSize.y);
    window_.draw(tileSprite_);
}

void Renderer::renderFireAnimation(sf::Vector2f position, float burnTime, size_t tileX, size_t tileY)
{
    if (hasFireAnimation_)
    {
        float time = animationClock_.getElapsedTime().asSeconds();
        int frame = static_cast<int>((time * 15.0f + tileX)) % 10;
        fireSprite_.setTexture(fireAnimationSheet_);
        fireSprite_.setTextureRect(sf::IntRect(frame * fireFrameWidth_, 0, fireFrameWidth_, fireFrameHeight_));
        fireSprite_.setPosition(position);
        fireSprite_.setScale(Map::TILE_SIZE / (float)fireFrameWidth_, Map::TILE_SIZE / (float)fireFrameHeight_);
        window_.draw(fireSprite_);
    }
    else
    {
        tileShape_.setPosition(position);
        tileShape_.setSize(sf::Vector2f(Map::TILE_SIZE, Map::TILE_SIZE));
        tileShape_.setFillColor(sf::Color(255, 100, 0, 180));
        window_.draw(tileShape_);
    }
}

sf::Color Renderer::getFallbackColor(TileState state) const
{
    switch (state)
    {
    case TileState::Wall:
        return sf::Color(100, 100, 120);
    case TileState::Floor:
        return sf::Color(50, 50, 60);
    case TileState::Flammable:
        return sf::Color(34, 139, 34);
    case TileState::Burning:
        return sf::Color(255, 69, 0);
    case TileState::Burnt:
        return sf::Color(30, 30, 30);
    case TileState::Prop:
        return sf::Color(139, 69, 19);
    default:
        return sf::Color::Black;
    }
}

void Renderer::setZoom(float zoom)
{
    zoomLevel_ = std::max(0.5f, std::min(2.0f, zoom));
    camera_.setSize(Map::WIDTH * Map::TILE_SIZE / zoomLevel_, Map::HEIGHT * Map::TILE_SIZE / zoomLevel_);
}
void Renderer::panView(float dx, float dy) { camera_.move(dx, dy); }
void Renderer::resetView()
{
    float w = Map::WIDTH * Map::TILE_SIZE;
    float h = Map::HEIGHT * Map::TILE_SIZE;
    camera_.setSize(w, h);
    camera_.setCenter(w / 2, h / 2);
    zoomLevel_ = 1.0f;
}
void Renderer::updateView() { window_.setView(camera_); }