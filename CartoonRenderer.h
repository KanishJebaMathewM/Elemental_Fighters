#ifndef CARTOONRENDERER_H
#define CARTOONRENDERER_H

#include <SFML/Graphics.hpp>
#include <cmath>

enum class Element;
enum class AnimationState;

class CartoonRenderer {
public:
    CartoonRenderer();

    void drawCharacter(sf::RenderWindow& w, sf::Vector2f pos, Element e,
                       AnimationState anim, int facing, float t, float flash,
                       float hpRatio, bool airBall, float airBallTimer);

    void drawElementalAura(sf::RenderWindow& w, sf::Vector2f pos, Element e, float t);
    void drawAirBall(sf::RenderWindow& w, sf::Vector2f pos, float t);

    void drawPortrait(sf::RenderWindow& w, sf::Vector2f pos, Element e,
                      bool selected, int pIdx, float t);

    void drawBackground(sf::RenderWindow& w, float t);
    void drawHPBar(sf::RenderWindow& w, sf::Vector2f pos, float hpR, float enR,
                   Element e, bool rightAligned);

private:
    // Anime character parts per element
    void drawAirBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl);
    void drawFireBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl);
    void drawWaterBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl);
    void drawEarthBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl);
    void drawMetalBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl);
    void drawLightningBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl);
    void drawPlantBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl);
    void drawIceBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl);
    void drawSwordBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl);
    void drawFanBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl);

    void drawOutfit(sf::RenderWindow& w, sf::Vector2f pos, Element e, float bob, float fl);
    void drawArm(sf::RenderWindow& w, sf::Vector2f pos, Element e, int facing,
                 float angle, bool attacking, float fl);
    void drawLeg(sf::RenderWindow& w, sf::Vector2f pos, Element e, float angle,
                 float xOff, float fl);

    sf::Color bodyCol(Element e) const;
    sf::Color darkCol(Element e) const;
    sf::Color lightCol(Element e) const;
    sf::Color accentCol(Element e) const;
    sf::Color flash(sf::Color c, float fl) const;

    void drawCloud(sf::RenderWindow& w, float x, float y, float s);
    void drawMountain(sf::RenderWindow& w, float x, float by, float wd, float ht, sf::Color c);
    void drawTree(sf::RenderWindow& w, float x, float by, float s);
};

#endif
