// File: src/Game.cpp (UPDATED - Fix Audio Overlap)
#include "Game.hpp"
#include <random>
#include <iostream>
#include <algorithm>

// Easing function for smooth animation
float easeInOutQuad(float t)
{
    return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
}

Game::Game() : currentState_(GameState::MainMenu)
{
    // ... [Keep constructor logic] ...
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    unsigned int screenWidth = desktop.width;
    unsigned int screenHeight = desktop.height;

    Map::initializeDimensions(screenWidth, screenHeight);

    mapConfig_.flammableDensity = 0.65f;

    fireConfig_.burnDuration = 100.0f;
    fireConfig_.heatTransferRate = 0.3f;
    fireConfig_.spreadCheckInterval = 0.2f;
    fireConfig_.allowDiagonalSpread = false;
    fireConfig_.autoSpreadEnabled = true;

    playerConfig_.speed = 150.f;
    playerConfig_.bodyWidth = 32.f;
    playerConfig_.bodyHeight = 48.f;
    playerConfig_.spriteScale = 0.15f;
    playerConfig_.waterStreamLength = 50.f;

    window_.create(desktop, "Fire Spread Simulation", sf::Style::Fullscreen);
    window_.setVerticalSyncEnabled(true);
    window_.setFramerateLimit(60);

    transitionOverlay_.setSize(sf::Vector2f(10000.f, 10000.f));
    transitionOverlay_.setOrigin(5000.f, 5000.f);
    transitionOverlay_.setFillColor(sf::Color(0, 0, 0, 0));

    if (bgMusic_.openFromFile("sounds/Main Menu.ogg"))
    {
        bgMusic_.setLoop(true);
        bgMusic_.setVolume(50.f);
        bgMusic_.play();
    }
    else
    {
        std::cerr << "Warning: Could not load background music." << std::endl;
    }

    if (!initializeSystems())
    {
        throw std::runtime_error("Failed to initialize game systems");
    }
}

bool Game::initializeSystems() { return initializeMenu(); }

bool Game::initializeMenu()
{
    mainMenu_ = std::make_unique<MainMenu>(window_, bgMusic_);
    return mainMenu_->loadAssets();
}

void Game::initLevel()
{
    std::cout << "ðŸ —ï¸   Initializing Level..." << std::endl;

    // ========================================================
    // FIX: Resume music if it was paused by Game Over screen
    // ========================================================
    if (bgMusic_.getStatus() != sf::SoundSource::Playing)
    {
        bgMusic_.play();
    }

    waterLevel_ = 0.0f;
    fireIntensity_ = 0.0f;
    hasShownWinScreen_ = false;
    hasShownLoseScreen_ = false;
    paused_ = false;
    if (gameOverScreen_)
        gameOverScreen_->reset();

    updatables_.clear();
    drawables_.clear();
    shootingSystem_.reset();
    playerController_.reset();
    fireSimulator_.reset();
    physicsEngine_.reset();

    physicsEngine_ = std::make_unique<PhysicsEngine>(Map::WIDTH * Map::TILE_SIZE, Map::HEIGHT * Map::TILE_SIZE);
    map_ = Map(mapConfig_, &physicsEngine_->getWorld());
    fireSimulator_ = std::make_unique<FireSimulator>(map_, fireConfig_);

    initialFlammableCount_ = 0;
    for (size_t y = 0; y < Map::HEIGHT; ++y)
    {
        for (size_t x = 0; x < Map::WIDTH; ++x)
        {
            if (map_.getTileState(x, y) == TileState::Flammable)
                initialFlammableCount_++;
        }
    }
    if (initialFlammableCount_ == 0)
        initialFlammableCount_ = 1;

    playerController_ = std::make_unique<PlayerController>(*physicsEngine_, map_, playerConfig_);
    playerController_->loadAssets();

    if (!bulletSystem_)
        bulletSystem_ = std::make_unique<BulletSystem>();
    shootingSystem_ = std::make_unique<ShootingSystem>(*playerController_, *bulletSystem_);

    if (!renderer_)
        renderer_ = std::make_unique<Renderer>(window_, map_);
    if (!gameUI_)
    {
        gameUI_ = std::make_unique<GameUI>(window_);
        gameUI_->loadAssets();
    }
    if (!gameOverScreen_)
    {
        gameOverScreen_ = std::make_unique<GameOverScreen>(window_);
        gameOverScreen_->loadAssets();
    }

    playerController_->setPosition(map_.getSafeSpawnPoint());

    updatables_.push_back(physicsEngine_.get());
    updatables_.push_back(fireSimulator_.get());
    updatables_.push_back(playerController_.get());
    drawables_.push_back(playerController_.get());

    cameraMode_ = CameraMode::Overview;
    cameraTimer_ = 0.0f;

    setupRender();
    gameView_ = window_.getView();

    auto furnitureTiles = map_.getAllFurnitureTiles();
    if (!furnitureTiles.empty())
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, furnitureTiles.size() - 1);
        auto [x, y] = furnitureTiles[dist(gen)];
        fireSimulator_->igniteAt(x, y);
    }
}

// ... [Keep updateCamera, startTransition, updateTransition, run, etc.] ...
void Game::updateCamera(float dt)
{
    float mapWidth = Map::WIDTH * Map::TILE_SIZE;
    float mapHeight = Map::HEIGHT * Map::TILE_SIZE;

    float aspect = gameView_.getSize().x / gameView_.getSize().y;
    sf::Vector2f targetSize(600.0f * aspect, 600.0f);
    sf::Vector2f fullMapSize(mapWidth, mapHeight);

    if (targetSize.x > mapWidth)
        targetSize.x = mapWidth;
    if (targetSize.y > mapHeight)
        targetSize.y = mapHeight;

    sf::Vector2f playerPos = playerController_->getPosition();
    float halfTargetW = targetSize.x / 2.0f;
    float halfTargetH = targetSize.y / 2.0f;
    sf::Vector2f targetCenter = playerPos;

    targetCenter.x = std::max(halfTargetW, std::min(targetCenter.x, mapWidth - halfTargetW));
    targetCenter.y = std::max(halfTargetH, std::min(targetCenter.y, mapHeight - halfTargetH));

    if (cameraMode_ == CameraMode::Overview)
    {
        cameraTimer_ += dt;
        if (cameraTimer_ >= 1.5f)
        {
            cameraTimer_ = 0.0f;
            cameraMode_ = CameraMode::Zooming;
        }
        setupRender();
        gameView_ = window_.getView();
    }
    else if (cameraMode_ == CameraMode::Zooming)
    {
        cameraTimer_ += dt;
        float t = std::min(1.0f, cameraTimer_ / 2.0f);
        float easedT = easeInOutQuad(t);

        sf::Vector2f startSize = fullMapSize;
        sf::Vector2f currentSize = startSize + (targetSize - startSize) * easedT;
        gameView_.setSize(currentSize);

        sf::Vector2f startCenter(mapWidth / 2.0f, mapHeight / 2.0f);
        sf::Vector2f currentCenter = startCenter + (targetCenter - startCenter) * easedT;
        gameView_.setCenter(currentCenter);

        if (t >= 1.0f)
            cameraMode_ = CameraMode::Locked;
    }
    else if (cameraMode_ == CameraMode::Locked)
    {
        gameView_.setSize(targetSize);
        gameView_.setCenter(targetCenter);
    }
}

void Game::startTransition(bool isReset)
{
    if (isTransitioning_)
        return;
    isTransitioning_ = true;
    isFadingOut_ = true;
    transitionAlpha_ = 0.0f;
    pendingReset_ = isReset;
}

void Game::updateTransition(float dt)
{
    const float FADE_SPEED = 500.0f;
    if (isFadingOut_)
    {
        transitionAlpha_ += FADE_SPEED * dt;
        if (transitionAlpha_ >= 255.0f)
        {
            transitionAlpha_ = 255.0f;
            isFadingOut_ = false;
            if (pendingReset_)
                initLevel();
            else
            {
                initLevel();
                currentState_ = GameState::Playing;
                if (mainMenu_)
                    mainMenu_->reset();
            }
        }
    }
    else
    {
        transitionAlpha_ -= FADE_SPEED * dt;
        if (transitionAlpha_ <= 0.0f)
        {
            transitionAlpha_ = 0.0f;
            isTransitioning_ = false;
        }
    }
    transitionOverlay_.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(transitionAlpha_)));
}

void Game::run()
{
    sf::Clock clock;
    while (running_)
    {
        sf::Event event;
        while (window_.pollEvent(event))
            processEvents(event);
        sf::Time sfmlDt = clock.restart();
        float dt = sfmlDt.asSeconds();
        if (isTransitioning_)
        {
            updateTransition(dt);
            if (transitionAlpha_ > 240.0f)
            {
                render();
                continue;
            }
        }
        if (currentState_ == GameState::MainMenu)
            updateMenu(dt);
        else if (currentState_ == GameState::Playing)
        {
            std::chrono::milliseconds deltaTime(static_cast<long long>(sfmlDt.asMilliseconds()));
            updateGame(deltaTime);
        }
        render();
    }
}

void Game::processEvents(sf::Event &event)
{
    if (event.type == sf::Event::Closed)
    {
        running_ = false;
        return;
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11)
    {
        toggleFullscreen();
        return;
    }
    if (currentState_ == GameState::MainMenu)
        processMenuEvents(event);
    else if (currentState_ == GameState::Playing)
        processGameEvents(event);
}

void Game::processMenuEvents(sf::Event &event)
{
    if (isTransitioning_)
        return;
    mainMenu_->handleEvent(event);
    if (mainMenu_->shouldStartGame())
        startTransition(false);
    if (mainMenu_->shouldExit())
        running_ = false;
}

void Game::processGameEvents(sf::Event &event)
{
    if (isTransitioning_)
        return;
    if (gameOverScreen_)
    {
        gameOverScreen_->handleEvent(event);
        if (gameOverScreen_->shouldRestart())
        {
            startTransition(true);
            return;
        }
        if (gameOverScreen_->shouldReturnToMenu())
        {
            returnToMenu();
            return;
        }
    }
    if (event.type == sf::Event::Resized)
        setupRender();
    if (event.type == sf::Event::KeyPressed)
        handleKeyPress(event.key.code);
    if (event.type == sf::Event::MouseButtonPressed)
        handleMouseClick(event.mouseButton.button, event.mouseButton.x, event.mouseButton.y);
}

void Game::updateMenu(float deltaTime) { mainMenu_->update(deltaTime); }

void Game::updateGame(std::chrono::milliseconds deltaTime)
{
    float dt = deltaTime.count() / 1000.0f;
    if (paused_)
        return;

    shootingSystem_->handleInput(waterLevel_, dt);

    for (auto *updatable : updatables_)
        updatable->update(deltaTime);

    bulletSystem_->checkCollisions(map_, *fireSimulator_);

    if (playerController_->isFiring() && waterLevel_ > 0.0f)
    {
        checkWaterFireCollision();
    }

    checkWaterRefill(dt);
    updateCamera(dt);
    updateUIMetrics();
    checkWinLoseConditions();

    if (gameOverScreen_)
        gameOverScreen_->update(dt);
}

void Game::checkWaterRefill(float dt)
{
    if (waterLevel_ >= 1.0f)
        return;
    sf::Vector2f center = playerController_->getPosition();
    int px = static_cast<int>(center.x / Map::TILE_SIZE);
    int py = static_cast<int>(center.y / Map::TILE_SIZE);
    bool nearWaterSource = false;
    for (int y = py - 1; y <= py + 1; ++y)
    {
        for (int x = px - 1; x <= px + 1; ++x)
        {
            if (map_.isValidPosition(x, y))
            {
                int id = map_.getTile(x, y).overlayTileId;
                if (id == 38 || id == 39)
                {
                    nearWaterSource = true;
                    break;
                }
            }
        }
        if (nearWaterSource)
            break;
    }
    if (nearWaterSource)
    {
        waterLevel_ += 0.5f * dt;
        if (waterLevel_ > 1.0f)
            waterLevel_ = 1.0f;
    }
}

void Game::updateUIMetrics()
{
    size_t burningCount = fireSimulator_->getBurningCount();
    size_t burntCount = fireSimulator_->getBurntCount();
    if (initialFlammableCount_ > 0)
        fireIntensity_ = (float)(burningCount + burntCount) / (float)initialFlammableCount_;
    else
        fireIntensity_ = 0.0f;
    fireIntensity_ = std::min(1.0f, fireIntensity_);
    if (gameUI_)
    {
        gameUI_->setWaterLevel(waterLevel_);
        gameUI_->setFireLevel(fireIntensity_);
    }
}

void Game::checkWaterFireCollision()
{
    if (!playerController_->isFiring())
        return;
    sf::FloatRect waterRect = playerController_->getWaterCollisionRect();
    int minX = std::max(0, static_cast<int>(waterRect.left / Map::TILE_SIZE));
    int maxX = std::min(static_cast<int>(Map::WIDTH) - 1, static_cast<int>((waterRect.left + waterRect.width) / Map::TILE_SIZE));
    int minY = std::max(0, static_cast<int>(waterRect.top / Map::TILE_SIZE));
    int maxY = std::min(static_cast<int>(Map::HEIGHT) - 1, static_cast<int>((waterRect.top + waterRect.height) / Map::TILE_SIZE));

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            if (map_.getTileState(x, y) == TileState::Burning)
            {
                sf::FloatRect tileRect = map_.getTileCollisionRect(x, y);
                if (waterRect.intersects(tileRect))
                    fireSimulator_->extinguishAt(x, y);
            }
        }
    }
}

// ========================================================
// FIX: Pause BG Music on Win/Lose
// ========================================================
void Game::checkWinLoseConditions()
{
    if (hasShownWinScreen_ || hasShownLoseScreen_ || isTransitioning_)
        return;

    if (fireSimulator_->getBurningCount() == 0 && fireSimulator_->getBurntCount() > 0)
    {
        hasShownWinScreen_ = true;
        bgMusic_.pause(); // Pause background music for Win sound
        gameOverScreen_->show(GameResult::Win);
    }
    else if (fireIntensity_ >= 0.65f)
    {
        hasShownLoseScreen_ = true;
        bgMusic_.pause(); // Pause background music for Lose sound
        gameOverScreen_->show(GameResult::Lose);
    }
}

void Game::render()
{
    window_.clear(sf::Color::Black);
    if (currentState_ == GameState::MainMenu)
        renderMenu();
    else if (currentState_ == GameState::Playing)
        renderGame();
    window_.display();
}

void Game::renderMenu()
{
    window_.setView(window_.getDefaultView());
    mainMenu_->render();
    if (isTransitioning_)
    {
        transitionOverlay_.setPosition(window_.getSize().x / 2.f, window_.getSize().y / 2.f);
        window_.draw(transitionOverlay_);
    }
}

void Game::renderGame()
{
    window_.setView(gameView_);
    renderer_->render();
    if (showDebug_)
    {
        drawDebugColliders();
        playerController_->drawDebug(window_);
    }
    for (auto *drawable : drawables_)
        drawable->draw(window_);

    window_.setView(window_.getDefaultView());
    if (gameUI_)
        gameUI_->render();
    if (gameOverScreen_)
        gameOverScreen_->render();
    if (isTransitioning_)
    {
        transitionOverlay_.setPosition(window_.getSize().x / 2.f, window_.getSize().y / 2.f);
        window_.draw(transitionOverlay_);
    }
}

void Game::handleKeyPress(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Escape)
        returnToMenu();
    if (key == sf::Keyboard::R)
        startTransition(true);
    if (key == sf::Keyboard::P)
        paused_ = !paused_;
    if (key == sf::Keyboard::M)
        showDebug_ = !showDebug_;
}

void Game::returnToMenu()
{
    currentState_ = GameState::MainMenu;

    // ========================================================
    // FIX: Resume BG Music when returning to menu
    // ========================================================
    if (bgMusic_.getStatus() != sf::SoundSource::Playing)
    {
        bgMusic_.play();
    }

    updatables_.clear();
    drawables_.clear();
    fireSimulator_.reset();
    shootingSystem_.reset();
    playerController_.reset();
    physicsEngine_.reset();
    bulletSystem_.reset();
}

void Game::toggleFullscreen()
{
    isFullscreen_ = !isFullscreen_;
    sf::VideoMode mode = isFullscreen_ ? sf::VideoMode::getDesktopMode() : sf::VideoMode(1280, 720);
    sf::Uint32 style = isFullscreen_ ? sf::Style::Fullscreen : sf::Style::Default;
    window_.create(mode, "Fire Spread Simulation", style);
    Map::initializeDimensions(mode.width, mode.height);
    window_.setVerticalSyncEnabled(true);
    window_.setFramerateLimit(60);
    if (currentState_ == GameState::Playing)
        startTransition(true);
    else
        initializeMenu();
}

sf::View Game::getLetterboxView(sf::FloatRect gameBounds, int windowWidth, int windowHeight) const
{
    float gameAspect = gameBounds.width / gameBounds.height;
    float windowAspect = windowWidth / (float)windowHeight;
    float sizeX = 1.0f, sizeY = 1.0f, posX = 0.0f, posY = 0.0f;
    if (gameAspect > windowAspect)
    {
        sizeX = windowAspect / gameAspect;
        posX = (1.0f - sizeX) / 2.0f;
    }
    else
    {
        sizeY = gameAspect / windowAspect;
        posY = (1.0f - sizeY) / 2.0f;
    }
    sf::View view;
    view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));
    view.setSize(gameBounds.width, gameBounds.height);
    view.setCenter(gameBounds.width / 2.0f, gameBounds.height / 2.0f);
    return view;
}

std::pair<size_t, size_t> Game::screenToGrid(int screenX, int screenY) const
{
    sf::Vector2u windowSize = window_.getSize();
    sf::Vector2f worldPos = window_.mapPixelToCoords(sf::Vector2i(screenX, screenY), gameView_);
    return {static_cast<size_t>(worldPos.x / Map::TILE_SIZE), static_cast<size_t>(worldPos.y / Map::TILE_SIZE)};
}

void Game::drawDebugColliders()
{
    sf::RectangleShape debugRect(sf::Vector2f(Map::TILE_SIZE, Map::TILE_SIZE));
    debugRect.setFillColor(sf::Color::Transparent);
    debugRect.setOutlineThickness(2.0f);
    for (size_t y = 0; y < Map::HEIGHT; ++y)
    {
        for (size_t x = 0; x < Map::WIDTH; ++x)
        {
            TileState state = map_.getTileState(x, y);
            if (state == TileState::Wall || state == TileState::Prop)
            {
                debugRect.setPosition(x * Map::TILE_SIZE, y * Map::TILE_SIZE);
                debugRect.setOutlineColor(state == TileState::Wall ? sf::Color::Red : sf::Color::Blue);
                window_.draw(debugRect);
            }
        }
    }
}

void Game::setupRender()
{
    sf::Vector2u wSize = window_.getSize();
    window_.setView(getLetterboxView(sf::FloatRect(0, 0, Map::WIDTH * Map::TILE_SIZE, Map::HEIGHT * Map::TILE_SIZE), wSize.x, wSize.y));
}

void Game::handleMouseClick(sf::Mouse::Button button, int x, int y)
{
    if (button == sf::Mouse::Left)
    {
        auto [gx, gy] = screenToGrid(x, y);
        if (map_.isValidPosition(gx, gy) && map_.isFurnitureTile(gx, gy))
            fireSimulator_->igniteAt(gx, gy);
    }
}