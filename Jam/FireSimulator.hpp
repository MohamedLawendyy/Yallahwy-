// File: src/FireSimulator.hpp
#pragma once
#include "IUpdatable.hpp"
#include "Map.hpp"
#include <memory>
#include "FireSpreadLogic.hpp" // Add this include
#include <chrono>
#include <vector>

struct FireSpreadConfig
{
    float burnDuration = 4.0f;         // Base duration - how long a tile burns
    float heatTransferRate = 1.2f;     // Base heat transfer per second to neighbors
    float spreadCheckInterval = 0.1f;  // How often to check for spreading
    bool allowDiagonalSpread = false;  // Spread diagonally or only orthogonal
    bool autoSpreadEnabled = true;     // If true, fire spreads automatically
    float heatDissipationRate = 0.98f; // Heat decay multiplier per check

    // NEW: Progressive speed control
    float earlyBurnSpeedMultiplier = 0.4f; // Burn speed in first phase (0.0-1.0, lower = slower)
    float lateBurnSpeedMultiplier = 2.0f;  // Burn speed in final phase (>1.0 = faster)
    float burnAccelerationPoint = 0.6f;    // When to start speeding up (0.0-1.0, 0.6 = 60% through)

    float earlySpreadSpeedMultiplier = 0.5f; // Spread speed at start (lower = slower)
    float lateSpreadSpeedMultiplier = 1.8f;  // Spread speed at end (higher = faster)
    float spreadAccelerationPoint = 0.5f;    // When spread starts accelerating
};

class FireSimulator : public IUpdatable
{
public:
    explicit FireSimulator(Map &map, const FireSpreadConfig &config = FireSpreadConfig());

    void update(std::chrono::milliseconds deltaTime) override;

    // Configuration
    void setConfig(const FireSpreadConfig &config) { config_ = config; }
    const FireSpreadConfig &getConfig() const { return config_; }

    // Ignition control
    void igniteAt(size_t x, size_t y);
    void extinguishAt(size_t x, size_t y);

    // Statistics
    size_t getBurningCount() const;
    size_t getBurntCount() const;

private:
    Map &map_;
    FireSpreadConfig config_;
    float spreadTimer_ = 0.0f;

    // Core fire logic
    void updateBurningTiles(float deltaTime);
    void processFireSpread();
    void transferHeatToNeighbors(size_t x, size_t y, float heatAmount);
    void checkIgnition(size_t x, size_t y);

    // Speed calculation helpers
    float calculateBurnSpeedMultiplier(float burnProgress) const;
    float calculateSpreadSpeedMultiplier(float burnProgress) const;

    // Neighbor queries
    std::vector<std::pair<int, int>> getNeighbors(int x, int y) const;
    bool canSpreadTo(int x, int y) const;

    // Helper
    std::vector<std::pair<size_t, size_t>> getBurningCells() const;
    std::unique_ptr<FireSpreadLogic> fireSpreadLogic_; // Add this
};