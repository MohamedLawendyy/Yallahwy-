// File: src/PlayerController.hpp (UPDATED - Door Interaction)
#pragma once
#include "IUpdatable.hpp"
#include "IDrawable.hpp"
#include "AnimatedSprite.h"
#include "PhysicsEngine.hpp"
#include "Map.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector> // Required for std::vector

enum class Direction
{
    Up,
    Down,
    Left,
    Right
};

struct PlayerConfig
{
    float speed = 200.f;
    float bodyWidth = 32.f;
    float bodyHeight = 48.f;
    float spriteScale = 0.25f;
    unsigned animFrames = 8;
    float frameTime = 0.10f;
    
    // Water stream configuration
    float waterStreamLength = 30.f;  
    float waterStreamWidth = 40.f;    
};

class PlayerController : public IUpdatable, public IDrawable
{
public:
    // FIX: Map is now non-const reference so we can open doors
    PlayerController(PhysicsEngine &physics, Map &map, const PlayerConfig &config = PlayerConfig());
    ~PlayerController();

    void update(std::chrono::milliseconds deltaTime) override;
    void draw(sf::RenderWindow &window) override;

    void setPosition(sf::Vector2f worldPos);
    sf::Vector2f getPosition() const;
    Direction getDirection() const { return direction_; }

    bool loadAssets();
    bool isCollidingWithTile(size_t x, size_t y) const;
    void drawDebug(sf::RenderWindow &window) const;
    std::vector<sf::Vector2f> getCheckPoints() const;
    sf::FloatRect getBodyRect() const;

    void setFiring(bool firing) { isFiring_ = firing; }
    bool isFiring() const { return isFiring_; }
    
    sf::FloatRect getWaterCollisionRect() const;

private:
    void clampPositionToWorldBounds();
    PhysicsEngine &physics_;
    Map &map_; // FIX: Non-const reference
    PlayerConfig config_;

    b2Body *body_ = nullptr;

    std::unique_ptr<AnimatedSprite> idle_up_, idle_down_, idle_left_, idle_right_;
    std::unique_ptr<AnimatedSprite> run_up_, run_down_, run_left_, run_right_;
    std::unique_ptr<AnimatedSprite> water_up_, water_down_, water_left_, water_right_;

    Direction direction_ = Direction::Down;
    bool moving_ = false;
    bool isFiring_ = false;
    sf::Vector2f position_;

    // Door Tracking
    std::vector<sf::Vector2i> openDoors_; // Tracks currently open doors
    void updateDoors(); // Checks proximity to doors

    void createPhysicsBody(sf::Vector2f worldPos);
    void updatePhysicsBody(sf::Vector2f velocity);
    void syncPositionFromPhysics();

    AnimatedSprite *getCurrentAnimation();
    void updateAnimations(float dt);

    sf::Vector2f processInput();
    bool canMoveTo(sf::Vector2f newPos) const;
    sf::FloatRect calculateWaterBounds() const;
};