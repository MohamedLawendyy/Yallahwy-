// File: src/Game.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <vector>
#include "Map.hpp"
#include "FireSimulator.hpp"
#include "Renderer.hpp"
#include "PhysicsEngine.hpp"
#include "PlayerController.hpp"
#include "BulletSystem.hpp"
#include "ShootingSystem.hpp"
#include "IUpdatable.hpp"
#include "IDrawable.hpp"
#include "GameState.hpp"
#include "MainMenu.hpp"
#include "GameUI.hpp"
#include "GameOverScreen.hpp"

class Game
{
public:
    Game();
    void run();

private:
    // Core components
    sf::RenderWindow window_;
    sf::Clock clock_;
    Map map_;

    // Audio
    sf::Music bgMusic_;

    // Game State
    GameState currentState_;

    // Configuration
    MapConfig mapConfig_;
    FireSpreadConfig fireConfig_;
    PlayerConfig playerConfig_;

    // Systems
    std::unique_ptr<PhysicsEngine> physicsEngine_;
    std::unique_ptr<FireSimulator> fireSimulator_;
    std::unique_ptr<PlayerController> playerController_;
    std::unique_ptr<BulletSystem> bulletSystem_;
    std::unique_ptr<ShootingSystem> shootingSystem_;
    std::unique_ptr<Renderer> renderer_;

    // UI Systems
    std::unique_ptr<MainMenu> mainMenu_;
    std::unique_ptr<GameUI> gameUI_;
    std::unique_ptr<GameOverScreen> gameOverScreen_;

    // System collections
    std::vector<IUpdatable *> updatables_;
    std::vector<IDrawable *> drawables_;

    // State
    bool running_ = true;
    bool paused_ = false;
    bool showDebug_ = false;
    bool isFullscreen_ = true;

    // Metrics
    float waterLevel_ = 1.0f;
    float fireIntensity_ = 0.0f;
    size_t initialFlammableCount_ = 0;
    bool hasShownWinScreen_ = false;
    bool hasShownLoseScreen_ = false;

    // Transitions
    sf::RectangleShape transitionOverlay_;
    float transitionAlpha_ = 0.0f;
    bool isTransitioning_ = false;
    bool isFadingOut_ = false;
    bool pendingReset_ = false;

    // ========================================================
    // NEW: Camera Animation State
    // ========================================================
    enum class CameraMode
    {
        Overview,
        Zooming,
        Locked
    };
    CameraMode cameraMode_ = CameraMode::Overview;
    sf::View gameView_;
    float cameraTimer_ = 0.0f;

    // Camera Settings
    const float CAMERA_OVERVIEW_DURATION = 2.0f; // Seconds to stay in full map view
    const float CAMERA_ZOOM_DURATION = 2.0f;     // Seconds to zoom in
    const float TARGET_ZOOM_HEIGHT = 400.0f;     // Height of view when zoomed in (pixels)

    // Helper Methods
    void initLevel();
    void startTransition(bool isReset);
    void updateTransition(float dt);
    void updateCamera(float dt); // NEW helper

    // NEW: Check for water refill
    void checkWaterRefill(float dt);

    // Core Loop
    void processEvents(sf::Event &event);
    void processMenuEvents(sf::Event &event);
    void processGameEvents(sf::Event &event);
    void update(std::chrono::milliseconds deltaTime);
    void updateMenu(float deltaTime);
    void updateGame(std::chrono::milliseconds deltaTime);
    void render();
    void renderMenu();
    void renderGame();
    void setupRender(); // Used for initial setup only now
    void handleKeyPress(sf::Keyboard::Key key);
    void handleMouseClick(sf::Mouse::Button button, int x, int y);

    // Logic
    void updateUIMetrics();
    void checkWinLoseConditions();
    void checkWaterFireCollision();
    void drawDebugColliders();

    // Initialization
    bool initializeSystems();
    bool initializeMenu();
    void returnToMenu();
    void toggleFullscreen();

    // View helpers
    sf::View getLetterboxView(sf::FloatRect gameBounds, int windowWidth, int windowHeight) const;
    std::pair<size_t, size_t> screenToGrid(int screenX, int screenY) const;
};