#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <SFML/Graphics.hpp>
#include <vector>

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float lifetime;
    float maxLifetime;
    float size;
    float rotation;
    float rotationVel;
    int type; // 0=square, 1=circle, 2=star, 3=spark
};

class ParticleSystem {
public:
    ParticleSystem();
    
    void emit(sf::Vector2f position, int count, sf::Color color);
    void emitEffect(sf::Vector2f position, int count, sf::Color color, int type, float speedMult=1.f);
    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window);

private:
    std::vector<Particle> mParticles;
    sf::RectangleShape mSquare;
    sf::CircleShape mCircle;
};

#endif // PARTICLESYSTEM_H
