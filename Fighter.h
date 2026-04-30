#ifndef FIGHTER_H
#define FIGHTER_H

#include <SFML/Graphics.hpp>
#include "InputHandler.h"

enum class Element { Air, Fire, Water, Earth, Metal, Lightning, Plant, Ice, Sword, Fan };

enum class AnimationState {
    Idle, Run, Jump, Attack, AirBall, WaterRide, Block
};

class Game;

class Fighter {
public:
    Fighter(Element elem, sf::Vector2f startPos, const PlayerControls& controls, Game* game);

    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window);

    void takeDamage(float amount, float knockbackX, float knockbackY);
    void heal(float amount);
    void addEnergy(float amount);
    
    sf::FloatRect getHitbox() const;
    sf::Vector2f getPosition() const { return mPos; }
    Element getElement() const { return mElement; }
    float getHP() const { return mHP; }
    float getMaxHP() const { return mMaxHP; }
    float getEnergy() const { return mEnergy; }
    float getMaxEnergy() const { return mMaxEnergy; }
    bool isDead() const { return mHP <= 0; }
    AnimationState getAnimState() const { return mAnimState; }
    int getFacing() const { return mFacing; }
    float getFlashTimer() const { return mFlashTimer; }
    bool isInAirBall() const { return mAirBallTimer > 0.f; }
    float getAirBallTimer() const { return mAirBallTimer; }
    
    void setPosition(sf::Vector2f pos) { mPos = pos; }
    void setVelX(float vx) { mVel.x = vx; }
    void setFacing(int f) { mFacing = f; }
    void cpuJump() { if(mJumpsRemaining>0){ mVel.y=mJumpForce; mJumpsRemaining--; mIsGrounded=false; } }
    void cpuAttack() { if(mAttackCooldown<=0){ performNormalAttack(); mAttackCooldown=0.3f; } }
    void cpuSpecial() { if(mAttackCooldown<=0){ performSpecial1(); mAttackCooldown=1.0f; } }
    void cpuBlock() { if(mAttackCooldown<=0){ performSpecial2(); mAttackCooldown=0.6f; } }
    void cpuUltimate() { if(mAttackCooldown<=0){ performSpecial3(); mAttackCooldown=2.0f; } }
    void setCpuMode(bool v) { mCpuMode=v; }
    bool isCpuMode() const { return mCpuMode; }

private:
    void handleInput();
    void applyPhysics(sf::Time deltaTime);
    void handleAttacks();
    
    void performNormalAttack();
    void performSpecial1();
    void performSpecial2();
    void performSpecial3();
    
    void spawnAttack(int type);

    Game* mGame;
    Element mElement;
    PlayerControls mControls;

    sf::Vector2f mPos;
    sf::Vector2f mVel;
    sf::Vector2f mSize;
    
    sf::RectangleShape mShape;
    sf::Sprite mSprite;
    sf::Texture mTexture;
    bool mUseSprite;

    float mHP, mMaxHP;
    float mEnergy, mMaxEnergy;

    AnimationState mAnimState;
    int mCurrentFrame;
    float mFrameTimer, mTimePerFrame;
    int mFrameWidth, mFrameHeight, mFramesPerRow;

    float mSpeed, mJumpForce, mWeight;
    int mMaxJumps, mJumpsRemaining;
    bool mIsGrounded;
    
    float mFlashTimer;
    float mAttackCooldown;
    int mFacing;
    
    // Air Ball (air element special)
    float mAirBallTimer;
    float mWaterRideTimer = 0.f;
    bool mCpuMode = false;
};

#endif
