#include "Fighter.h"
#include "Game.h"
#include "Attack.h"
#include <cmath>

Fighter::Fighter(Element elem, sf::Vector2f startPos, const PlayerControls& controls, Game* game)
    : mElement(elem), mPos(startPos), mControls(controls), mGame(game),
      mSize(40.f, 80.f), mVel(0.f, 0.f), mHP(100.f), mMaxHP(100.f),
      mEnergy(100.f), mMaxEnergy(100.f), mIsGrounded(false), 
      mFlashTimer(0.f), mAttackCooldown(0.f), mFacing(1),
      mAirBallTimer(0.f), mUseSprite(false)
{
    mShape.setSize(mSize);
    mShape.setOrigin(mSize.x/2.f, mSize.y/2.f);
    mShape.setPosition(mPos);
    mAnimState = AnimationState::Idle;
    mCurrentFrame=0; mFrameTimer=0; mTimePerFrame=0.15f;
    mFrameWidth=256; mFrameHeight=256; mFramesPerRow=4;
    mSpeed=300; mJumpForce=-500; mWeight=1.0f; mMaxJumps=1;
    
    switch(mElement) {
        case Element::Air:
            mShape.setFillColor(sf::Color(255,200,80));
            mSpeed=420; mWeight=0.5f; mMaxJumps=3; mJumpForce=-480; break;
        case Element::Fire:
            mShape.setFillColor(sf::Color(220,50,30));
            mSpeed=340; mWeight=1.1f; mJumpForce=-460; break;
        case Element::Water:
            mShape.setFillColor(sf::Color(40,120,220));
            mSpeed=310; mWeight=0.9f; break;
        case Element::Earth:
            mShape.setFillColor(sf::Color(60,150,50));
            mSpeed=220; mWeight=1.6f; mJumpForce=-420;
            mMaxHP=130; mHP=130; break;
        case Element::Metal:
            mShape.setFillColor(sf::Color(160,160,170));
            mSpeed=260; mWeight=1.4f; mJumpForce=-440;
            mMaxHP=120; mHP=120; break;
        case Element::Lightning:
            mShape.setFillColor(sf::Color(200,180,255));
            mSpeed=400; mWeight=0.7f; mMaxJumps=2;
            mJumpForce=-500; break;
        case Element::Plant:
            mShape.setFillColor(sf::Color(80,180,60));
            mSpeed=280; mWeight=1.1f; mMaxHP=110; mHP=110; break;
        case Element::Ice:
            mShape.setFillColor(sf::Color(160,220,255));
            mSpeed=320; mWeight=0.9f; break;
        case Element::Sword:
            mSpeed=360; mWeight=1.0f; mMaxHP=115; mHP=115; mMaxJumps=1; break;
        case Element::Fan:
            mSpeed=420; mWeight=0.6f; mMaxHP=105; mHP=105; mMaxJumps=2; mJumpForce=-480; break;
    }
    mJumpsRemaining = mMaxJumps;
}

void Fighter::update(sf::Time deltaTime) {
    float dt = deltaTime.asSeconds();
    handleInput();
    applyPhysics(deltaTime);
    handleAttacks();
    
    if(mFlashTimer>0) mFlashTimer-=dt;
    if(mAttackCooldown>0) mAttackCooldown-=dt;
    
    // Air Ball hover
    if(mAirBallTimer>0){
        mAirBallTimer-=dt;
        mVel.y=-50;
        mAnimState=AnimationState::AirBall;
        // Air bender can still attack during air ball!
        if(!mCpuMode && sf::Keyboard::isKeyPressed(mControls.normalAttack) && mAttackCooldown<=0){
            // Drop air balls downward
            auto a=std::make_unique<Attack>(mPos, sf::Vector2f(mFacing*200,-50), sf::Vector2f(16,16),
                8, sf::Vector2f(mFacing*100,-150), 1.0f, Element::Air, this);
            mGame->addAttack(std::move(a));
            mAttackCooldown=0.3f;
        }
    } else if(mWaterRideTimer>0){
        mWaterRideTimer-=dt;
        mVel.y=-80;
        mAnimState=AnimationState::WaterRide;
    } else if(!mCpuMode && sf::Keyboard::isKeyPressed(mControls.block)){
        mAnimState=AnimationState::Block;
    } else if(mAttackCooldown>0){
        mAnimState=AnimationState::Attack;
    } else if(!mIsGrounded){
        mAnimState=AnimationState::Jump;
    } else if(std::abs(mVel.x)>10){
        mAnimState=AnimationState::Run;
    } else {
        mAnimState=AnimationState::Idle;
    }
    
    mEnergy+=5*dt;
    if(mEnergy>mMaxEnergy) mEnergy=mMaxEnergy;
    mShape.setPosition(mPos);
}

void Fighter::handleInput() {
    if(mCpuMode){ mVel.x*=0.85f; return; } // CPU controls externally
    if(sf::Keyboard::isKeyPressed(mControls.left)){ mVel.x=-mSpeed; mFacing=-1; }
    else if(sf::Keyboard::isKeyPressed(mControls.right)){ mVel.x=mSpeed; mFacing=1; }
    else { mVel.x*=0.8f; }

    static bool jp=false;
    if(sf::Keyboard::isKeyPressed(mControls.up)){
        if(!jp && mJumpsRemaining>0){ mVel.y=mJumpForce; mJumpsRemaining--; mIsGrounded=false; }
        jp=true;
    } else { jp=false; }
}

void Fighter::applyPhysics(sf::Time deltaTime) {
    float dt=deltaTime.asSeconds();
    float grav=(mAirBallTimer>0)?200:1500*mWeight;
    mVel.y+=grav*dt;
    mPos+=mVel*dt;
    float floorY=500;
    if(mPos.y+mSize.y/2>floorY){ mPos.y=floorY-mSize.y/2; mVel.y=0; mIsGrounded=true; mJumpsRemaining=mMaxJumps; }
    if(mPos.x-mSize.x/2<0){ mPos.x=mSize.x/2; mVel.x=0; }
    if(mPos.x+mSize.x/2>800){ mPos.x=800-mSize.x/2; mVel.x=0; }
    if(mPos.y-mSize.y/2<0){ mPos.y=mSize.y/2; mVel.y=0; }
}

void Fighter::handleAttacks() {
    if(mCpuMode||mAttackCooldown>0) return;
    
    // Block (K / Num2) — element-specific defensive move
    if(sf::Keyboard::isKeyPressed(mControls.block)){
        performSpecial2(); // Block
        mAttackCooldown=0.6f;
    }
    else if(sf::Keyboard::isKeyPressed(mControls.normalAttack)){
        performNormalAttack();
        mAttackCooldown=0.25f;
    }
    else if(sf::Keyboard::isKeyPressed(mControls.special1)){
        performSpecial1(); // Element special
        mAttackCooldown=1.0f;
    }
    else if(sf::Keyboard::isKeyPressed(mControls.special3)){
        performSpecial3(); // Ultimate
        mAttackCooldown=2.0f;
    }
}

// --- NORMAL ATTACK: Element-accurate bending ---
void Fighter::performNormalAttack() {
    sf::Vector2f ap=mPos; ap.x+=mFacing*30;
    
    switch(mElement){
        case Element::Air: {
            // Air blast — fast horizontal wind push
            auto a=std::make_unique<Attack>(ap, sf::Vector2f(mFacing*800,0),
                sf::Vector2f(20,14), 6, sf::Vector2f(mFacing*200,-80), 0.8f, mElement, this);
            mGame->addAttack(std::move(a));
            break;
        }
        case Element::Fire: {
            // Fire punch — fireball from fist
            auto a=std::make_unique<Attack>(ap, sf::Vector2f(mFacing*550,0),
                sf::Vector2f(18,18), 12, sf::Vector2f(mFacing*220,-100), 0.9f, mElement, this);
            mGame->addAttack(std::move(a));
            break;
        }
        case Element::Water: {
            // Water whip — arcs toward opponent
            auto a=std::make_unique<Attack>(ap, sf::Vector2f(mFacing*500,-180),
                sf::Vector2f(14,14), 9, sf::Vector2f(mFacing*180,-120), 1.2f, mElement, this);
            mGame->addAttack(std::move(a));
            break;
        }
        case Element::Earth: {
            sf::Vector2f rockPos(mPos.x+mFacing*20, 480);
            auto a=std::make_unique<Attack>(rockPos, sf::Vector2f(mFacing*400,-200),
                sf::Vector2f(22,22), 14, sf::Vector2f(mFacing*260,-140), 1.4f, mElement, this);
            mGame->addAttack(std::move(a)); break;
        }
        case Element::Metal: {
            // Metal blade — fast sharp projectile
            auto a=std::make_unique<Attack>(ap, sf::Vector2f(mFacing*650,0),
                sf::Vector2f(20,8), 13, sf::Vector2f(mFacing*240,-80), 0.8f, mElement, this);
            mGame->addAttack(std::move(a)); break;
        }
        case Element::Lightning: {
            // Lightning bolt — instant fast bolt
            auto a=std::make_unique<Attack>(ap, sf::Vector2f(mFacing*900,-50),
                sf::Vector2f(24,6), 11, sf::Vector2f(mFacing*300,-160), 0.5f, mElement, this, 0);
            mGame->addAttack(std::move(a)); break;
        }
        case Element::Plant: {
            // Vine whip / leaf blade
            auto a=std::make_unique<Attack>(ap, sf::Vector2f(mFacing*500,0),
                sf::Vector2f(25,10), 10, sf::Vector2f(mFacing*200,-100), 1.0f, mElement, this, 0);
            mGame->addAttack(std::move(a)); break;
        }
        case Element::Ice: {
            auto a=std::make_unique<Attack>(ap, sf::Vector2f(mFacing*600,0),
                sf::Vector2f(18,18), 12, sf::Vector2f(mFacing*180,-120), 1.2f, mElement, this, 0);
            mGame->addAttack(std::move(a)); break;
        }
        case Element::Sword: {
            auto a=std::make_unique<Attack>(ap, sf::Vector2f(mFacing*560,-20),
                sf::Vector2f(34,14), 15, sf::Vector2f(mFacing*280,-120), 0.55f, Element::Sword, this, 0);
            mGame->addAttack(std::move(a)); break;
        }
        case Element::Fan: {
            auto a=std::make_unique<Attack>(ap, sf::Vector2f(mFacing*470,-10),
                sf::Vector2f(28,24), 10, sf::Vector2f(mFacing*320,-160), 0.7f, Element::Fan, this, 0);
            mGame->addAttack(std::move(a)); break;
        }
    }
}

// --- SPECIAL (L / Num3): Signature bending move ---
void Fighter::performSpecial1() {
    if(mEnergy<25) return;
    mEnergy-=25;
    
    switch(mElement){
        case Element::Air: {
            // Air Ball — float in spinning air sphere
            mAirBallTimer=2.5f;
            mVel.y=-300;
            break;
        }
        case Element::Fire: {
            // Fire breath — 5 fireballs in spread
            for(int i=-2;i<=2;i++){
                sf::Vector2f ap=mPos; ap.x+=mFacing*30;
                float angle=i*12.f*3.14159f/180.f;
                sf::Vector2f vel(mFacing*480*std::cos(angle), 480*std::sin(angle));
                auto a=std::make_unique<Attack>(ap, vel, sf::Vector2f(14,14), 7,
                    sf::Vector2f(mFacing*150,-60), 0.7f, mElement, this, 1);
                mGame->addAttack(std::move(a));
            }
            break;
        }
        case Element::Water: {
            // Water Tsunami Ride — rise up on a water pillar + shoot water
            mWaterRideTimer=2.0f; mVel.y=-350;
            for(int i=0;i<3;i++){
                sf::Vector2f ap=mPos; ap.x+=mFacing*(20+i*15);
                auto a=std::make_unique<Attack>(ap, sf::Vector2f(mFacing*400,-100+i*50),
                    sf::Vector2f(14,14), 7, sf::Vector2f(mFacing*160,-80), 1.0f, mElement, this, 1);
                mGame->addAttack(std::move(a));
            } break;
        }
        case Element::Earth: {
            for(int i=0;i<3;i++){
                sf::Vector2f rp(mPos.x+mFacing*(30+i*35), 485);
                auto a=std::make_unique<Attack>(rp, sf::Vector2f(mFacing*350,-350-i*80),
                    sf::Vector2f(24,24), 11, sf::Vector2f(mFacing*240,-180), 1.5f, mElement, this, 1);
                mGame->addAttack(std::move(a));
            } break;
        }
        case Element::Metal: {
            // Metal storm — 4 metal blades in fan
            for(int i=-1;i<=2;i++){
                float angle=i*10*3.14159f/180.f;
                sf::Vector2f vel(mFacing*600*std::cos(angle), 600*std::sin(angle));
                auto a=std::make_unique<Attack>(mPos, vel, sf::Vector2f(18,6), 10,
                    sf::Vector2f(mFacing*200,-100), 0.8f, mElement, this, 1);
                mGame->addAttack(std::move(a));
            } break;
        }
        case Element::Lightning: {
            // Chain lightning — 3 fast bolts
            for(int i=0;i<3;i++){
                sf::Vector2f ap(mPos.x+mFacing*30, mPos.y-20+i*20);
                auto a=std::make_unique<Attack>(ap, sf::Vector2f(mFacing*800,-30+i*30),
                    sf::Vector2f(30,4), 9, sf::Vector2f(mFacing*250,-120), 0.6f, mElement, this, 1);
                mGame->addAttack(std::move(a));
            } break;
        }
        case Element::Plant: {
            // Razor leaf flurry
            for(int i=-1;i<=1;i++){
                auto a=std::make_unique<Attack>(mPos, sf::Vector2f(mFacing*450, i*100),
                    sf::Vector2f(20,10), 8, sf::Vector2f(mFacing*150,-80), 1.2f, mElement, this, 1);
                mGame->addAttack(std::move(a));
            } break;
        }
        case Element::Ice: {
            for(int i=0; i<4; i++){
                auto a=std::make_unique<Attack>(mPos, sf::Vector2f(mFacing*300, (i-1.5f)*80),
                    sf::Vector2f(25,25), 6, sf::Vector2f(mFacing*120, -50), 0.8f, mElement, this, 1);
                mGame->addAttack(std::move(a));
            } break;
        }
        case Element::Sword: {
            for(int i=-1;i<=1;i++){
                sf::Vector2f ap=mPos; ap.x+=mFacing*30;
                float angle=i*10.f*3.14159f/180.f;
                sf::Vector2f vel(mFacing*520*std::cos(angle), 520*std::sin(angle));
                auto a=std::make_unique<Attack>(ap, vel, sf::Vector2f(26,14), 18,
                    sf::Vector2f(mFacing*320,-180), 1.0f, Element::Sword, this, 1);
                mGame->addAttack(std::move(a));
            }
            break;
        }
        case Element::Fan: {
            for(int i=-1; i<=1; i++){
                sf::Vector2f ap=mPos; ap.x+=mFacing*(25+i*8);
                auto a=std::make_unique<Attack>(ap, sf::Vector2f(mFacing*430, i*140),
                    sf::Vector2f(28,22), 13, sf::Vector2f(mFacing*240,-90), 0.9f, Element::Fan, this, 1);
                mGame->addAttack(std::move(a));
            }
            break;
        }
    }
}

// --- BLOCK (K / Num2): Element-specific shield ---
void Fighter::performSpecial2() {
    if(mEnergy<10) return;
    mEnergy-=10;
    
    switch(mElement){
        case Element::Air: {
            // Air shield — spinning air dome that pushes enemies away
            auto a=std::make_unique<Attack>(mPos, sf::Vector2f(0,0),
                sf::Vector2f(50,50), 2, sf::Vector2f(mFacing*300,-200), 0.8f, mElement, this, 2);
            mGame->addAttack(std::move(a));
            break;
        }
        case Element::Fire: {
            // Fire wall — wall of flame in front
            sf::Vector2f fp(mPos.x+mFacing*40, mPos.y);
            auto a=std::make_unique<Attack>(fp, sf::Vector2f(0,0),
                sf::Vector2f(16,60), 5, sf::Vector2f(mFacing*150,-50), 1.2f, mElement, this, 2);
            mGame->addAttack(std::move(a));
            break;
        }
        case Element::Water: {
            // Ice wall — frozen water barrier + heals slightly
            sf::Vector2f wp(mPos.x+mFacing*45, mPos.y);
            auto a=std::make_unique<Attack>(wp, sf::Vector2f(0,0),
                sf::Vector2f(18,65), 4, sf::Vector2f(mFacing*120,-80), 1.5f, mElement, this, 2);
            mGame->addAttack(std::move(a));
            heal(10);
            break;
        }
        case Element::Earth: {
            sf::Vector2f ep(mPos.x+mFacing*45, 460);
            auto a=std::make_unique<Attack>(ep, sf::Vector2f(0,-100),
                sf::Vector2f(24,70), 6, sf::Vector2f(mFacing*350,-120), 2.5f, mElement, this, 2);
            mGame->addAttack(std::move(a)); break;
        }
        case Element::Metal: {
            // Metal armor shield — absorb + reflect
            auto a=std::make_unique<Attack>(mPos, sf::Vector2f(0,0),
                sf::Vector2f(45,60), 3, sf::Vector2f(mFacing*250,-100), 1.0f, mElement, this, 2);
            mGame->addAttack(std::move(a)); break;
        }
        case Element::Lightning: {
            // Thunder field — electric barrier
            auto a=std::make_unique<Attack>(mPos, sf::Vector2f(0,0),
                sf::Vector2f(55,55), 4, sf::Vector2f(mFacing*200,-180), 0.7f, mElement, this, 2);
            mGame->addAttack(std::move(a)); break;
        }
        case Element::Plant: {
            auto a=std::make_unique<Attack>(mPos, sf::Vector2f(0,0),
                sf::Vector2f(30,65), 5, sf::Vector2f(mFacing*160,-100), 1.2f, mElement, this, 2);
            mGame->addAttack(std::move(a)); break;
        }
        case Element::Ice: {
            auto a=std::make_unique<Attack>(mPos, sf::Vector2f(0,0),
                sf::Vector2f(40,60), 4, sf::Vector2f(mFacing*180,-120), 1.0f, mElement, this, 2);
            mGame->addAttack(std::move(a)); break;
        }
        case Element::Sword: {
            auto a=std::make_unique<Attack>(mPos, sf::Vector2f(0,0),
                sf::Vector2f(24,64), 6, sf::Vector2f(mFacing*220,-120), 0.8f, Element::Sword, this, 2);
            mGame->addAttack(std::move(a)); break;
        }
        case Element::Fan: {
            auto a=std::make_unique<Attack>(mPos, sf::Vector2f(0,0),
                sf::Vector2f(52,52), 4, sf::Vector2f(mFacing*260,-160), 0.75f, Element::Fan, this, 2);
            mGame->addAttack(std::move(a)); break;
        }
    }
}

// --- ULTIMATE (I / Num0): Most powerful move ---
void Fighter::performSpecial3() {
    if(mEnergy<50) return;
    mEnergy-=50;
    
    switch(mElement){
        case Element::Air: {
            // Storm burst — 8 wind blasts in all directions
            for(int i=0;i<8;i++){
                float a=i*45*3.14159f/180.f;
                sf::Vector2f vel(std::cos(a)*500, std::sin(a)*500);
                auto atk=std::make_unique<Attack>(mPos, vel, sf::Vector2f(18,18),
                    10, sf::Vector2f(vel.x*0.4f,vel.y*0.4f-100), 1.0f, mElement, this, 3);
                mGame->addAttack(std::move(atk));
            }
            break;
        }
        case Element::Fire: {
            // Comet — massive fireball
            sf::Vector2f ap=mPos; ap.x+=mFacing*40;
            auto a=std::make_unique<Attack>(ap, sf::Vector2f(mFacing*380,0),
                sf::Vector2f(50,50), 35, sf::Vector2f(mFacing*400,-250), 2.0f, mElement, this, 3);
            mGame->addAttack(std::move(a));
            break;
        }
        case Element::Water: {
            // Tidal wave — huge wide water wall
            sf::Vector2f ap=mPos; ap.x+=mFacing*40;
            auto a=std::make_unique<Attack>(ap, sf::Vector2f(mFacing*280,0),
                sf::Vector2f(80,50), 25, sf::Vector2f(mFacing*350,-200), 1.5f, mElement, this, 3);
            mGame->addAttack(std::move(a));
            break;
        }
        case Element::Earth: {
            for(int dir=-1;dir<=1;dir+=2){
                auto a=std::make_unique<Attack>(sf::Vector2f(mPos.x,475), sf::Vector2f(dir*350,0),
                    sf::Vector2f(60,30), 20, sf::Vector2f(0,-400), 1.5f, mElement, this, 3);
                mGame->addAttack(std::move(a));
            } break;
        }
        case Element::Metal: {
            // Meteor rain — 6 metal chunks falling from sky
            for(int i=0;i<6;i++){
                sf::Vector2f sp(mPos.x+mFacing*(50+i*40)-60, 30);
                auto a=std::make_unique<Attack>(sp, sf::Vector2f(mFacing*80,500),
                    sf::Vector2f(20,20), 15, sf::Vector2f(0,-300), 1.5f, mElement, this, 3);
                mGame->addAttack(std::move(a));
            } break;
        }
        case Element::Lightning: {
            // Thunder storm — 5 bolts across screen
            for(int i=0;i<5;i++){
                sf::Vector2f sp(100+i*150, 20);
                auto a=std::make_unique<Attack>(sp, sf::Vector2f(0,800),
                    sf::Vector2f(8,40), 18, sf::Vector2f(0,-350), 0.8f, mElement, this, 3);
                mGame->addAttack(std::move(a));
            } break;
        }
        case Element::Plant: {
            // Giant root spikes
            for(int i=0;i<4;i++){
                sf::Vector2f sp(mPos.x+mFacing*(50+i*60), 480);
                auto a=std::make_unique<Attack>(sp, sf::Vector2f(0,-400),
                    sf::Vector2f(40,80), 22, sf::Vector2f(mFacing*300,-300), 1.2f, mElement, this, 3);
                mGame->addAttack(std::move(a));
            } break;
        }
        case Element::Ice: {
            for(int i=0; i<3; i++){
                sf::Vector2f sp(mPos.x+mFacing*(40+i*80), 450);
                auto a=std::make_unique<Attack>(sp, sf::Vector2f(mFacing*50,-200),
                    sf::Vector2f(50,100), 25, sf::Vector2f(mFacing*350,-250), 1.5f, mElement, this, 3);
                mGame->addAttack(std::move(a));
            } break;
        }
        case Element::Sword: {
            for(int i=0; i<5; i++){
                sf::Vector2f sp(mPos.x+mFacing*(20+i*40), mPos.y);
                auto a=std::make_unique<Attack>(sp, sf::Vector2f(mFacing*600,0),
                    sf::Vector2f(42,42), 30, sf::Vector2f(mFacing*420,-220), 0.3f, Element::Sword, this, 3);
                mGame->addAttack(std::move(a));
            } break;
        }
        case Element::Fan: {
            auto a=std::make_unique<Attack>(mPos, sf::Vector2f(mFacing*200,0),
                sf::Vector2f(100,200), 40, sf::Vector2f(mFacing*500,-400), 2.0f, Element::Fan, this, 3);
            mGame->addAttack(std::move(a)); break;
        }
    }
}

void Fighter::takeDamage(float amount, float kbX, float kbY) {
    // Blocking reduces damage by 70%
    if(mAnimState==AnimationState::Block){ amount*=0.3f; kbX*=0.3f; kbY*=0.3f; }
    mHP-=amount; mVel.x+=kbX; mVel.y+=kbY;
    mFlashTimer=0.15f; mAirBallTimer=0;
    if(mHP<0) mHP=0;
}
void Fighter::heal(float a){ mHP+=a; if(mHP>mMaxHP) mHP=mMaxHP; }
void Fighter::addEnergy(float a){ mEnergy+=a; if(mEnergy>mMaxEnergy) mEnergy=mMaxEnergy; }
sf::FloatRect Fighter::getHitbox() const { return sf::FloatRect(mPos.x-mSize.x/2,mPos.y-mSize.y/2,mSize.x,mSize.y); }
void Fighter::render(sf::RenderWindow& w){ w.draw(mShape); }
void Fighter::spawnAttack(int){}
