#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class AnimatedSprite : public sf::Drawable, public sf::Transformable {
public:
    AnimatedSprite();

    bool loadFromFile(const std::string& filename, unsigned frames);
    void setFrameTime(float seconds);
    void play(bool loop = true);
    void update(float dt);
    void setFlipX(bool flip);
    void setSpriteScale(float sx, float sy);
    void clearFrames();
    void addFrame(const sf::IntRect& rect);
    
    // NEW: Expose global bounds for collision detection (via underlying sprite)
    sf::FloatRect getGlobalBounds() const;

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Texture m_texture;
    sf::Sprite m_sprite;

    unsigned m_frames = 1;
    unsigned m_currentFrame = 0;
    float m_frameTime = 0.1f;
    float m_accumulator = 0.f;
    bool m_loop = true;
    bool m_flipX = false;

    int frameWidth = 0;
    int frameHeight = 0;
    std::vector<sf::IntRect> m_framesRects;
};