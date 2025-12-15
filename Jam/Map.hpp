// File: src/Map.hpp (UPDATED - Full tilemap editor integration)
#pragma once
#include <vector>
#include <random>
#include <string>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "TileType.hpp"

struct MapConfig
{
    float flammableDensity = 0.65f; // Percentage of floors to convert to flammable
};

class Map
{
public:
    static size_t WIDTH;
    static size_t HEIGHT;
    static float TILE_SIZE;

    static void initializeDimensions(unsigned int screenWidth, unsigned int screenHeight);

    Map() = default;
    Map(const MapConfig &config, b2World *physicsWorld = nullptr);

    // Tile access
    TileState getTileState(size_t x, size_t y) const;
    Tile &getTile(size_t x, size_t y);
    const Tile &getTile(size_t x, size_t y) const;
    void setTileState(size_t x, size_t y, TileState state);

    // Validation
    bool isValidPosition(int x, int y) const;
    bool isWalkable(int x, int y) const;

    // Collision
    sf::FloatRect getTileCollisionRect(int x, int y) const;
    bool canPlaceBody(sf::FloatRect bodyRect) const;

    // Coordinate conversion
    std::pair<int, int> worldToGrid(sf::Vector2f worldPos) const;
    sf::Vector2f gridToWorld(size_t x, size_t y) const;

    // Grid access
    const std::vector<std::vector<Tile>> &getGrid() const { return grid_; }

    // Safe spawn point
    sf::Vector2f getSafeSpawnPoint() const;

    // Utility
    void reset();
    void setConfig(const MapConfig &config) { config_ = config; }

    float getWorldWidth() const { return WIDTH * TILE_SIZE; }
    float getWorldHeight() const { return HEIGHT * TILE_SIZE; }

    // CSV loading from tilemap editor
    bool loadFromCSV(const std::string &filename);

    // Physics
    void createPhysicsColliders();

    // Check if a tile has flammable furniture
    bool isFurnitureTile(size_t x, size_t y) const
    {
        if (!isValidPosition(x, y))
            return false;

        const Tile &tile = grid_[y][x];
        int overlayId = tile.overlayTileId;

        // Check if overlay is flammable furniture
        return overlayId == 21 || // Chair
               overlayId == 22 || // Desk TL
               overlayId == 23 || // Desk TR
               overlayId == 24 || // Desk BL
               overlayId == 25 || // Desk BR
               overlayId == 26 || // Plant Top
               overlayId == 27 || // Plant Bottom
               overlayId == 36;   // Box
    }

    // Get all furniture tile positions
    std::vector<std::pair<size_t, size_t>> getAllFurnitureTiles() const
    {
        std::vector<std::pair<size_t, size_t>> furniture;

        for (size_t y = 0; y < HEIGHT; ++y)
        {
            for (size_t x = 0; x < WIDTH; ++x)
            {
                if (isFurnitureTile(x, y) && grid_[y][x].state == TileState::Flammable)
                {
                    furniture.emplace_back(x, y);
                }
            }
        }

        return furniture;
    }

private:
    std::vector<std::vector<Tile>> grid_;
    std::vector<b2Body *> wallBodies_; // Track wall collision bodies
    MapConfig config_;
    b2World *physicsWorld_ = nullptr;

    // Safe spawn point
    sf::Vector2f safeSpawnPoint_;

    // Generation pipeline
    void initializeGrid();
    void clearGrid();
    void clearPhysicsBodies();

    // CSV mapping functions
    TileState mapEditorIdToTileState(int editorId);
    WallType mapEditorIdToWallType(int editorId);
    void mapOverlayToDecoration(size_t x, size_t y, int overlayId);

    // Find safe spawn point
    void findSafeSpawnPoint();

    // Auto-tiling
    void applyAutoTiling();
    WallType calculateWallType(int x, int y) const;
    bool isWall(int x, int y) const;

    // Flammable conversion
    void convertFloorToFlammable(std::mt19937 &gen);

    // Helper
    bool isSolid(int x, int y) const;

    // Physics helper
    void createWallCollider(int x, int y);
    // NEW: Mark only furniture tiles as flammable
    void markFurnitureAsFlammable();
    void createFurnitureColliders(b2World *world);
};