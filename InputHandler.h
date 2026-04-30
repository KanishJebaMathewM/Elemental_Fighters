#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <SFML/Window/Keyboard.hpp>

// P1: WASD move | J=Attack | K=Block | L=Special | I=Ultimate
// P2: Arrows move | Num1=Attack | Num2=Block | Num3=Special | Num0=Ultimate
struct PlayerControls {
    sf::Keyboard::Key up, down, left, right;
    sf::Keyboard::Key normalAttack;
    sf::Keyboard::Key block;
    sf::Keyboard::Key special1;  // Element special
    sf::Keyboard::Key special3;  // Ultimate
};

class InputHandler {
public:
    static PlayerControls getPlayer1Controls() {
        return { sf::Keyboard::W, sf::Keyboard::S, sf::Keyboard::A, sf::Keyboard::D,
                 sf::Keyboard::J, sf::Keyboard::K, sf::Keyboard::L, sf::Keyboard::I };
    }
    static PlayerControls getPlayer2Controls() {
        return { sf::Keyboard::Up, sf::Keyboard::Down, sf::Keyboard::Left, sf::Keyboard::Right,
                 sf::Keyboard::Numpad1, sf::Keyboard::Numpad2, sf::Keyboard::Numpad3, sf::Keyboard::Numpad0 };
    }
};

#endif
