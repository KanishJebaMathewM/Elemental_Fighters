#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "CartoonRenderer.h"

class Fighter;
class ParticleSystem;
class Attack;

enum class GameState { TitleScreen, CharacterSelect, Playing, GameOver };

class Game {
public:
    Game();
    ~Game();
    void run();
    void addAttack(std::unique_ptr<Attack> attack);

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();

    void renderTitleScreen();
    void renderCharacterSelect();
    void renderPlaying();

    void updatePlaying(sf::Time deltaTime);
    void updateCharacterSelect(sf::Time deltaTime);
    void updateCPU(sf::Time deltaTime);
    void resetGame();

    sf::RenderWindow mWindow;
    GameState mState;
    CartoonRenderer mCartoon;
    sf::Clock mGameClock;

    std::unique_ptr<Fighter> mPlayer1;
    std::unique_ptr<Fighter> mPlayer2;
    std::vector<std::unique_ptr<Attack>> mAttacks;
    std::unique_ptr<ParticleSystem> mParticleSystem;

    int mP1SelectIndex, mP2SelectIndex;
    std::vector<sf::Color> mCharacterColors;
    float mShakeTime, mShakeMagnitude;
    sf::View mDefaultView;
    sf::Font mFont;
    bool mFontLoaded;

    // Title screen
    int mMenuChoice; // 0 = 1 Player, 1 = 2 Player
    bool mSinglePlayer;

    // CPU AI
    float mCpuActionTimer;
    float mCpuSpecialTimer;

    // Round system
    int mP1Wins, mP2Wins;
    int mRound;
    float mFightTimer;
    float mRoundEndTimer;
};

#endif
