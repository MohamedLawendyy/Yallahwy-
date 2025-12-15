// File: src/FireSimulator.cpp (UPDATED - Slower spread, water-only extinguishing)
#include "FireSimulator.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

FireSimulator::FireSimulator(Map &map, const FireSpreadConfig &config)
    : map_(map), config_(config)
{
    fireSpreadLogic_ = std::make_unique<FireSpreadLogic>(map_, *this);
}

void FireSimulator::update(std::chrono::milliseconds deltaTime)
{
    float dt = deltaTime.count() / 1000.0f;
    updateBurningTiles(dt);
}

void FireSimulator::updateBurningTiles(float deltaTime)
{
    // Use the new fire spread logic
    fireSpreadLogic_->update(deltaTime);

    // Update existing burning tiles
    for (size_t y = 0; y < Map::HEIGHT; ++y)
    {
        for (size_t x = 0; x < Map::WIDTH; ++x)
        {
            Tile &tile = map_.getTile(x, y);

            if (tile.state == TileState::Burning)
            {
                float burnProgress = tile.burnTime / config_.burnDuration;
                float burnSpeedMultiplier = calculateBurnSpeedMultiplier(burnProgress);
                tile.burnTime += deltaTime * burnSpeedMultiplier;

                // IMPORTANT: Fire does NOT extinguish automatically
                // It only gets extinguished by water (via extinguishAt() method)
                // So we remove the auto-transition to Burnt state
                
                // Keep the fire burning indefinitely until water puts it out
                if (tile.burnTime >= config_.burnDuration)
                {
                    // Instead of extinguishing, just cap the burn time
                    // This keeps the fire burning at full intensity
                    tile.burnTime = config_.burnDuration;
                }
            }
        }
    }
}

float FireSimulator::calculateBurnSpeedMultiplier(float burnProgress) const
{
    if (burnProgress < config_.burnAccelerationPoint)
    {
        return config_.earlyBurnSpeedMultiplier;
    }
    else
    {
        float lateProgress = (burnProgress - config_.burnAccelerationPoint) /
                             (1.0f - config_.burnAccelerationPoint);

        return config_.earlyBurnSpeedMultiplier +
               (config_.lateBurnSpeedMultiplier - config_.earlyBurnSpeedMultiplier) * lateProgress;
    }
}

float FireSimulator::calculateSpreadSpeedMultiplier(float burnProgress) const
{
    if (burnProgress < config_.spreadAccelerationPoint)
    {
        return config_.earlySpreadSpeedMultiplier;
    }
    else
    {
        float lateProgress = (burnProgress - config_.spreadAccelerationPoint) /
                             (1.0f - config_.spreadAccelerationPoint);

        return config_.earlySpreadSpeedMultiplier +
               (config_.lateSpreadSpeedMultiplier - config_.earlySpreadSpeedMultiplier) * lateProgress;
    }
}

void FireSimulator::transferHeatToNeighbors(size_t x, size_t y, float heatAmount)
{
    auto neighbors = getNeighbors(x, y);

    if (neighbors.empty())
        return;

    float heatPerNeighbor = heatAmount / neighbors.size();

    for (const auto &[nx, ny] : neighbors)
    {
        if (!map_.isValidPosition(nx, ny))
            continue;

        Tile &neighborTile = map_.getTile(nx, ny);

        // Only flammable tiles can accumulate heat
        if (neighborTile.state == TileState::Flammable)
        {
            neighborTile.heatAccumulated += heatPerNeighbor;
        }
    }
}

void FireSimulator::checkIgnition(size_t x, size_t y)
{
    Tile &tile = map_.getTile(x, y);

    if (tile.heatAccumulated >= tile.ignitionThreshold)
    {
        tile.state = TileState::Burning;
        tile.burnTime = 0.0f;
        tile.heatAccumulated = 0.0f;
    }
    else if (tile.heatAccumulated > 0.0f)
    {
        bool hasFireNearby = false;
        auto neighbors = getNeighbors(x, y);
        for (const auto &[nx, ny] : neighbors)
        {
            if (map_.isValidPosition(nx, ny) &&
                map_.getTileState(nx, ny) == TileState::Burning)
            {
                hasFireNearby = true;
                break;
            }
        }

        if (!hasFireNearby)
        {
            tile.heatAccumulated *= config_.heatDissipationRate;

            if (tile.heatAccumulated < 0.01f)
            {
                tile.heatAccumulated = 0.0f;
            }
        }
    }
}

std::vector<std::pair<int, int>> FireSimulator::getNeighbors(int x, int y) const
{
    std::vector<std::pair<int, int>> neighbors;

    neighbors.push_back({x - 1, y});
    neighbors.push_back({x + 1, y});
    neighbors.push_back({x, y - 1});
    neighbors.push_back({x, y + 1});

    if (config_.allowDiagonalSpread)
    {
        neighbors.push_back({x - 1, y - 1});
        neighbors.push_back({x + 1, y - 1});
        neighbors.push_back({x - 1, y + 1});
        neighbors.push_back({x + 1, y + 1});
    }

    return neighbors;
}

bool FireSimulator::canSpreadTo(int x, int y) const
{
    if (!map_.isValidPosition(x, y))
        return false;

    TileState state = map_.getTileState(x, y);
    return state == TileState::Flammable;
}

std::vector<std::pair<size_t, size_t>> FireSimulator::getBurningCells() const
{
    std::vector<std::pair<size_t, size_t>> burning;

    for (size_t y = 0; y < Map::HEIGHT; ++y)
    {
        for (size_t x = 0; x < Map::WIDTH; ++x)
        {
            if (map_.getTileState(x, y) == TileState::Burning)
            {
                burning.emplace_back(x, y);
            }
        }
    }

    return burning;
}

void FireSimulator::igniteAt(size_t x, size_t y)
{
    if (map_.isValidPosition(x, y))
    {
        TileState state = map_.getTileState(x, y);
        if (state == TileState::Flammable)
        {
            map_.setTileState(x, y, TileState::Burning);
            std::cout << "Fire ignited at (" << x << ", " << y << ")" << std::endl;
        }
    }
}

// IMPORTANT: This is the ONLY way fire gets extinguished - by water spray!
void FireSimulator::extinguishAt(size_t x, size_t y)
{
    if (map_.isValidPosition(x, y))
    {
        if (map_.getTileState(x, y) == TileState::Burning)
        {
            // Change burning tile to burnt (extinguished)
            map_.setTileState(x, y, TileState::Burnt);
            map_.getTile(x, y).heatAccumulated = 0.0f;
            map_.getTile(x, y).burnTime = 0.0f;
            
            std::cout << "💧 Water extinguished fire at (" << x << ", " << y << ")" << std::endl;
        }
    }
}

size_t FireSimulator::getBurningCount() const
{
    return getBurningCells().size();
}

size_t FireSimulator::getBurntCount() const
{
    size_t count = 0;
    for (size_t y = 0; y < Map::HEIGHT; ++y)
    {
        for (size_t x = 0; x < Map::WIDTH; ++x)
        {
            if (map_.getTileState(x, y) == TileState::Burnt)
            {
                ++count;
            }
        }
    }
    return count;
}