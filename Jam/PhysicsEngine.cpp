// File: src/PhysicsEngine.cpp
#include "PhysicsEngine.hpp"
#include <memory>
PhysicsEngine::PhysicsEngine(float worldWidth, float worldHeight)
    : world_(std::make_unique<b2World>(b2Vec2(0.0f, 0.0f)))
{ // Gravity off for top-down
    // Optional: Add ground body or wind simulation here
}

PhysicsEngine::~PhysicsEngine() = default;

void PhysicsEngine::update(std::chrono::milliseconds deltaTime)
{
    float dt = deltaTime.count() / 1000.0f;
    accumulator_ += dt;
    while (accumulator_ >= TIME_STEP)
    {
        world_->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
        accumulator_ -= TIME_STEP;
    }
    // Could apply wind: world_->SetGravity(b2Vec2(windForce, 0.0f));
}