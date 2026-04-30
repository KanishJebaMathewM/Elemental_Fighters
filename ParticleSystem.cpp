#include "ParticleSystem.h"
#include <cstdlib>
#include <cmath>

ParticleSystem::ParticleSystem() {
    mCircle.setRadius(1.f);
    mSquare.setSize(sf::Vector2f(1.f, 1.f));
}

void ParticleSystem::emit(sf::Vector2f position, int count, sf::Color color) {
    emitEffect(position, count, color, 0, 1.f);
}

void ParticleSystem::emitEffect(sf::Vector2f position, int count, sf::Color color, int type, float speedMult) {
    for(int i=0; i<count; i++) {
        Particle p;
        p.position = position;
        float angle = (rand() % 360) * 3.14159f / 180.f;
        float speed = (50 + rand() % 200) * speedMult;
        p.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed - 50.f);
        p.color = color;
        p.maxLifetime = 0.3f + (rand() % 40) / 100.f;
        p.lifetime = p.maxLifetime;
        p.size = 2.f + (rand() % 4);
        p.rotation = rand() % 360;
        p.rotationVel = (rand() % 400 - 200);
        p.type = type;
        mParticles.push_back(p);
    }
}

void ParticleSystem::update(sf::Time deltaTime) {
    float dt = deltaTime.asSeconds();
    for(auto it = mParticles.begin(); it != mParticles.end(); ) {
        it->lifetime -= dt;
        if(it->lifetime <= 0) {
            it = mParticles.erase(it);
        } else {
            it->position += it->velocity * dt;
            it->velocity.y += 180.f * dt; // Gravity
            it->rotation += it->rotationVel * dt;
            
            float ratio = it->lifetime / it->maxLifetime;
            it->color.a = (sf::Uint8)(255 * ratio * ratio); // Smooth fade out
            it->size *= 0.985f; // Shrink
            ++it;
        }
    }
    // Cap particles
    while(mParticles.size() > 1500) mParticles.erase(mParticles.begin());
}

void ParticleSystem::render(sf::RenderWindow& window) {
    for(const auto& p : mParticles) {
        if(p.type == 0) { // Square
            mSquare.setSize(sf::Vector2f(p.size, p.size));
            mSquare.setOrigin(p.size / 2, p.size / 2);
            mSquare.setPosition(p.position);
            mSquare.setFillColor(p.color);
            mSquare.setRotation(p.rotation);
            window.draw(mSquare);
        } else { // Circle
            mCircle.setRadius(p.size / 2.f);
            mCircle.setOrigin(p.size / 2, p.size / 2);
            mCircle.setPosition(p.position);
            mCircle.setFillColor(p.color);
            window.draw(mCircle);
        }
    }
}
