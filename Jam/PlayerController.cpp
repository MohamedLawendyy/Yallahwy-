// File: src/PlayerController.cpp
#include "PlayerController.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

PlayerController::PlayerController(PhysicsEngine &physics, Map &map, const PlayerConfig &config)
    : physics_(physics), map_(map), config_(config)
{
    // ... [Keep existing constructor initialization] ...
    idle_up_ = std::make_unique<AnimatedSprite>();
    idle_down_ = std::make_unique<AnimatedSprite>();
    idle_left_ = std::make_unique<AnimatedSprite>();
    idle_right_ = std::make_unique<AnimatedSprite>();
    run_up_ = std::make_unique<AnimatedSprite>();
    run_down_ = std::make_unique<AnimatedSprite>();
    run_left_ = std::make_unique<AnimatedSprite>();
    run_right_ = std::make_unique<AnimatedSprite>();

    water_up_ = std::make_unique<AnimatedSprite>();
    water_down_ = std::make_unique<AnimatedSprite>();
    water_left_ = std::make_unique<AnimatedSprite>();
    water_right_ = std::make_unique<AnimatedSprite>();
}

PlayerController::~PlayerController()
{
    if (body_)
    {
        physics_.getWorld().DestroyBody(body_);
        body_ = nullptr;
    }
}

bool PlayerController::loadAssets()
{
    // ... [Keep existing loadAssets implementation] ...
    if (!idle_up_->loadFromFile("IDLE/Up.png", config_.animFrames))
        return false;
    if (!idle_down_->loadFromFile("IDLE/Down.png", config_.animFrames))
        return false;
    if (!idle_left_->loadFromFile("IDLE/Left.png", config_.animFrames))
        return false;
    if (!idle_right_->loadFromFile("IDLE/Right.png", config_.animFrames))
        return false;
    if (!run_up_->loadFromFile("RUN/Up.png", config_.animFrames))
        return false;
    if (!run_down_->loadFromFile("RUN/Down.png", config_.animFrames))
        return false;
    if (!run_left_->loadFromFile("RUN/Left.png", config_.animFrames))
        return false;
    if (!run_right_->loadFromFile("RUN/Right.png", config_.animFrames))
        return false;

    if (!water_up_->loadFromFile("WATER/Up.png", 8))
        return false;
    if (!water_down_->loadFromFile("WATER/Down.png", 8))
        return false;
    if (!water_left_->loadFromFile("WATER/Left.png", 8))
        return false;
    if (!water_right_->loadFromFile("WATER/Right.png", 8))
        return false;

    float waterScale = 0.15f;
    for (auto *anim : {water_up_.get(), water_down_.get(), water_left_.get(), water_right_.get()})
    {
        anim->setSpriteScale(waterScale, waterScale);
        anim->setFrameTime(0.08f);
        anim->play(true);
    }

    float scale = config_.spriteScale;
    for (auto *anim : {idle_up_.get(), idle_down_.get(), idle_left_.get(), idle_right_.get(),
                       run_up_.get(), run_down_.get(), run_left_.get(), run_right_.get()})
    {
        anim->setSpriteScale(scale, scale);
        anim->setFrameTime(config_.frameTime);
        anim->play(true);
    }
    return true;
}

void PlayerController::createPhysicsBody(sf::Vector2f worldPos)
{
    // ... [Keep existing physics creation] ...
    const float SCALE = 30.0f;
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = b2Vec2(worldPos.x / SCALE, worldPos.y / SCALE);
    bodyDef.fixedRotation = true;
    bodyDef.linearDamping = 10.0f;
    body_ = physics_.getWorld().CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox((config_.bodyWidth / 2.0f) / SCALE, (config_.bodyHeight / 2.0f) / SCALE);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body_->CreateFixture(&fixtureDef);
}

void PlayerController::setPosition(sf::Vector2f worldPos)
{
    position_ = worldPos;
    if (!body_)
        createPhysicsBody(worldPos);
    else
    {
        const float SCALE = 30.0f;
        body_->SetTransform(b2Vec2(worldPos.x / SCALE, worldPos.y / SCALE), 0);
    }
}

sf::Vector2f PlayerController::getPosition() const { return position_; }

sf::Vector2f PlayerController::processInput()
{
    sf::Vector2f move(0, 0);
    moving_ = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        move.y -= 1;
        direction_ = Direction::Up;
        moving_ = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        move.y += 1;
        direction_ = Direction::Down;
        moving_ = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        move.x -= 1;
        direction_ = Direction::Left;
        moving_ = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        move.x += 1;
        direction_ = Direction::Right;
        moving_ = true;
    }

    if (moving_ && move.x != 0 && move.y != 0)
        move /= std::sqrt(2.f);
    return move;
}

sf::FloatRect PlayerController::getBodyRect() const
{
    return sf::FloatRect(
        position_.x - config_.bodyWidth / 2.0f,
        position_.y - config_.bodyHeight / 2.0f,
        config_.bodyWidth, config_.bodyHeight);
}

sf::FloatRect PlayerController::calculateWaterBounds() const
{
    sf::FloatRect waterRect;
    switch (direction_)
    {
    case Direction::Up:
        waterRect = sf::FloatRect(position_.x - config_.waterStreamWidth / 2.f, position_.y - config_.waterStreamLength, config_.waterStreamWidth, config_.waterStreamLength);
        break;
    case Direction::Down:
        waterRect = sf::FloatRect(position_.x - config_.waterStreamWidth / 2.f, position_.y, config_.waterStreamWidth, config_.waterStreamLength);
        break;
    case Direction::Left:
        waterRect = sf::FloatRect(position_.x - config_.waterStreamLength, position_.y - config_.waterStreamWidth / 2.f, config_.waterStreamLength, config_.waterStreamWidth);
        break;
    case Direction::Right:
        waterRect = sf::FloatRect(position_.x, position_.y - config_.waterStreamWidth / 2.f, config_.waterStreamLength, config_.waterStreamWidth);
        break;
    }
    return waterRect;
}

sf::FloatRect PlayerController::getWaterCollisionRect() const
{
    return isFiring_ ? calculateWaterBounds() : sf::FloatRect(0, 0, 0, 0);
}

bool PlayerController::canMoveTo(sf::Vector2f newPos) const
{
    const float COLLISION_MARGIN = 0.5f;
    sf::FloatRect playerRect(
        newPos.x - config_.bodyWidth / 2.0f + COLLISION_MARGIN,
        newPos.y - config_.bodyHeight / 2.0f + COLLISION_MARGIN,
        config_.bodyWidth - COLLISION_MARGIN * 2.0f,
        config_.bodyHeight - COLLISION_MARGIN * 2.0f);

    int minX = static_cast<int>(playerRect.left / Map::TILE_SIZE);
    int maxX = static_cast<int>((playerRect.left + playerRect.width) / Map::TILE_SIZE);
    int minY = static_cast<int>(playerRect.top / Map::TILE_SIZE);
    int maxY = static_cast<int>((playerRect.top + playerRect.height) / Map::TILE_SIZE);

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            if (!map_.isValidPosition(x, y))
                return false;

            TileState state = map_.getTileState(x, y);

            if (state == TileState::Wall || state == TileState::Prop)
            {
                sf::FloatRect tileRect = map_.getTileCollisionRect(x, y);
                if (playerRect.intersects(tileRect))
                    return false;
            }
        }
    }
    return true;
}

// ========================================================
// FIXED: SYNCHRONIZED DOOR LOGIC
// ========================================================
void PlayerController::updateDoors()
{
    sf::Vector2f center = getPosition();
    int px = static_cast<int>(center.x / Map::TILE_SIZE);
    int py = static_cast<int>(center.y / Map::TILE_SIZE);

    auto trackDoor = [&](int x, int y)
    {
        for (const auto &d : openDoors_)
        {
            if (d.x == x && d.y == y)
                return;
        }
        openDoors_.push_back({x, y});
    };

    // 1. OPEN DOORS (Check 3x3 area)
    for (int y = py - 1; y <= py + 1; ++y)
    {
        for (int x = px - 1; x <= px + 1; ++x)
        {
            if (!map_.isValidPosition(x, y))
                continue;

            Tile &tile = map_.getTile(x, y);

            // --- HANDLE TOP PART (Closed: 16) ---
            if (tile.overlayTileId == 16)
            {
                tile.overlayTileId = 18; // Open Self
                trackDoor(x, y);

                // Open Bottom Partner
                if (map_.isValidPosition(x, y + 1))
                {
                    Tile &bottom = map_.getTile(x, y + 1);
                    if (bottom.overlayTileId == 17)
                    {
                        bottom.overlayTileId = 19;
                        trackDoor(x, y + 1);
                    }
                }
            }
            // --- HANDLE BOTTOM PART (Closed: 17) ---
            else if (tile.overlayTileId == 17)
            {
                tile.overlayTileId = 19; // Open Self
                trackDoor(x, y);

                // Open Top Partner
                if (map_.isValidPosition(x, y - 1))
                {
                    Tile &top = map_.getTile(x, y - 1);
                    if (top.overlayTileId == 16)
                    {
                        top.overlayTileId = 18;
                        trackDoor(x, y - 1);
                    }
                }
            }
        }
    }

    // 2. CLOSE DOORS (Sync Logic)
    for (int i = openDoors_.size() - 1; i >= 0; --i)
    {
        sf::Vector2i doorPos = openDoors_[i];

        // Safety check if map changed
        if (!map_.isValidPosition(doorPos.x, doorPos.y))
        {
            openDoors_.erase(openDoors_.begin() + i);
            continue;
        }

        Tile &tile = map_.getTile(doorPos.x, doorPos.y);

        // Determine Partner Position
        sf::Vector2i partnerPos = doorPos;
        bool hasPartner = false;

        if (tile.overlayTileId == 18)
        { // Top Open
            partnerPos = {doorPos.x, doorPos.y + 1};
            hasPartner = true;
        }
        else if (tile.overlayTileId == 19)
        { // Bottom Open
            partnerPos = {doorPos.x, doorPos.y - 1};
            hasPartner = true;
        }
        else
        {
            // Not an open door anymore? Remove tracking.
            openDoors_.erase(openDoors_.begin() + i);
            continue;
        }

        // --- PARTNER CHECK LOGIC ---
        // Player distance to THIS tile
        int dx1 = std::abs(doorPos.x - px);
        int dy1 = std::abs(doorPos.y - py);

        // Player distance to PARTNER tile
        int dx2 = 100, dy2 = 100;
        if (hasPartner && map_.isValidPosition(partnerPos.x, partnerPos.y))
        {
            dx2 = std::abs(partnerPos.x - px);
            dy2 = std::abs(partnerPos.y - py);
        }

        // Logic: Keep open if player is close to ME OR close to PARTNER
        // Using "1" as the close distance (adjacent tile)
        bool nearMe = (dx1 <= 1 && dy1 <= 1);
        bool nearPartner = (dx2 <= 1 && dy2 <= 1);

        if (!nearMe && !nearPartner)
        {
            // Player is far from BOTH parts of the door. Safe to close.
            if (tile.overlayTileId == 18)
                tile.overlayTileId = 16; // Top Close
            else if (tile.overlayTileId == 19)
                tile.overlayTileId = 17; // Bottom Close

            openDoors_.erase(openDoors_.begin() + i);
        }
    }
}
void PlayerController::updatePhysicsBody(sf::Vector2f velocity)
{
    if (!body_)
        return;
    const float SCALE = 30.0f;
    body_->SetLinearVelocity(b2Vec2(velocity.x / SCALE, velocity.y / SCALE));
}

void PlayerController::syncPositionFromPhysics()
{
    if (!body_)
        return;
    const float SCALE = 30.0f;
    b2Vec2 b2Pos = body_->GetPosition();
    sf::Vector2f targetPos(b2Pos.x * SCALE, b2Pos.y * SCALE);

    if (canMoveTo(targetPos))
    {
        position_ = targetPos;
    }
    else
    {
        if (canMoveTo(sf::Vector2f(targetPos.x, position_.y)))
            position_.x = targetPos.x;
        else if (canMoveTo(sf::Vector2f(position_.x, targetPos.y)))
            position_.y = targetPos.y;

        body_->SetTransform(b2Vec2(position_.x / SCALE, position_.y / SCALE), 0);
    }
}

void PlayerController::clampPositionToWorldBounds()
{
    float worldW = Map::WIDTH * Map::TILE_SIZE;
    float worldH = Map::HEIGHT * Map::TILE_SIZE;
    float hw = config_.bodyWidth / 2.f, hh = config_.bodyHeight / 2.f;

    position_.x = std::max(hw, std::min(position_.x, worldW - hw));
    position_.y = std::max(hh, std::min(position_.y, worldH - hh));

    if (body_)
    {
        const float SCALE = 30.0f;
        body_->SetTransform(b2Vec2(position_.x / SCALE, position_.y / SCALE), 0);
    }
}

void PlayerController::update(std::chrono::milliseconds deltaTime)
{
    float dt = deltaTime.count() / 1000.0f;

    sf::Vector2f moveDir = processInput();
    sf::Vector2f velocity = moveDir * config_.speed;

    updatePhysicsBody(velocity);
    syncPositionFromPhysics();
    clampPositionToWorldBounds();
    updateAnimations(dt);

    updateDoors();
}

void PlayerController::updateAnimations(float dt)
{
    idle_up_->update(dt);
    idle_down_->update(dt);
    idle_left_->update(dt);
    idle_right_->update(dt);
    run_up_->update(dt);
    run_down_->update(dt);
    run_left_->update(dt);
    run_right_->update(dt);
    if (isFiring_)
    {
        water_up_->update(dt);
        water_down_->update(dt);
        water_left_->update(dt);
        water_right_->update(dt);
    }
}

AnimatedSprite *PlayerController::getCurrentAnimation()
{
    if (!moving_)
    {
        switch (direction_)
        {
        case Direction::Up:
            return idle_up_.get();
        case Direction::Down:
            return idle_down_.get();
        case Direction::Left:
            return idle_left_.get();
        case Direction::Right:
            return idle_right_.get();
        }
    }
    else
    {
        switch (direction_)
        {
        case Direction::Up:
            return run_up_.get();
        case Direction::Down:
            return run_down_.get();
        case Direction::Left:
            return run_left_.get();
        case Direction::Right:
            return run_right_.get();
        }
    }
    return idle_down_.get();
}

void PlayerController::draw(sf::RenderWindow &window)
{
    AnimatedSprite *currentAnim = getCurrentAnimation();
    currentAnim->setPosition(position_);
    window.draw(*currentAnim);



    if (isFiring_)
    {
        AnimatedSprite *waterAnim = nullptr;
        sf::Vector2f offset(0, 0);
        switch (direction_)
        {
        case Direction::Up:
            waterAnim = water_up_.get();
            break;
        case Direction::Down:
            waterAnim = water_down_.get();
            break;
        case Direction::Left:
            waterAnim = water_left_.get();
            offset = sf::Vector2f(-30, 0);
            break;
        case Direction::Right:
            waterAnim = water_right_.get();
            offset = sf::Vector2f(30, 0);
            break;
        }
        if (waterAnim)
        {
            waterAnim->setPosition(position_ + offset);
            window.draw(*waterAnim);
        }
    }
}

bool PlayerController::isCollidingWithTile(size_t x, size_t y) const
{
    return getBodyRect().intersects(map_.getTileCollisionRect(x, y));
}

void PlayerController::drawDebug(sf::RenderWindow &window) const
{
    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(config_.bodyWidth, config_.bodyHeight));
    rect.setOrigin(config_.bodyWidth / 2.0f, config_.bodyHeight / 2.0f);
    rect.setPosition(position_);
    rect.setFillColor(sf::Color(255, 0, 0, 100));
    rect.setOutlineColor(sf::Color::Red);
    rect.setOutlineThickness(2.0f);
    window.draw(rect);

    if (isFiring_)
    {
        sf::FloatRect waterBounds = getWaterCollisionRect();
        sf::RectangleShape waterRect;
        waterRect.setPosition(waterBounds.left, waterBounds.top);
        waterRect.setSize(sf::Vector2f(waterBounds.width, waterBounds.height));
        waterRect.setFillColor(sf::Color(0, 100, 255, 100));
        waterRect.setOutlineColor(sf::Color::Cyan);
        waterRect.setOutlineThickness(2.0f);
        window.draw(waterRect);
    }
}

std::vector<sf::Vector2f> PlayerController::getCheckPoints() const
{
    sf::FloatRect bodyRect = getBodyRect();
    float margin = 2.0f;
    sf::FloatRect paddedRect = bodyRect;
    paddedRect.left += margin;
    paddedRect.top += margin;
    paddedRect.width -= margin * 2.f;
    paddedRect.height -= margin * 2.f;

    std::vector<sf::Vector2f> points;
    points.push_back(sf::Vector2f(paddedRect.left + paddedRect.width / 2.f, paddedRect.top + paddedRect.height / 2.f));
    points.push_back(sf::Vector2f(paddedRect.left, paddedRect.top));
    points.push_back(sf::Vector2f(paddedRect.left + paddedRect.width, paddedRect.top));
    points.push_back(sf::Vector2f(paddedRect.left, paddedRect.top + paddedRect.height));
    points.push_back(sf::Vector2f(paddedRect.left + paddedRect.width, paddedRect.top + paddedRect.height));
    return points;
}