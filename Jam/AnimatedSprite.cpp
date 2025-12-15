#include "AnimatedSprite.h"
#include <cmath>

AnimatedSprite::AnimatedSprite() {}

// NEW: Implementation for getGlobalBounds
sf::FloatRect AnimatedSprite::getGlobalBounds() const {
    return m_sprite.getGlobalBounds();
}

void AnimatedSprite::clearFrames()
{
    m_framesRects.clear();
    m_frames = 0;
    m_currentFrame = 0;
    m_accumulator = 0.f;
    m_sprite.setTextureRect(sf::IntRect());
}

void AnimatedSprite::addFrame(const sf::IntRect& rect)
{
    m_framesRects.push_back(rect);
    m_frames = static_cast<unsigned>(m_framesRects.size());
}

bool AnimatedSprite::loadFromFile(const std::string& filename, unsigned frames)
{
    if (!m_texture.loadFromFile(filename))
        return false;

    m_texture.setSmooth(true);
    m_sprite.setTexture(m_texture);

    // Do NOT auto-calc frame width/height if custom frames exist
    m_frames = frames;
    m_currentFrame = 0;

    if (m_framesRects.empty())
    {
        // AUTO MODE (one row sliced horizontally)
        frameWidth = m_texture.getSize().x / frames;
        frameHeight = m_texture.getSize().y;

        m_sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
        m_sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    }
    else
    {
        // MANUAL MODE
        m_sprite.setTextureRect(m_framesRects[0]);
        frameWidth = m_framesRects[0].width;
        frameHeight = m_framesRects[0].height;
        m_sprite.setOrigin(frameWidth / 2.f, frameHeight / 2.f);
    }

    return true;
}

void AnimatedSprite::setFrameTime(float seconds)
{
    m_frameTime = seconds;
}

void AnimatedSprite::play(bool loop)
{
    m_loop = loop;
}

void AnimatedSprite::update(float dt)
{
    m_accumulator += dt;

    if (m_accumulator >= m_frameTime)
    {
        m_accumulator -= m_frameTime;
        m_currentFrame++;

        if (m_currentFrame >= m_frames)
        {
            if (m_loop) m_currentFrame = 0;
            else m_currentFrame = m_frames - 1;
        }

        // Use manual frames if available
        if (!m_framesRects.empty())
        {
            m_sprite.setTextureRect(m_framesRects[m_currentFrame]);
        }
        else
        {
            // Default slicing
            int left = m_currentFrame * frameWidth;
            m_sprite.setTextureRect(sf::IntRect(left, 0, frameWidth, frameHeight));
        }

        if (m_flipX)
            m_sprite.setScale(-std::abs(m_sprite.getScale().x), m_sprite.getScale().y);
        else
            m_sprite.setScale(std::abs(m_sprite.getScale().x), m_sprite.getScale().y);
    }
}

void AnimatedSprite::setFlipX(bool flip)
{
    m_flipX = flip;
}

void AnimatedSprite::setSpriteScale(float sx, float sy)
{
    float sign = m_flipX ? -1.f : 1.f;
    m_sprite.setScale(sign * sx, sy);
}

void AnimatedSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_sprite, states);
}