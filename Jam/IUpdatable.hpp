// File: src/IUpdatable.hpp
#pragma once
#include <chrono>

/**
 * Interface for components that need to be updated each frame
 * Follows Interface Segregation Principle - minimal, focused interface
 */
class IUpdatable {
public:
    virtual ~IUpdatable() = default;
    virtual void update(std::chrono::milliseconds deltaTime) = 0;
};