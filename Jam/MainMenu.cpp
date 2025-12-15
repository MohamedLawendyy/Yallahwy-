// File: src/MainMenu.cpp
#include "MainMenu.hpp"
#include <iostream>

MainMenu::MainMenu(sf::RenderWindow &window, sf::Music &music)
    : window_(window),
      music_(music), // Store reference
      bgOffset_(0.f),
      bgSpeed_(70.f),
      mute_(false),
      startGame_(false),
      exitGame_(false)
{
    // Sync mute state with current music volume
    if (music_.getVolume() < 5.f)
        mute_ = true;
}

MainMenu::~MainMenu()
{
    // Do NOT stop music here, it belongs to Game now
}

bool MainMenu::loadAssets()
{
    if (!menuBGTexture_.loadFromFile("textures/UIBackground_V2.png"))
        return false;
    if (!menuTexture_.loadFromFile("textures/UIHelment.png"))
        return false;
    if (!logoTexture_.loadFromFile("textures/UILogo_V2.png"))
        return false;
    if (!volTexture_.loadFromFile("textures/UIVolume.png"))
        return false;
    if (!volFireTexture_.loadFromFile("textures/UIFire.png"))
        return false;

    if (!font_.loadFromFile("fonts/DynaPuff-Bold.ttf"))
        return false;

    menuBGTexture_.setRepeated(true);
    setupShapes();

    // Initialize volume icon state based on current music volume
    if (mute_)
        volFire_.setScale(0.f, 0.f);
    else
        volFire_.setScale(1.f, 1.f);

    return true;
}

void MainMenu::setupShapes()
{
    sf::Vector2u windowSize = window_.getSize();

    menuBG_.setSize(sf::Vector2f(windowSize.x, windowSize.y));
    menuBG_.setTexture(&menuBGTexture_);
    menuBG_.setTextureRect(sf::IntRect(0, 0, 6000, 3000));
    menuBG_.setFillColor(sf::Color(255, 220, 30, 220));

    logo_.setSize(sf::Vector2f(933, 315));
    logo_.setTexture(&logoTexture_);
    logo_.setTextureRect(sf::IntRect(0, 0, 933, 315));
    logo_.setOrigin(logo_.getLocalBounds().width / 2.f, logo_.getLocalBounds().height / 2.f);
    logo_.setPosition(windowSize.x / 2.f, windowSize.y / 6.f);
    logo_.setScale(0.8f, 0.8f);

    menu_.setSize(sf::Vector2f(784, 570));
    menu_.setTexture(&menuTexture_);
    menu_.setTextureRect(sf::IntRect(0, 0, 784, 570));
    menu_.setOrigin(menu_.getLocalBounds().width / 2.f, menu_.getLocalBounds().height / 2.f);
    menu_.setPosition(windowSize.x / 2.f, windowSize.y / 1.7f);
    menu_.setScale(0.9f, 0.9f);

    vol_.setSize(sf::Vector2f(178, 83));
    vol_.setTexture(&volTexture_);
    vol_.setTextureRect(sf::IntRect(0, 0, 178, 83));
    vol_.setPosition(windowSize.x - 200, windowSize.y - 150);

    volFire_.setSize(sf::Vector2f(46, 57));
    volFire_.setTexture(&volFireTexture_);
    volFire_.setTextureRect(sf::IntRect(0, 0, 46, 57));
    volFire_.setPosition(windowSize.x - 185, windowSize.y - 140);

    startBtn_.setSize(sf::Vector2f(400, 100));
    startBtn_.setFillColor(sf::Color::Transparent);
    startBtn_.setOrigin(200, 50);
    startBtn_.setPosition(windowSize.x / 2, windowSize.y / 1.7);

    exitBtn_.setSize(sf::Vector2f(400, 100));
    exitBtn_.setFillColor(sf::Color::Transparent);
    exitBtn_.setOrigin(200, 50);
    exitBtn_.setPosition(windowSize.x / 2, windowSize.y / 1.4);

    startText_.setFont(font_);
    startText_.setString("Start");
    startText_.setCharacterSize(80);
    startText_.setFillColor(sf::Color::White);
    startText_.setOutlineThickness(10);
    startText_.setOutlineColor(sf::Color(0, 0, 0, 255));
    startText_.setOrigin(startText_.getLocalBounds().width / 2, startText_.getLocalBounds().height / 2);
    startText_.setPosition(startBtn_.getPosition());

    exitText_.setFont(font_);
    exitText_.setString("Exit");
    exitText_.setCharacterSize(80);
    exitText_.setFillColor(sf::Color::White);
    exitText_.setOutlineThickness(10);
    exitText_.setOutlineColor(sf::Color(0, 0, 0, 255));
    exitText_.setOrigin(exitText_.getLocalBounds().width / 2, exitText_.getLocalBounds().height / 2);
    exitText_.setPosition(exitBtn_.getPosition());
}

void MainMenu::update(float dt)
{
    updateBackground(dt);
    updateHoverEffects();
}

void MainMenu::updateBackground(float dt)
{
    bgOffset_ += bgSpeed_ * dt;
    menuBG_.setTextureRect(sf::IntRect(0, bgOffset_, 6000, 3000));
}

void MainMenu::updateHoverEffects()
{
    sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window_));

    if (startBtn_.getGlobalBounds().contains(mousePos))
    {
        startText_.setFillColor(sf::Color::Yellow);
        startText_.setScale(1.1f, 1.1f);
    }
    else
    {
        startText_.setFillColor(sf::Color::White);
        startText_.setScale(1.f, 1.f);
    }

    if (exitBtn_.getGlobalBounds().contains(mousePos))
    {
        exitText_.setFillColor(sf::Color::Yellow);
        exitText_.setScale(1.1f, 1.1f);
    }
    else
    {
        exitText_.setFillColor(sf::Color::White);
        exitText_.setScale(1.f, 1.f);
    }

    if (vol_.getGlobalBounds().contains(mousePos))
    {
        vol_.setScale(1.1f, 1.1f);
        if (!mute_)
            volFire_.setScale(1.1f, 1.1f);
    }
    else
    {
        vol_.setScale(1.f, 1.f);
        if (!mute_)
            volFire_.setScale(1.f, 1.f);
    }
}

void MainMenu::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        handleMouseClick(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
    }
}

void MainMenu::handleMouseClick(const sf::Vector2f &mousePos)
{
    if (startBtn_.getGlobalBounds().contains(mousePos))
    {
        startGame_ = true;
        // Do NOT stop music here
    }

    if (exitBtn_.getGlobalBounds().contains(mousePos))
    {
        exitGame_ = true;
    }

    if (vol_.getGlobalBounds().contains(mousePos))
    {
        mute_ = !mute_;
        if (mute_)
        {
            music_.setVolume(0.f);
            volFire_.setScale(0.f, 0.f);
        }
        else
        {
            music_.setVolume(50.f);
            volFire_.setScale(1.f, 1.f);
        }
    }
}

void MainMenu::render()
{
    window_.draw(menuBG_);
    window_.draw(menu_);
    window_.draw(logo_);
    window_.draw(vol_);
    window_.draw(volFire_);
    window_.draw(startText_);
    window_.draw(exitText_);
}

void MainMenu::reset()
{
    startGame_ = false;
    exitGame_ = false;
}