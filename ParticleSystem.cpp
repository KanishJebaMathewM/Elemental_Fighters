#include "ParticleSystem.h"
#include <cstdlib>
#include <cmath>

ParticleSystem::ParticleSystem() {}

void ParticleSystem::emit(sf::Vector2f position, int count, sf::Color color) {
    for(int i=0;i<count;i++){
        Particle p;
        p.position=position;
        float vx=(rand()%100-50)*2.5f;
        float vy=(rand()%100-50)*2.5f - 30.f; // Bias upward
        p.velocity=sf::Vector2f(vx,vy);
        p.color=color;
        p.maxLifetime=0.4f+(rand()%60)/100.f; // 0.4 to 1.0s
        p.lifetime=p.maxLifetime;
        p.size=3.f+(rand()%5);
        mParticles.push_back(p);
    }
}

void ParticleSystem::update(sf::Time deltaTime) {
    float dt=deltaTime.asSeconds();
    for(auto it=mParticles.begin();it!=mParticles.end();){
        it->lifetime-=dt;
        if(it->lifetime<=0){
            it=mParticles.erase(it);
        } else {
            it->position+=it->velocity*dt;
            it->velocity.y+=120.f*dt; // Slight gravity on particles
            float ratio=it->lifetime/it->maxLifetime;
            it->color.a=(sf::Uint8)(255*ratio);
            it->size*=0.997f; // Shrink slightly
            ++it;
        }
    }
    // Cap particles to avoid slowdown
    while(mParticles.size()>800) mParticles.erase(mParticles.begin());
}

void ParticleSystem::render(sf::RenderWindow& window) {
    for(const auto& p:mParticles){
        mShape.setSize(sf::Vector2f(p.size,p.size));
        mShape.setOrigin(p.size/2,p.size/2);
        mShape.setPosition(p.position);
        mShape.setFillColor(p.color);
        window.draw(mShape);
    }
}
