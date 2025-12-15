// File: src/PhysicsEngine.hpp (Wraps Box2D for physics; minimal for now, e.g., global wind force)
#pragma once
#include "IUpdatable.hpp"
#include <Box2D/Box2D.h>
#include <memory>
#include <chrono>

class PhysicsEngine : public IUpdatable
{
public:
    PhysicsEngine(float worldWidth, float worldHeight);
    ~PhysicsEngine();
    b2World &getWorld() { return *world_; }
    void update(std::chrono::milliseconds deltaTime) override;

private:
    std::unique_ptr<b2World> world_;
    static constexpr float TIME_STEP = 1.0f / 60.0f;
    static constexpr int VELOCITY_ITERATIONS = 6;
    static constexpr int POSITION_ITERATIONS = 2;
    float accumulator_ = 0.0f;
    
};