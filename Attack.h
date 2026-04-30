#ifndef ATTACK_H
#define ATTACK_H

#include <SFML/Graphics.hpp>
#include "Fighter.h"

class ParticleSystem;

class Attack {
public:
    Attack(sf::Vector2f startPos, sf::Vector2f velocity, sf::Vector2f size, 
           float damage, sf::Vector2f knockback, float lifetime, Element element, Fighter* owner, int type = 0);

    void update(sf::Time deltaTime, ParticleSystem& particles);
    void render(sf::RenderWindow& window);

    bool isDead() const { return mLifeTime <= 0.f || mIsDestroyed; }
    void destroy() { mIsDestroyed = true; }
    
    sf::FloatRect getHitbox() const;
    
    float getDamage() const { return mDamage; }
    sf::Vector2f getKnockback() const { return mKnockback; }
    Fighter* getOwner() const { return mOwner; }
    Element getElement() const { return mElement; }
    int getType() const { return mType; }

private:
    sf::Vector2f mPos;
    sf::Vector2f mVel;
    sf::Vector2f mSize;
    
    float mDamage;
    sf::Vector2f mKnockback;
    float mLifeTime;
    float mMaxLife;
    bool mIsDestroyed;
    
    Element mElement;
    Fighter* mOwner; // To avoid hitting the owner
    int mType; // 0=Normal, 1=Special, 2=Block, 3=Ultimate
    float mOffsetX;


    sf::RectangleShape mShape; // Placeholder
};

#endif // ATTACK_H
