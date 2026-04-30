#include "Attack.h"
#include "ParticleSystem.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Attack::Attack(sf::Vector2f startPos, sf::Vector2f velocity, sf::Vector2f size, 
               float damage, sf::Vector2f knockback, float lifetime, Element element, Fighter* owner, int type)
    : mPos(startPos), mVel(velocity), mSize(size), mDamage(damage), 
      mKnockback(knockback), mLifeTime(lifetime), mMaxLife(lifetime),
      mElement(element), mOwner(owner), mIsDestroyed(false), mType(type)
{
    mShape.setSize(mSize);
    mShape.setOrigin(mSize.x/2.f, mSize.y/2.f);
    mShape.setPosition(mPos);
    if(mOwner) mOffsetX = std::abs(mPos.x - mOwner->getPosition().x);
    else mOffsetX = 0;
}

void Attack::update(sf::Time deltaTime, ParticleSystem& particles) {
    float dt = deltaTime.asSeconds();
    
    // Dynamic shield tracking
    if(mType == 2 && mOwner) {
        mPos.x = mOwner->getPosition().x + mOwner->getFacing() * mOffsetX;
    }
    // Earth rocks: rise from ground first
    else if(mElement==Element::Earth && mLifeTime>mMaxLife-0.2f){
        mPos.y-=300*dt;
        mVel=sf::Vector2f(mVel.x,0);
    } else {
        mPos+=mVel*dt;
    }
    // Water arc gravity
    if(mElement==Element::Water && mType != 2) mVel.y+=400*dt;
    // Big fireballs slight gravity
    if(mElement==Element::Fire && mSize.x>30 && mType != 2) mVel.y+=100*dt;
    
    mLifeTime-=dt;
    mShape.setPosition(mPos);
    
    // --- HEAVY PARTICLE EMISSION ---
    if(mElement==Element::Fire){
        // Tons of fire particles — flames, embers, sparks
        particles.emit(mPos, 5, sf::Color(255,80,0));
        particles.emit(mPos, 4, sf::Color(255,160,0));
        particles.emit(mPos, 3, sf::Color(255,220,50));
        particles.emit(mPos, 2, sf::Color(255,255,150));  // Bright core sparks
        // Smoke trail
        particles.emit(sf::Vector2f(mPos.x-mVel.x*0.01f, mPos.y), 2, sf::Color(80,60,40,100));
    } else if(mElement==Element::Water){
        // Lots of water droplets, spray, mist
        particles.emit(mPos, 5, sf::Color(40,120,240));
        particles.emit(mPos, 4, sf::Color(80,180,255));
        particles.emit(mPos, 3, sf::Color(160,220,255));  // White spray
        particles.emit(mPos, 2, sf::Color(200,240,255,120)); // Mist
    } else if(mElement==Element::Air){
        particles.emit(mPos, 3, sf::Color(200,230,255,150));
        particles.emit(mPos, 1, sf::Color(255,255,255,80));
    } else if(mElement==Element::Earth){
        particles.emit(mPos, 2, sf::Color(120,90,50));
        particles.emit(mPos, 2, sf::Color(80,80,80));
        particles.emit(mPos, 1, sf::Color(160,130,80,100));
    } else if(mElement==Element::Metal){
        particles.emit(mPos, 3, sf::Color(180,180,190));
        particles.emit(mPos, 2, sf::Color(220,220,230));
        particles.emit(mPos, 2, sf::Color(255,200,100)); // Sparks
    } else if(mElement==Element::Lightning){
        particles.emit(mPos, 4, sf::Color(180,140,255));
        particles.emit(mPos, 3, sf::Color(255,255,100));
        particles.emit(mPos, 2, sf::Color(255,255,255,200)); // Bright flash
    } else if(mElement==Element::Plant){
        particles.emit(mPos, 3, sf::Color(60,150,50));
        particles.emit(mPos, 2, sf::Color(100,200,80));
    } else if(mElement==Element::Ice){
        particles.emit(mPos, 3, sf::Color(180,240,255));
        particles.emit(mPos, 2, sf::Color(255,255,255));
    } else if(mElement==Element::Sword){
        particles.emit(mPos, 2, sf::Color(200,220,255,150));
        particles.emit(mPos, 1, sf::Color(255,255,255,200));
    } else if(mElement==Element::Fan){
        particles.emit(mPos, 3, sf::Color(255,200,80,180));
        particles.emit(mPos, 2, sf::Color(255,240,150,200));
    }
}


void Attack::render(sf::RenderWindow& window) {
    float t = mMaxLife - mLifeTime;
    int type = mType; // 0=Normal, 1=Special, 2=Block, 3=Ultimate
    
    switch(mElement) {
        case Element::Air: {
            if (type == 0) {
                // Air Slice (Crescent)
                sf::ConvexShape crescent; crescent.setPointCount(4);
                float s = mSize.x / 2;
                crescent.setPoint(0, {s, 0}); crescent.setPoint(1, {0, -s});
                crescent.setPoint(2, {-s*0.5f, 0}); crescent.setPoint(3, {0, s});
                crescent.setPosition(mPos);
                crescent.setRotation(std::atan2(mVel.y, mVel.x) * 180 / 3.14159f);
                crescent.setFillColor(sf::Color(200, 240, 255, 180));
                window.draw(crescent);
                // Inner highlight
                crescent.setScale(0.5f, 0.5f);
                crescent.setFillColor(sf::Color(255, 255, 255, 200));
                window.draw(crescent);
            }
            else if (type == 2) {
                // Air Dome (Block)
                for(int i=0; i<3; i++){
                    float a = t * 15 + i;
                    sf::CircleShape dome(mSize.x/2); dome.setOrigin(mSize.x/2, mSize.x/2);
                    dome.setPosition(mPos);
                    dome.setFillColor(sf::Color::Transparent);
                    dome.setOutlineThickness(2 + std::sin(a)*1.5f);
                    dome.setOutlineColor(sf::Color(180, 220, 255, (sf::Uint8)(100 + 50*std::sin(a*2))));
                    dome.setScale(1.f, 0.8f + 0.2f*std::cos(a));
                    window.draw(dome);
                }
            }
            else if (type == 3) {
                // Tornado (Ultimate)
                for(int i=0; i<5; i++){
                    float wy = mPos.y + (i-2)*8;
                    float ww = mSize.x/2 - abs(i-2)*2 + std::sin(t*20+i)*4;
                    sf::RectangleShape wind({ww*2, 4}); wind.setOrigin(ww, 2);
                    wind.setPosition(mPos.x + std::cos(t*15+i)*5, wy);
                    wind.setFillColor(sf::Color(220, 240, 255, 200));
                    window.draw(wind);
                }
            }
            break;
        }
        case Element::Fire: {
            if (type == 0 || type == 1) {
                // Fireball
                sf::CircleShape core(mSize.x/3); core.setOrigin(core.getRadius(), core.getRadius());
                core.setPosition(mPos);
                core.setFillColor(sf::Color(255, 255, 150));
                window.draw(core);
                for(int i=0; i<3; i++){
                    float r = mSize.x/2 + std::sin(t*15+i)*3;
                    sf::CircleShape flame(r, 6); flame.setOrigin(r, r);
                    flame.setPosition(mPos.x - mVel.x*0.002f*i, mPos.y - mVel.y*0.002f*i);
                    flame.setFillColor(sf::Color(255, 100-i*30, 0, 150));
                    window.draw(flame);
                }
            }
            else if (type == 2) {
                // Fire Wall (Block)
                for(int i=0; i<8; i++){
                    float fh = mSize.y/2 + std::sin(t*15+i)*10;
                    sf::RectangleShape fw({6, fh}); fw.setOrigin(3, fh);
                    fw.setPosition(mPos.x + (i-4)*4, mPos.y + mSize.y/2);
                    fw.setFillColor(sf::Color(255, 120-i*10, 0, 200));
                    window.draw(fw);
                }
            }
            else if (type == 3) {
                // Comet (Ultimate)
                sf::CircleShape core(mSize.x/2.5f); core.setOrigin(core.getRadius(), core.getRadius());
                core.setPosition(mPos); core.setFillColor(sf::Color(255, 255, 255)); window.draw(core);
                for(int i=0; i<6; i++){
                    float r = mSize.x/1.5f + std::sin(t*20+i)*6;
                    sf::CircleShape flame(r, 8); flame.setOrigin(r, r);
                    flame.setPosition(mPos.x - mVel.x*0.001f*i, mPos.y - mVel.y*0.001f*i);
                    flame.setFillColor(sf::Color(255, 60+i*20, 0, 150-i*20));
                    window.draw(flame);
                }
            }
            break;
        }
        case Element::Water: {
            if (type == 0 || type == 1) {
                // Water Arc/Bullet
                sf::CircleShape drop(mSize.x/2.5f); drop.setOrigin(drop.getRadius(), drop.getRadius());
                drop.setPosition(mPos); drop.setFillColor(sf::Color(100, 200, 255)); window.draw(drop);
                for(int i=0; i<4; i++){
                    float r = mSize.x/2 - i;
                    sf::CircleShape trail(r); trail.setOrigin(r, r);
                    trail.setPosition(mPos.x - mVel.x*0.004f*i, mPos.y - mVel.y*0.004f*i + std::sin(t*10+i)*3);
                    trail.setFillColor(sf::Color(40, 120, 220, 150-i*30)); window.draw(trail);
                }
            }
            else if (type == 2) {
                // Ice Wall (Block)
                for(int i=0; i<4; i++){
                    sf::ConvexShape ice; ice.setPointCount(4);
                    float h = mSize.y/2 + (i%2)*10;
                    ice.setPoint(0, {0, -h}); ice.setPoint(1, {8, 0});
                    ice.setPoint(2, {0, h}); ice.setPoint(3, {-8, 0});
                    ice.setPosition(mPos.x + (i-1.5f)*8, mPos.y);
                    ice.setFillColor(sf::Color(180, 240, 255, 220));
                    ice.setOutlineThickness(1); ice.setOutlineColor(sf::Color(100, 180, 255));
                    window.draw(ice);
                }
            }
            else if (type == 3) {
                // Tidal Wave (Ultimate)
                sf::CircleShape wave(mSize.x/1.5f); wave.setOrigin(wave.getRadius(), wave.getRadius());
                wave.setPosition(mPos); wave.setFillColor(sf::Color(30, 100, 200, 200)); window.draw(wave);
                for(int i=0; i<5; i++){
                    sf::CircleShape foam(mSize.x/3); foam.setOrigin(foam.getRadius(), foam.getRadius());
                    foam.setPosition(mPos.x + std::cos(t*8+i)*15, mPos.y - mSize.x/2 + std::sin(t*8+i)*10);
                    foam.setFillColor(sf::Color(220, 240, 255, 180)); window.draw(foam);
                }
            }
            break;
        }
        case Element::Earth: {
            if (type == 0 || type == 1 || type == 3) {
                // Boulder / Meteor (Ultimate is just bigger boulder)
                sf::ConvexShape rock; rock.setPointCount(6);
                float s = mSize.x/2;
                rock.setPoint(0, {0, -s}); rock.setPoint(1, {s*0.8f, -s*0.4f});
                rock.setPoint(2, {s, s*0.5f}); rock.setPoint(3, {0, s});
                rock.setPoint(4, {-s*0.9f, s*0.3f}); rock.setPoint(5, {-s*0.6f, -s*0.7f});
                rock.setPosition(mPos); rock.setRotation(t*150);
                rock.setFillColor(sf::Color(120, 90, 50));
                rock.setOutlineThickness(2); rock.setOutlineColor(sf::Color(60, 40, 20));
                window.draw(rock);
            }
            else if (type == 2) {
                // Earth Wall (Block)
                sf::RectangleShape wall({mSize.x, mSize.y}); wall.setOrigin(mSize.x/2, mSize.y/2);
                wall.setPosition(mPos); wall.setFillColor(sf::Color(100, 80, 50));
                wall.setOutlineThickness(2); wall.setOutlineColor(sf::Color(50, 40, 20));
                window.draw(wall);
                // Cracks
                sf::RectangleShape crack({mSize.x*0.8f, 2}); crack.setOrigin(mSize.x*0.4f, 1);
                crack.setPosition(mPos.x, mPos.y - 10); crack.setRotation(15);
                crack.setFillColor(sf::Color(40, 30, 15)); window.draw(crack);
                crack.setPosition(mPos.x, mPos.y + 15); crack.setRotation(-20); window.draw(crack);
            }
            break;
        }
        case Element::Metal: {
            if (type == 0 || type == 1) {
                // Spinning blade / knife (Dark iron with superheated glowing orange edge)
                sf::ConvexShape blade; blade.setPointCount(4);
                float s = mSize.x/2;
                blade.setPoint(0, {0, -s}); blade.setPoint(1, {s*0.4f, 0});
                blade.setPoint(2, {0, s}); blade.setPoint(3, {-s*0.4f, 0});
                blade.setPosition(mPos); blade.setRotation(t*500);
                blade.setFillColor(sf::Color(80, 80, 90)); // Dark iron
                blade.setOutlineThickness(2.5f); blade.setOutlineColor(sf::Color(255, 120, 0)); // Superheated orange edge
                window.draw(blade);
                // Heat distortion / inner glow
                sf::ConvexShape inner = blade;
                inner.setScale(0.5f, 0.5f);
                inner.setFillColor(sf::Color(255, 200, 0, 180));
                inner.setOutlineThickness(0);
                window.draw(inner);
            }
            else if (type == 2) {
                // Metal Dome (Block) - Republic City dark green/grey metal
                sf::CircleShape dome(mSize.x/2, 6); dome.setOrigin(mSize.x/2, mSize.x/2);
                dome.setPosition(mPos); dome.setFillColor(sf::Color(90, 105, 90));
                dome.setOutlineThickness(3); dome.setOutlineColor(sf::Color(50, 65, 50));
                window.draw(dome);
                // Heavy riveted lines
                for(int i=0; i<3; i++){
                    sf::RectangleShape line({mSize.x, 2}); line.setOrigin(mSize.x/2, 1);
                    line.setPosition(mPos); line.setRotation(i*60);
                    line.setFillColor(sf::Color(60, 70, 60)); window.draw(line);
                }
                // Shine reflection
                sf::CircleShape shine(mSize.x/4); shine.setOrigin(mSize.x/4, mSize.x/4);
                shine.setPosition(mPos.x - mSize.x/5, mPos.y - mSize.x/5);
                shine.setFillColor(sf::Color(255, 255, 255, 90)); window.draw(shine);
            }
            else if (type == 3) {
                // Heavy meteor chunk (Dark metal with burning trail)
                sf::ConvexShape chunk; chunk.setPointCount(5);
                float s = mSize.x/2;
                chunk.setPoint(0, {0, -s}); chunk.setPoint(1, {s, -s*0.2f});
                chunk.setPoint(2, {s*0.6f, s}); chunk.setPoint(3, {-s*0.8f, s*0.8f});
                chunk.setPoint(4, {-s, 0});
                chunk.setPosition(mPos); chunk.setRotation(t*200);
                chunk.setFillColor(sf::Color(60, 60, 70)); // Very dark metal
                chunk.setOutlineThickness(2.5f); chunk.setOutlineColor(sf::Color(255, 80, 0)); // Red hot edges
                window.draw(chunk);
                // Burning molten trail
                for(int i=0; i<3; i++){
                    sf::CircleShape fire(s - i*2); fire.setOrigin(s-i*2, s-i*2);
                    fire.setPosition(mPos.x - mVel.x*0.012f*(i+1), mPos.y - mVel.y*0.012f*(i+1));
                    fire.setFillColor(sf::Color(255, 150-i*50, 0, 180-i*40)); 
                    window.draw(fire);
                }
            }
            break;
        }
        case Element::Lightning: {
            if (type == 0 || type == 1 || type == 3) {
                // Lightning bolt (Pure Yellow/Gold)
                float bx = mPos.x, by = mPos.y;
                for(int i=0; i<5; i++){
                    float nx = bx + mVel.x*0.003f; 
                    float ny = by + mVel.y*0.003f + std::sin(t*40+i)*15; // More jagged
                    // Thick outer gold glow
                    sf::RectangleShape glow({18, 5}); glow.setOrigin(0, 2.5f);
                    glow.setPosition(bx, by);
                    glow.setRotation(std::atan2(ny-by, nx-bx)*180/3.14159f);
                    glow.setFillColor(sf::Color(255, 200, 0, 150));
                    window.draw(glow);
                    // Bright white-yellow inner core
                    sf::RectangleShape seg({15, 2}); seg.setOrigin(0, 1.f);
                    seg.setPosition(bx, by);
                    seg.setRotation(std::atan2(ny-by, nx-bx)*180/3.14159f);
                    seg.setFillColor(sf::Color(255, 255, 150));
                    window.draw(seg);
                    bx = nx; by = ny;
                }
                // Intense bright impact core
                sf::CircleShape core(mSize.x/2.0f); core.setOrigin(core.getRadius(), core.getRadius());
                core.setPosition(mPos); core.setFillColor(sf::Color(255, 255, 255, 220)); window.draw(core);
            }
            else if (type == 2) {
                // Thunder Field (Block) - Bright Yellow electrical web
                sf::CircleShape field(mSize.x/2); field.setOrigin(mSize.x/2, mSize.x/2);
                field.setPosition(mPos); field.setFillColor(sf::Color(255, 220, 0, 60));
                field.setOutlineThickness(3); field.setOutlineColor(sf::Color(255, 255, 0, 200));
                window.draw(field);
                // Wild snapping yellow arcs
                for(int i=0; i<4; i++){
                    float la = t*20 + i*3.14f/2.f;
                    sf::RectangleShape arc({mSize.x*0.9f, 3}); arc.setOrigin(mSize.x*0.45f, 1.5f);
                    arc.setPosition(mPos.x + std::cos(la)*8, mPos.y + std::sin(la)*8);
                    arc.setRotation(la*50); 
                    arc.setFillColor(sf::Color(255, 255, 150, 220));
                    window.draw(arc);
                }
            }
            break;
        }

        case Element::Plant: {
            if (type == 0 || type == 1) {
                // Vine/Leaf blade
                sf::ConvexShape leaf; leaf.setPointCount(4);
                float s = mSize.x/2;
                leaf.setPoint(0, {s, 0}); leaf.setPoint(1, {s*0.3f, -s*0.4f});
                leaf.setPoint(2, {-s, 0}); leaf.setPoint(3, {s*0.3f, s*0.4f});
                leaf.setPosition(mPos); leaf.setRotation(std::atan2(mVel.y, mVel.x)*180/3.14159f);
                leaf.setFillColor(sf::Color(80, 200, 60));
                leaf.setOutlineThickness(1.5f); leaf.setOutlineColor(sf::Color(40, 100, 30));
                window.draw(leaf);
                // Swirling leaves
                for(int i=0; i<3; i++){
                    sf::CircleShape sl(2); sl.setOrigin(2,2);
                    sl.setPosition(mPos.x - mVel.x*0.01f*i + std::cos(t*15+i)*8, mPos.y + std::sin(t*15+i)*8);
                    sl.setFillColor(sf::Color(120, 255, 80)); window.draw(sl);
                }
            }
            else if (type == 2) {
                // Plant Barrier (Vines)
                for(int i=0; i<5; i++){
                    float h = mSize.y/2 + std::sin(t*5+i)*5;
                    sf::RectangleShape vine({8, h}); vine.setOrigin(4, h);
                    vine.setPosition(mPos.x + (i-2)*6, mPos.y + mSize.y/2);
                    vine.setFillColor(sf::Color(50, 120, 40));
                    vine.setOutlineThickness(1); vine.setOutlineColor(sf::Color(30, 80, 20));
                    window.draw(vine);
                }
                // Thorns
                for(int i=0; i<4; i++){
                    sf::CircleShape thorn(3, 3); thorn.setOrigin(3,3);
                    thorn.setPosition(mPos.x + std::sin(t*2+i)*10, mPos.y - 10 + i*15);
                    thorn.setFillColor(sf::Color(100, 200, 50)); window.draw(thorn);
                }
            }
            else if (type == 3) {
                // Giant root spike
                sf::ConvexShape root; root.setPointCount(3);
                root.setPoint(0, {0, -mSize.y}); root.setPoint(1, {mSize.x/2, 0}); root.setPoint(2, {-mSize.x/2, 0});
                root.setPosition(mPos.x, mPos.y + mSize.y/2);
                root.setFillColor(sf::Color(90, 70, 40));
                root.setOutlineThickness(2); root.setOutlineColor(sf::Color(50, 40, 20));
                window.draw(root);
                // Vines wrapping the root
                for(int i=0; i<4; i++){
                    sf::RectangleShape wrap({mSize.x*0.6f, 4}); wrap.setOrigin(mSize.x*0.3f, 2);
                    wrap.setPosition(mPos.x, mPos.y - i*15);
                    wrap.setRotation(15 - i*10);
                    wrap.setFillColor(sf::Color(70, 160, 50)); window.draw(wrap);
                }
            }
            break;
        }
        case Element::Ice: {
            if (type == 0 || type == 1) {
                // Ice Shard
                sf::ConvexShape shard; shard.setPointCount(4);
                float s = mSize.x/2;
                shard.setPoint(0, {s, 0}); shard.setPoint(1, {-s*0.2f, -s*0.5f});
                shard.setPoint(2, {-s, 0}); shard.setPoint(3, {-s*0.2f, s*0.5f});
                shard.setPosition(mPos); shard.setRotation(std::atan2(mVel.y, mVel.x)*180/3.14159f);
                shard.setFillColor(sf::Color(180, 240, 255, 200));
                shard.setOutlineThickness(1.5f); shard.setOutlineColor(sf::Color(100, 200, 255));
                window.draw(shard);
                // Frost trail
                for(int i=0; i<3; i++){
                    sf::CircleShape frost(s/2 - i); frost.setOrigin(s/2-i, s/2-i);
                    frost.setPosition(mPos.x - mVel.x*0.005f*i, mPos.y - mVel.y*0.005f*i);
                    frost.setFillColor(sf::Color(255, 255, 255, 150)); window.draw(frost);
                }
            }
            else if (type == 2) {
                // Solid Ice Block
                sf::RectangleShape block({mSize.x, mSize.y}); block.setOrigin(mSize.x/2, mSize.y/2);
                block.setPosition(mPos);
                block.setFillColor(sf::Color(160, 220, 255, 180));
                block.setOutlineThickness(3); block.setOutlineColor(sf::Color(255, 255, 255, 200));
                window.draw(block);
                // Inner reflection
                sf::RectangleShape ref({mSize.x*0.4f, mSize.y*0.8f}); ref.setOrigin(mSize.x*0.2f, mSize.y*0.4f);
                ref.setPosition(mPos.x - mSize.x*0.2f, mPos.y);
                ref.setFillColor(sf::Color(255, 255, 255, 80)); window.draw(ref);
            }
            else if (type == 3) {
                // Giant Ice Spike Eruption
                sf::ConvexShape spike; spike.setPointCount(4);
                float h = mSize.y; float w = mSize.x/2;
                spike.setPoint(0, {0, -h}); spike.setPoint(1, {w, 0});
                spike.setPoint(2, {0, h*0.2f}); spike.setPoint(3, {-w, 0});
                spike.setPosition(mPos.x, mPos.y + h/2);
                spike.setRotation(std::atan2(mVel.y, mVel.x)*180/3.14159f + 90);
                spike.setFillColor(sf::Color(140, 210, 255));
                spike.setOutlineThickness(2); spike.setOutlineColor(sf::Color(200, 240, 255));
                window.draw(spike);
                // Shimmer
                sf::ConvexShape shim = spike; shim.setScale(0.5f, 0.8f);
                shim.setFillColor(sf::Color(255, 255, 255, 150)); shim.setOutlineThickness(0);
                window.draw(shim);
            }
            break;
        }
        case Element::Sword: {
            if (type == 0 || type == 1) {
                // Space Sword / Boomerang (Crescent shape with glowing trails)
                sf::ConvexShape blade; blade.setPointCount(4);
                float s = mSize.x/2;
                blade.setPoint(0, {0, -s}); blade.setPoint(1, {s*0.4f, 0});
                blade.setPoint(2, {0, s}); blade.setPoint(3, {-s*0.4f, 0});
                blade.setPosition(mPos); blade.setRotation(t*(type==0?0:800)); // Boomerang spins
                blade.setFillColor(sf::Color(180,200,220));
                blade.setOutlineThickness(2); blade.setOutlineColor(sf::Color(100,120,150));
                window.draw(blade);
                // Glowing trail
                for(int i=0; i<4; i++){
                    sf::CircleShape tr(s/2 - i); tr.setOrigin(s/2-i, s/2-i);
                    tr.setPosition(mPos.x - mVel.x*0.005f*i, mPos.y - mVel.y*0.005f*i);
                    tr.setFillColor(sf::Color(200, 220, 255, 100)); window.draw(tr);
                }
            }
            else if (type == 2) {
                // Sword Parry Block
                sf::RectangleShape sw({6, mSize.y}); sw.setOrigin(3, mSize.y/2);
                sw.setPosition(mPos); sw.setRotation(15);
                sw.setFillColor(sf::Color(150,170,190));
                sw.setOutlineThickness(1.5f); sw.setOutlineColor(sf::Color(255,255,255));
                window.draw(sw);
                // Spark clash
                sf::CircleShape spark(mSize.x/2); spark.setOrigin(mSize.x/2, mSize.x/2);
                spark.setPosition(mPos); spark.setFillColor(sf::Color(255,255,200,150));
                window.draw(spark);
            }
            else if (type == 3) {
                // Meteor Sword Barrage (Giant glowing space swords)
                sf::ConvexShape ms; ms.setPointCount(4);
                float s = mSize.x/2;
                ms.setPoint(0, {s, 0}); ms.setPoint(1, {0, -s*0.2f});
                ms.setPoint(2, {-s, 0}); ms.setPoint(3, {0, s*0.2f});
                ms.setPosition(mPos); ms.setRotation(std::atan2(mVel.y, mVel.x)*180/3.14159f);
                ms.setFillColor(sf::Color(255,255,255));
                ms.setOutlineThickness(3); ms.setOutlineColor(sf::Color(100,150,255));
                window.draw(ms);
                // Heavy light trail
                sf::RectangleShape trail({mSize.x*2, 6}); trail.setOrigin(mSize.x*2, 3);
                trail.setPosition(mPos); trail.setRotation(ms.getRotation());
                trail.setFillColor(sf::Color(150, 200, 255, 180)); window.draw(trail);
            }
            break;
        }
        case Element::Fan: {
            if (type == 0 || type == 1) {
                // Golden Kyoshi Fan
                sf::ConvexShape fan; fan.setPointCount(5);
                float s = mSize.x/2;
                fan.setPoint(0, {0, s*0.5f}); fan.setPoint(1, {-s, -s});
                fan.setPoint(2, {0, -s*1.2f}); fan.setPoint(3, {s, -s});
                fan.setPoint(4, {0, s*0.5f});
                fan.setPosition(mPos); fan.setRotation(t*(type==0?400:1000));
                fan.setFillColor(sf::Color(255,200,80));
                fan.setOutlineThickness(2); fan.setOutlineColor(sf::Color(180,120,40));
                window.draw(fan);
                // Golden gust
                sf::CircleShape gust(s*0.8f); gust.setOrigin(s*0.8f, s*0.8f);
                gust.setPosition(mPos.x - mVel.x*0.01f, mPos.y - mVel.y*0.01f);
                gust.setFillColor(sf::Color(255,240,150,120)); window.draw(gust);
            }
            else if (type == 2) {
                // Golden Shield
                sf::CircleShape sh(mSize.x/2, 6); sh.setOrigin(mSize.x/2, mSize.x/2);
                sh.setPosition(mPos); sh.setFillColor(sf::Color(255,200,80,180));
                sh.setOutlineThickness(3); sh.setOutlineColor(sf::Color(255,255,200));
                window.draw(sh);
                sf::CircleShape in(mSize.x/3, 6); in.setOrigin(mSize.x/3, mSize.x/3);
                in.setPosition(mPos); in.setFillColor(sf::Color(255,240,150,150));
                window.draw(in);
            }
            else if (type == 3) {
                // Massive Golden Fan
                sf::ConvexShape fan; fan.setPointCount(10);
                float s = mSize.x/2;
                fan.setPoint(0, {0, s*0.2f});
                for(int i=0; i<=8; i++){
                    float a = 3.14159f + (i/8.f)*3.14159f;
                    fan.setPoint(i+1, {std::cos(a)*s, std::sin(a)*s});
                }
                fan.setPosition(mPos); fan.setRotation(std::atan2(mVel.y, mVel.x)*180/3.14159f - 90 + t*100);
                fan.setFillColor(sf::Color(255,220,100));
                fan.setOutlineThickness(4); fan.setOutlineColor(sf::Color(200,100,20));
                window.draw(fan);
            }
            break;
        }
    }
}

sf::FloatRect Attack::getHitbox() const {
    return sf::FloatRect(mPos.x-mSize.x/2, mPos.y-mSize.y/2, mSize.x, mSize.y);
}
