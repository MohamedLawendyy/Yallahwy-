// File: src/FireSpreadLogic.cpp (UPDATED - Much slower fire spread)
#include "FireSpreadLogic.hpp"
#include "FireSimulator.hpp"
#include <cmath>
#include <iostream>

FireSpreadLogic::FireSpreadLogic(Map& map, FireSimulator& fireSim)
    : map_(map), fireSim_(fireSim), 
      rng_(std::random_device{}()),
      randomDist_(0.0f, 1.0f) {}

void FireSpreadLogic::update(float deltaTime) {
    // Get all currently burning tiles
    std::vector<std::pair<size_t, size_t>> burningTiles;
    
    for (size_t y = 0; y < Map::HEIGHT; ++y) {
        for (size_t x = 0; x < Map::WIDTH; ++x) {
            if (map_.getTileState(x, y) == TileState::Burning) {
                burningTiles.emplace_back(x, y);
            }
        }
    }
    
    // For each burning tile, spread heat to neighbors
    for (const auto& [x, y] : burningTiles) {
        const Tile& tile = map_.getTile(x, y);
        float burnProgress = tile.burnTime / fireSim_.getConfig().burnDuration;
        
        // MUCH SLOWER spread intensity calculation
        // Fire spreads very slowly, especially at the beginning
        float spreadIntensity = 0.8f; // Base slow spread (10% of original)
        
        if (burnProgress < 0.5f) {
            // Very early stage: extremely slow spread (5% speed)
            spreadIntensity = 0.5f;
        }
        else if (burnProgress < 0.8f) {
            // Middle stage: slow spread (10% speed)
            spreadIntensity = 0.1f;
        }
        else {
            // Late stage: moderate spread (20% speed) - still much slower than before
            spreadIntensity = 0.2f;
        }
        
        // Apply the slow spread with deltaTime
        spreadFromTile(x, y, spreadIntensity * deltaTime);
    }
}

void FireSpreadLogic::spreadFromTile(size_t x, size_t y, float heatAmount) {
    auto neighbors = getSpreadNeighbors(x, y);
    
    if (neighbors.empty()) return;
    
    // Distribute heat among neighbors
    float heatPerNeighbor = heatAmount / neighbors.size();
    
    for (const auto& [nx, ny] : neighbors) {
        if (!map_.isValidPosition(nx, ny)) continue;
        
        Tile& neighborTile = map_.getTile(nx, ny);
        
        // Only flammable tiles can accumulate heat
        if (neighborTile.state == TileState::Flammable) {
            // SLOWER heat accumulation with MORE randomness
            // This makes fire spread unpredictable and very gradual
            float randomFactor = 0.3f + randomDist_(rng_) * 0.4f; // 0.3 to 0.7 (reduced from 0.8-1.2)
            
            float heatToAdd = heatPerNeighbor * randomFactor;
            neighborTile.heatAccumulated += heatToAdd;
            
            // Check for ignition (but with higher threshold due to slower heat buildup)
            if (neighborTile.heatAccumulated >= neighborTile.ignitionThreshold) {
                fireSim_.igniteAt(nx, ny);
            }
        }
    }
}

bool FireSpreadLogic::canSpreadToTile(size_t x, size_t y) const {
    if (!map_.isValidPosition(x, y)) return false;
    
    TileState state = map_.getTileState(x, y);
    
    // Fire can only spread to flammable tiles
    return state == TileState::Flammable;
}

std::vector<std::pair<size_t, size_t>> FireSpreadLogic::getSpreadNeighbors(size_t x, size_t y) const {
    std::vector<std::pair<size_t, size_t>> neighbors;
    const auto& config = fireSim_.getConfig();
    
    // Always check orthogonal neighbors (adjacent tiles)
    std::vector<std::pair<int, int>> directions = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}  // Left, Right, Up, Down
    };
    
    // Add diagonal directions if allowed (usually not for realistic fire)
    if (config.allowDiagonalSpread) {
        directions.insert(directions.end(), {
            {-1, -1}, {1, -1}, {-1, 1}, {1, 1}  // Diagonals
        });
    }
    
    // Check each direction
    for (const auto& [dx, dy] : directions) {
        int nx = static_cast<int>(x) + dx;
        int ny = static_cast<int>(y) + dy;
        
        if (canSpreadToTile(nx, ny)) {
            neighbors.emplace_back(nx, ny);
        }
    }
    
    return neighbors;
}