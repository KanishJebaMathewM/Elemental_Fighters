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
};

class ParticleSystem {
public:
    ParticleSystem();
    
    void emit(sf::Vector2f position, int count, sf::Color color);
    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window);

private:
    std::vector<Particle> mParticles;
    sf::RectangleShape mShape; // for rendering
};

#endif // PARTICLESYSTEM_H
