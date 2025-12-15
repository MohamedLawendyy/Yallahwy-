// File: src/FireSpreadLogic.hpp
#pragma once
#include "Map.hpp"
#include <vector>
#include <random>
class FireSimulator; // <-- ADD THIS LINE (Forward Declaration)

class FireSpreadLogic
{
public:
    FireSpreadLogic(Map &map, FireSimulator &fireSim);

    // Update fire spread based on heat accumulation
    void update(float deltaTime);

    // Manually spread fire from a burning tile
    void spreadFromTile(size_t x, size_t y, float heatAmount);

    // Check if fire can spread to a tile (no collision check)
    bool canSpreadToTile(size_t x, size_t y) const;

    // Get all valid neighbor positions for spreading
    std::vector<std::pair<size_t, size_t>> getSpreadNeighbors(size_t x, size_t y) const;

private:
    Map &map_;
    FireSimulator &fireSim_;

    // Random number generator for probabilistic spreading
    std::mt19937 rng_;
    std::uniform_real_distribution<float> randomDist_;

    // Fire spread parameters
    float baseSpreadChance_ = 0.8f;
    float diagonalSpreadChance_ = 0.4f;
    float spreadRadius_ = 1.5f; // Tiles
};