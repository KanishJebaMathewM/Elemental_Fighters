#include "CartoonRenderer.h"
#include "CartoonBg.h"
#include "Fighter.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

CartoonRenderer::CartoonRenderer(){}

// =====================================================================
//  COLOUR HELPERS
// =====================================================================
sf::Color CartoonRenderer::bodyCol(Element e) const {
    switch(e){
        case Element::Air:       return sf::Color(255,210,160);
        case Element::Fire:      return sf::Color(245,205,155);
        case Element::Water:     return sf::Color(150,105,75);
        case Element::Earth:     return sf::Color(210,175,130);
        case Element::Metal:     return sf::Color(200,200,210);
        case Element::Lightning: return sf::Color(240,220,180);
        case Element::Plant:     return sf::Color(100,200,80);
        case Element::Ice:       return sf::Color(180,240,255);
        case Element::Sword:     return sf::Color(150,170,190);
        case Element::Fan:       return sf::Color(255,245,230);
    } return sf::Color::White;
}
sf::Color CartoonRenderer::darkCol(Element e) const {
    switch(e){
        case Element::Air:       return sf::Color(230,130,30);
        case Element::Fire:      return sf::Color(140,20,20);
        case Element::Water:     return sf::Color(30,60,160);
        case Element::Earth:     return sf::Color(50,120,40);
        case Element::Metal:     return sf::Color(80,80,95);
        case Element::Lightning: return sf::Color(90,30,160);
        case Element::Plant:     return sf::Color(30,80,20);
        case Element::Ice:       return sf::Color(60,100,160);
        case Element::Sword:     return sf::Color(80,100,120);
        case Element::Fan:       return sf::Color(160,120,40);
    } return sf::Color::White;
}
sf::Color CartoonRenderer::lightCol(Element e) const {
    switch(e){
        case Element::Air:       return sf::Color(255,230,100);
        case Element::Fire:      return sf::Color(255,100,30);
        case Element::Water:     return sf::Color(100,180,255);
        case Element::Earth:     return sf::Color(130,200,80);
        case Element::Metal:     return sf::Color(200,200,220);
        case Element::Lightning: return sf::Color(255,220,50);
        case Element::Plant:     return sf::Color(150,255,100);
        case Element::Ice:       return sf::Color(220,255,255);
        case Element::Sword:     return sf::Color(200,220,255);
        case Element::Fan:       return sf::Color(255,240,150);
    } return sf::Color::White;
}
sf::Color CartoonRenderer::accentCol(Element e) const {
    switch(e){
        case Element::Air:       return sf::Color(100,180,240);
        case Element::Fire:      return sf::Color(255,200,0);
        case Element::Water:     return sf::Color(200,230,255);
        case Element::Earth:     return sf::Color(139,90,43);
        case Element::Metal:     return sf::Color(170,170,180);
        case Element::Lightning: return sf::Color(255,255,255);
        case Element::Plant:     return sf::Color(50,150,30);
        case Element::Ice:       return sf::Color(255,255,255);
        case Element::Sword:     return sf::Color(100,120,150);
        case Element::Fan:       return sf::Color(200,80,40);
    } return sf::Color::White;
}
sf::Color CartoonRenderer::flash(sf::Color c, float f) const {
    return f>0 ? sf::Color(255,255,255,c.a) : c;
}

// =====================================================================
//  UTILITY DRAWING HELPERS
// =====================================================================
static void drawEllipse(sf::RenderWindow& w, float x, float y, float rx, float ry,
                        sf::Color fill, sf::Color outline=sf::Color::Transparent, float ot=0, int pts=32) {
    sf::CircleShape s(rx, pts);
    s.setOrigin(rx, rx);
    s.setScale(1.f, ry/rx);
    s.setPosition(x, y);
    s.setFillColor(fill);
    if(ot>0){ s.setOutlineThickness(ot); s.setOutlineColor(outline); }
    w.draw(s);
}

static void drawRoundRect(sf::RenderWindow& w, float x, float y, float ww, float hh,
                          float r, sf::Color fill) {
    // Approximate rounded rect with overlapping rects + corner circles
    sf::RectangleShape h({ww, hh-2*r}); h.setOrigin(ww/2, (hh-2*r)/2);
    h.setPosition(x, y); h.setFillColor(fill); w.draw(h);
    sf::RectangleShape v({ww-2*r, hh}); v.setOrigin((ww-2*r)/2, hh/2);
    v.setPosition(x, y); v.setFillColor(fill); w.draw(v);
    for(int i=-1;i<=1;i+=2) for(int j=-1;j<=1;j+=2) {
        sf::CircleShape c(r); c.setOrigin(r,r);
        c.setPosition(x+i*(ww/2-r), y+j*(hh/2-r));
        c.setFillColor(fill); w.draw(c);
    }
}

// Big expressive anime eye
static void drawBigEye(sf::RenderWindow& w, float x, float y, float rx, float ry,
                       sf::Color irisCol, bool highlight=true) {
    // White sclera
    drawEllipse(w, x, y, rx, ry, sf::Color(250,250,255), sf::Color(40,20,20), 1.2f);
    // Iris
    drawEllipse(w, x, y+1, rx*0.65f, ry*0.78f, irisCol);
    // Dark pupil
    drawEllipse(w, x, y+1.5f, rx*0.32f, ry*0.42f, sf::Color(15,10,10));
    // Catchlight
    if(highlight){
        sf::CircleShape hl(rx*0.25f); hl.setOrigin(rx*0.25f, rx*0.25f);
        hl.setPosition(x-rx*0.2f, y-ry*0.3f);
        hl.setFillColor(sf::Color(255,255,255,220)); w.draw(hl);
    }
    // Top eyelid line
    sf::RectangleShape lid({rx*2.2f, 1.5f}); lid.setOrigin(rx*1.1f, 0.75f);
    lid.setPosition(x, y-ry*0.85f); lid.setFillColor(sf::Color(30,15,15,200)); w.draw(lid);
}

// Draw a pair of eyes centered on (cx, cy)
static void drawEyePair(sf::RenderWindow& w, float cx, float cy, float spread,
                        float rx, float ry, sf::Color irisCol, int facing) {
    float fo = facing * 2.f;
    drawBigEye(w, cx - spread + fo, cy, rx, ry, irisCol);
    drawBigEye(w, cx + spread + fo, cy, rx, ry, irisCol);
}

// Eyebrow
static void drawBrow(sf::RenderWindow& w, float x, float y, float ww, sf::Color c, float angle=0) {
    sf::RectangleShape b({ww, 2.5f}); b.setOrigin(ww/2, 1.25f);
    b.setPosition(x, y); b.setRotation(angle); b.setFillColor(c); w.draw(b);
}

// Simple smile / frown
static void drawMouth(sf::RenderWindow& w, float x, float y, float ww, bool smile, sf::Color c) {
    if(smile) {
        // Curved up = 3 points arc approximation
        for(int i=0;i<5;i++){
            float a = -0.6f + i*0.3f;
            float px = x + a/0.6f * (ww/2);
            float py = y - smile * 3.f * std::cos(a*1.4f) + 3.f;
            sf::CircleShape d(1.5f); d.setOrigin(1.5f,1.5f); d.setPosition(px, py);
            d.setFillColor(c); w.draw(d);
        }
    } else {
        sf::RectangleShape mo({ww, 2.5f}); mo.setOrigin(ww/2, 1.25f);
        mo.setPosition(x, y); mo.setFillColor(c); w.draw(mo);
    }
}

// Hair tuft: convex spike
static void drawSpike(sf::RenderWindow& w, float bx, float by, float tx, float ty, float hw, sf::Color c) {
    sf::ConvexShape s; s.setPointCount(3);
    s.setPoint(0, {bx-hw, by}); s.setPoint(1, {tx, ty}); s.setPoint(2, {bx+hw, by});
    s.setFillColor(c); w.draw(s);
}

static void drawHeldSword(sf::RenderWindow& w, sf::Vector2f pos, int facing, float angle, float fl) {
    sf::Color steel = sf::Color(220, 230, 245);
    sf::Color edge = sf::Color(100, 120, 150);
    sf::Color glow = sf::Color(120, 180, 255, 160);

    sf::RectangleShape blade({28, 4});
    blade.setOrigin(4, 2);
    blade.setPosition(pos);
    blade.setRotation(angle);
    blade.setFillColor(steel);
    blade.setOutlineThickness(1.2f);
    blade.setOutlineColor(edge);
    w.draw(blade);

    sf::RectangleShape guard({8, 2});
    guard.setOrigin(4, 1);
    guard.setPosition(pos.x - std::cos(angle * (float)M_PI / 180.f) * 2.f,
                      pos.y - std::sin(angle * (float)M_PI / 180.f) * 2.f);
    guard.setRotation(angle + 90.f);
    guard.setFillColor(sf::Color(170, 190, 210));
    w.draw(guard);

    sf::RectangleShape hilt({10, 3});
    hilt.setOrigin(5, 1.5f);
    hilt.setPosition(pos.x - std::cos(angle * (float)M_PI / 180.f) * 7.f,
                     pos.y - std::sin(angle * (float)M_PI / 180.f) * 7.f);
    hilt.setRotation(angle);
    hilt.setFillColor(sf::Color(80, 90, 110));
    w.draw(hilt);

    if(fl <= 0.2f) {
        sf::CircleShape shine(5.f);
        shine.setOrigin(5.f, 5.f);
        shine.setPosition(pos.x + facing * 3.f, pos.y - 2.f);
        shine.setFillColor(glow);
        w.draw(shine);
    }
}

static void drawHeldFan(sf::RenderWindow& w, sf::Vector2f pos, int facing, float angle, float fl) {
    sf::Color gold = sf::Color(255, 210, 95);
    sf::Color edge = sf::Color(170, 110, 40);
    sf::Color cloth = sf::Color(255, 240, 185, 220);

    sf::ConvexShape fan; fan.setPointCount(6);
    fan.setPoint(0, {0, 12});
    fan.setPoint(1, {-10, -8});
    fan.setPoint(2, {-2, -16});
    fan.setPoint(3, {8, -16});
    fan.setPoint(4, {16, -8});
    fan.setPoint(5, {0, 12});
    fan.setOrigin(0, 0);
    fan.setPosition(pos);
    fan.setRotation(angle);
    fan.setFillColor(gold);
    fan.setOutlineThickness(1.2f);
    fan.setOutlineColor(edge);
    w.draw(fan);

    for(int i=0; i<4; ++i){
        float x = pos.x + (i - 1.5f) * 4.5f * facing;
        sf::RectangleShape rib({14.f, 1.5f});
        rib.setOrigin(0.f, 0.75f);
        rib.setPosition(x, pos.y + 2.f - i * 2.f);
        rib.setRotation(angle + facing * (8.f - i * 3.f));
        rib.setFillColor(cloth);
        w.draw(rib);
    }
}

// =====================================================================
//  CHARACTER HEAD DRAWING — PORTRAIT QUALITY
//  'p' is the CENTER of the head. 's' is a scale factor (1=normal fight, 1.6=portrait).
// =====================================================================

void CartoonRenderer::drawAirBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 19;
    sf::Color skin = flash(bodyCol(Element::Air), fl);
    sf::Color outline = flash(sf::Color(180,130,60), fl);

    // Head
    drawEllipse(w, hp.x, hp.y, r, r+1, skin, outline, 2.f);
    // Blue arrow tattoo (pointing down forehead) — wider, more visible
    sf::ConvexShape arr; arr.setPointCount(4);
    arr.setPoint(0,{0,-r-1}); arr.setPoint(1,{-7,-r+9}); arr.setPoint(2,{0,-r+5}); arr.setPoint(3,{7,-r+9});
    arr.setPosition(hp); arr.setFillColor(flash(sf::Color(60,155,230),fl)); w.draw(arr);
    // Arrow shaft going back
    sf::RectangleShape al({3, 10}); al.setOrigin(1.5f, 0); al.setPosition(hp.x, hp.y-r);
    al.setFillColor(flash(sf::Color(60,155,230),fl)); w.draw(al);
    // Cheek blush
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*10, hp.y+5, 5, 3, sf::Color(255,170,130,80));
    // Eyes
    drawEyePair(w, hp.x, hp.y-2, 7, 6, 4, flash(sf::Color(130,130,145),fl), face);
    // Eyebrows — gentle monk look
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*7, hp.y-8, 9, flash(sf::Color(90,60,30),fl), 0);
    // Mouth — calm smile
    drawMouth(w, hp.x+face*2, hp.y+9, 6, true, sf::Color(160,80,70));
    // Ears
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*(r-1), hp.y+2, 4, 5, skin, outline, 1.f);
}

void CartoonRenderer::drawFireBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 18;
    sf::Color skin = flash(bodyCol(Element::Fire), fl);
    sf::Color hair = flash(sf::Color(25,15,15), fl);

    // Pointed topknot base
    sf::ConvexShape topknot; topknot.setPointCount(5);
    topknot.setPoint(0,{-r+4,-2}); topknot.setPoint(1,{-5,-r-12}); topknot.setPoint(2,{0,-r-18});
    topknot.setPoint(3,{5,-r-12}); topknot.setPoint(4,{r-4,-2});
    topknot.setPosition(hp); topknot.setFillColor(hair); w.draw(topknot);
    // Side swept hair
    for(int i=-1;i<=1;i+=2){
        sf::ConvexShape sh; sh.setPointCount(4);
        sh.setPoint(0,{(float)(i*(r-2)),0}); sh.setPoint(1,{(float)(i*(r+5)),-8});
        sh.setPoint(2,{(float)(i*(r+3)),-16}); sh.setPoint(3,{(float)(i*(r-5)),-14});
        sh.setPosition(hp); sh.setFillColor(hair); w.draw(sh);
    }
    // Head face
    drawEllipse(w, hp.x, hp.y, r, r+1, skin, flash(sf::Color(160,120,80),fl), 2.f);
    // Gold Fire Nation crown top
    sf::ConvexShape crown; crown.setPointCount(5);
    crown.setPoint(0,{-8,-r-3}); crown.setPoint(1,{-4,-r-8}); crown.setPoint(2,{0,-r-11});
    crown.setPoint(3,{4,-r-8}); crown.setPoint(4,{8,-r-3});
    crown.setPosition(hp); crown.setFillColor(flash(sf::Color(255,200,40),fl)); w.draw(crown);

    // Eyes — gold/amber, intense
    drawEyePair(w, hp.x, hp.y-2, 7, 6, 4.5f, flash(sf::Color(190,140,0),fl), face);
    // Angry brows
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*7, hp.y-9, 9, hair, i*-8.f);
    // Zuko scar (left side = -1 facing)
    sf::ConvexShape scar; scar.setPointCount(4);
    scar.setPoint(0,{-13,-7}); scar.setPoint(1,{-6,-1}); scar.setPoint(2,{-4,1}); scar.setPoint(3,{-11,-4});
    scar.setPosition(hp); scar.setFillColor(flash(sf::Color(200,80,60,200),fl)); w.draw(scar);
    // Tight serious mouth
    drawMouth(w, hp.x+face*3, hp.y+10, 8, false, sf::Color(110,50,40));
    // Ears
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*(r-1), hp.y+2, 4, 5, skin);
}

void CartoonRenderer::drawWaterBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 18;
    sf::Color skin = flash(bodyCol(Element::Water), fl);
    sf::Color hairCol = flash(sf::Color(45,25,15), fl);

    // Hair back layer (thick volume)
    drawEllipse(w, hp.x, hp.y+2, r+3, r+4, hairCol);
    // Face
    drawEllipse(w, hp.x, hp.y, r, r+1, skin, flash(sf::Color(100,70,45),fl), 2.f);
    // Hair loopies — Katara's signature loops
    for(int i=-1;i<=1;i+=2){
        // Loop tube
        drawEllipse(w, hp.x+i*(r+3), hp.y, 5, 8, hairCol);
        // Blue Water Tribe bead
        drawEllipse(w, hp.x+i*(r+3), hp.y+10, 3, 3, flash(sf::Color(50,130,220),fl));
    }
    // Hair part line on top
    sf::RectangleShape part({2, 10}); part.setOrigin(1,0);
    part.setPosition(hp.x, hp.y-r); part.setFillColor(flash(sf::Color(30,15,10,140),fl)); w.draw(part);

    // Eyes — blue, warm and kind
    drawEyePair(w, hp.x, hp.y-2, 7, 6.5f, 5, flash(sf::Color(40,90,195),fl), face);
    // Gentle brows
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*7, hp.y-10, 9, hairCol, 0);
    // Cheek blush
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*9, hp.y+5, 5, 3, sf::Color(220,140,110,90));
    // Gentle smile
    drawMouth(w, hp.x+face*2, hp.y+10, 7, true, sf::Color(140,70,60));
    // Ears under hair
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*(r-1), hp.y+2, 4, 5, skin);
}

void CartoonRenderer::drawEarthBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 50 + bob);
    float r = 20;
    sf::Color skin = flash(bodyCol(Element::Earth), fl);
    sf::Color hairCol = flash(sf::Color(25,18,10), fl);

    // Broad stocky hair cap
    sf::ConvexShape hair; hair.setPointCount(4);
    hair.setPoint(0,{-r-2, 2}); hair.setPoint(1,{-r+4, -r-3}); hair.setPoint(2,{r-4,-r-3}); hair.setPoint(3,{r+2,2});
    hair.setPosition(hp); hair.setFillColor(hairCol); w.draw(hair);
    // Face (slightly angular = hex)
    sf::CircleShape h(r, 8); h.setOrigin(r,r); h.setPosition(hp);
    h.setFillColor(skin); h.setOutlineThickness(2); h.setOutlineColor(flash(sf::Color(150,120,80),fl)); w.draw(h);
    // Green Earth Kingdom headband
    sf::RectangleShape hb({r*2+6, 6}); hb.setOrigin(r+3, 3); hb.setPosition(hp.x, hp.y-8);
    hb.setFillColor(flash(sf::Color(40,140,40),fl)); w.draw(hb);
    // Headband small knot/bow
    drawEllipse(w, hp.x-r-1, hp.y-8, 4, 3, flash(sf::Color(30,100,30),fl));

    // Eyes — green, sturdy
    drawEyePair(w, hp.x, hp.y-1, 7, 6, 4, flash(sf::Color(70,150,45),fl), face);
    // Heavy strong brows
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*7, hp.y-9, 10, hairCol, i*-5.f);
    // Determined flat mouth
    drawMouth(w, hp.x+face*2, hp.y+11, 9, false, sf::Color(110,65,50));
    // Wide jaw ears
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*(r-1), hp.y+3, 5, 6, skin, flash(sf::Color(150,120,80),fl), 1.f);
}

void CartoonRenderer::drawMetalBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 18;
    // Full metal helmet
    sf::CircleShape helm(r+2, 8); helm.setOrigin(r+2,r+2); helm.setPosition(hp);
    helm.setFillColor(flash(sf::Color(135,135,150),fl));
    helm.setOutlineThickness(2); helm.setOutlineColor(flash(sf::Color(80,80,95),fl)); w.draw(helm);
    // Helmet ridge on top
    sf::RectangleShape ridge({8, 6}); ridge.setOrigin(4,6); ridge.setPosition(hp.x, hp.y-r-1);
    ridge.setFillColor(flash(sf::Color(100,100,115),fl)); w.draw(ridge);
    // Visor — glowing teal
    sf::RectangleShape visor({r*1.7f, 8}); visor.setOrigin(r*.85f, 4); visor.setPosition(hp.x+face*2, hp.y-2);
    visor.setFillColor(flash(sf::Color(140,210,230,210),fl)); w.draw(visor);
    // Visor glow line
    sf::RectangleShape vgl({r*1.7f, 2}); vgl.setOrigin(r*.85f, 1); vgl.setPosition(hp.x+face*2, hp.y-4);
    vgl.setFillColor(flash(sf::Color(200,240,255,120),fl)); w.draw(vgl);
    // Rivets
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*14, hp.y+10, 2, 2, flash(sf::Color(90,90,100),fl));
    // Chin guard
    sf::RectangleShape chin({18,6}); chin.setOrigin(9,0); chin.setPosition(hp.x,hp.y+r-5);
    chin.setFillColor(flash(sf::Color(120,120,135),fl)); w.draw(chin);
}

void CartoonRenderer::drawLightningBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 18;
    sf::Color skin = flash(bodyCol(Element::Lightning), fl);
    sf::Color hairCol = flash(sf::Color(100,50,180), fl);

    // Base face
    drawEllipse(w, hp.x, hp.y, r, r+1, skin, flash(sf::Color(120,80,180),fl), 2.f);
    // Purple electric spiky hair
    for(int i=-2;i<=2;i++){
        float sx = i*7.f, sh = (i==0)?22:(std::abs(i)==1?17:12);
        drawSpike(w, hp.x+sx, hp.y-r+2, hp.x+sx+i*2.f, hp.y-r-sh, 4, hairCol);
    }
    // Electric cheek marks
    for(int i=-1;i<=1;i+=2){
        sf::ConvexShape bolt; bolt.setPointCount(4);
        bolt.setPoint(0,{0,-5}); bolt.setPoint(1,{(float)(i*4),-1}); bolt.setPoint(2,{(float)(i*2),2}); bolt.setPoint(3,{(float)(i*5),6});
        bolt.setPosition(hp.x+i*13, hp.y+2);
        bolt.setFillColor(flash(sf::Color(255,245,80,190),fl)); w.draw(bolt);
    }
    // Eyes — purple/violet
    drawEyePair(w, hp.x, hp.y-2, 7, 6, 4, flash(sf::Color(160,80,240),fl), face);
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*7, hp.y-9, 8, hairCol, i*-6.f);
    drawMouth(w, hp.x+face*3, hp.y+10, 7, false, sf::Color(90,50,70));
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*(r-1), hp.y+2, 4, 5, skin);
}

void CartoonRenderer::drawPlantBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 17;
    sf::Color skin = flash(bodyCol(Element::Earth), fl);

    // Swamp hat (wide lily pad shape)
    drawEllipse(w, hp.x, hp.y-r-3, r+6, 5, flash(sf::Color(55,130,45),fl), flash(sf::Color(30,80,20),fl), 2.f);
    // Hat stem
    sf::RectangleShape stem({4,8}); stem.setOrigin(2,8); stem.setPosition(hp.x,hp.y-r-2);
    stem.setFillColor(flash(sf::Color(40,100,30),fl)); w.draw(stem);
    // Face
    drawEllipse(w, hp.x, hp.y, r, r+1, skin, flash(sf::Color(100,80,50),fl), 2.f);
    // Vine mask across eyes
    sf::RectangleShape mask({r*2-2, 5}); mask.setOrigin(r-1, 2.5f); mask.setPosition(hp.x, hp.y-3);
    mask.setFillColor(flash(sf::Color(38,98,28),fl)); w.draw(mask);
    // Glowing green eyes through mask
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*6+face*1.5f, hp.y-3, 3, 3, flash(sf::Color(120,255,80),fl));
    // Vine tendrils hanging
    for(int i=-1;i<=1;i+=2){
        sf::RectangleShape vine({2, 8+std::abs(i)*3}); vine.setOrigin(1, 0);
        vine.setPosition(hp.x+i*6, hp.y-r-2);
        vine.setFillColor(flash(sf::Color(45,120,35),fl)); w.draw(vine);
    }
    drawMouth(w, hp.x, hp.y+10, 7, false, sf::Color(80,60,40));
}

void CartoonRenderer::drawIceBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 18;
    sf::Color skin = flash(bodyCol(Element::Water), fl);
    sf::Color fur = flash(sf::Color(235,235,250),fl);

    // Fur-lined parka hood (outer)
    drawEllipse(w, hp.x, hp.y-2, r+5, r+5, fur, flash(sf::Color(180,200,230),fl), 2.f);
    // Inner hood dark
    drawEllipse(w, hp.x, hp.y, r+1, r+1, flash(sf::Color(30,40,60),fl));
    // Face oval exposed
    drawEllipse(w, hp.x, hp.y+2, r-5, r-3, skin);
    // Ice blue warpaint
    for(int i=-1;i<=1;i+=2){
        sf::RectangleShape wp({6,2}); wp.setOrigin(3,1);
        wp.setPosition(hp.x+i*5, hp.y+5); wp.setRotation(i*-10.f);
        wp.setFillColor(flash(sf::Color(80,200,255),fl)); w.draw(wp);
    }
    // Eyes — dark, determined
    drawEyePair(w, hp.x, hp.y+1, 5, 5, 3.5f, flash(sf::Color(30,30,45),fl), face);
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*5, hp.y-6, 7, flash(sf::Color(20,20,30),fl), 0);
    drawMouth(w, hp.x+face*2, hp.y+10, 6, false, sf::Color(100,70,60));
}

void CartoonRenderer::drawSwordBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 18;
    sf::Color skin = flash(bodyCol(Element::Water), fl);
    sf::Color hairCol = flash(sf::Color(35,22,12), fl);

    // Shaved-sides wolf tail hair top
    sf::ConvexShape hair; hair.setPointCount(4);
    hair.setPoint(0,{-r,2}); hair.setPoint(1,{-r+4,-r-2}); hair.setPoint(2,{r-4,-r-2}); hair.setPoint(3,{r,2});
    hair.setPosition(hp); hair.setFillColor(hairCol); w.draw(hair);
    // Face
    drawEllipse(w, hp.x, hp.y, r, r+1, skin, flash(sf::Color(100,70,45),fl), 2.f);
    // Wolf tail top knot (to the side)
    drawEllipse(w, hp.x + face*(-r+4), hp.y-r+2, 7, 7, hairCol);
    // Wolf tail strand
    sf::RectangleShape tail({3,14}); tail.setOrigin(1.5f,0);
    tail.setPosition(hp.x+face*(-r+4), hp.y-r+6); tail.setRotation(face*15.f);
    tail.setFillColor(hairCol); w.draw(tail);

    // Eyes — cool blue-grey, confident
    drawEyePair(w, hp.x, hp.y-2, 7, 6, 4, flash(sf::Color(60,100,160),fl), face);
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*7, hp.y-9, 9, hairCol, i*-6.f);
    // Smirk
    drawMouth(w, hp.x+face*3, hp.y+10, 7, false, sf::Color(100,55,50));
    sf::RectangleShape smirk({3,1.5f}); smirk.setOrigin(0,0.75f);
    smirk.setPosition(hp.x+face*3+face*2, hp.y+9); smirk.setRotation(face*-15.f);
    smirk.setFillColor(sf::Color(100,55,50)); w.draw(smirk);
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*(r-1), hp.y+2, 4, 5, skin);
}

void CartoonRenderer::drawFanBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 18;
    sf::Color skin = flash(sf::Color(250,245,240),fl); // Kyoshi white foundation

    // Big elaborate hair updo
    drawEllipse(w, hp.x, hp.y-r-4, r, 8, flash(sf::Color(25,15,10),fl));
    // Face with white foundation
    drawEllipse(w, hp.x, hp.y, r, r+1, skin, flash(sf::Color(200,185,180),fl), 2.f);
    // Green eye shadow makeup band
    sf::RectangleShape eyeband({r*2-2, 8}); eyeband.setOrigin(r-1, 4); eyeband.setPosition(hp.x, hp.y-4);
    eyeband.setFillColor(flash(sf::Color(0,100,60,160),fl)); w.draw(eyeband);
    // Kyoshi red eye shadow
    sf::RectangleShape red1({r-4, 6}); red1.setOrigin((r-4)/2, 3);
    red1.setPosition(hp.x+face*4, hp.y-4);
    red1.setFillColor(flash(sf::Color(180,40,40,180),fl)); w.draw(red1);
    // Eyes — amber/green
    drawEyePair(w, hp.x, hp.y-3, 7, 6, 4.5f, flash(sf::Color(160,110,0),fl), face);
    // Bold brows
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*7, hp.y-11, 9, flash(sf::Color(25,15,10),fl), 0);
    // Red bold lips
    drawEllipse(w, hp.x+face*1.5f, hp.y+10, 5, 3, sf::Color(160,30,30));
    // Gold headdress
    sf::ConvexShape hd; hd.setPointCount(5);
    hd.setPoint(0,{-12,-r-2}); hd.setPoint(1,{-6,-r-10}); hd.setPoint(2,{0,-r-14});
    hd.setPoint(3,{6,-r-10}); hd.setPoint(4,{12,-r-2});
    hd.setPosition(hp); hd.setFillColor(flash(sf::Color(255,210,40),fl)); w.draw(hd);
    // Headdress gem
    drawEllipse(w, hp.x, hp.y-r-14, 4, 4, flash(sf::Color(255,80,80),fl));
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*(r-1), hp.y+2, 4, 5, skin);
}

// =====================================================================
//  OUTFIT (torso / legs / arms)
// =====================================================================
void CartoonRenderer::drawOutfit(sf::RenderWindow& w, sf::Vector2f pos, Element e, float bob, float fl) {
    sf::Color oc = flash(darkCol(e),fl);
    sf::Color ac = flash(accentCol(e),fl);
    sf::Color lc = flash(lightCol(e),fl);

    // Torso with slight taper
    sf::ConvexShape torso; torso.setPointCount(4);
    torso.setPoint(0, {-14, 0}); torso.setPoint(1, {-12, 38}); torso.setPoint(2, {12, 38}); torso.setPoint(3, {14, 0});
    torso.setPosition(pos.x, pos.y-34+bob); torso.setFillColor(oc);
    w.draw(torso);
    // Collar
    sf::ConvexShape collar; collar.setPointCount(4);
    collar.setPoint(0,{-8,0}); collar.setPoint(1,{-4,-5}); collar.setPoint(2,{4,-5}); collar.setPoint(3,{8,0});
    collar.setPosition(pos.x, pos.y-34+bob); collar.setFillColor(lc); w.draw(collar);
    // Chest emblem (element symbol circle)
    drawEllipse(w, pos.x, pos.y-18+bob, 7, 7, lc, ac, 1.5f);
    // Inner symbol
    drawEllipse(w, pos.x, pos.y-18+bob, 3, 3, ac);
    // Belt
    sf::RectangleShape belt({30,5}); belt.setOrigin(15,2.5f);
    belt.setPosition(pos.x, pos.y+3+bob); belt.setFillColor(ac); w.draw(belt);
    // Belt buckle
    drawEllipse(w, pos.x, pos.y+3+bob, 4, 4, lc, ac, 1.f);

    // Element-specific costume detail
    if(e == Element::Fire){
        // Shoulder armor spikes
        for(int i=-1;i<=1;i+=2){
            sf::ConvexShape sp; sp.setPointCount(3);
            sp.setPoint(0,{0,0}); sp.setPoint(1,{(float)(i*9),-10}); sp.setPoint(2,{(float)(i*5),0});
            sp.setPosition(pos.x+i*13, pos.y-32+bob);
            sp.setFillColor(flash(sf::Color(180,30,20),fl)); w.draw(sp);
        }
        // Gold trim lines
        sf::RectangleShape trim({26,2}); trim.setOrigin(13,1);
        trim.setPosition(pos.x, pos.y-28+bob); trim.setFillColor(flash(sf::Color(255,200,40),fl)); w.draw(trim);
    }
    if(e == Element::Air){
        // Flowing robe sides
        sf::ConvexShape robe; robe.setPointCount(4);
        robe.setPoint(0,{-14,0}); robe.setPoint(1,{-20,40}); robe.setPoint(2,{20,40}); robe.setPoint(3,{14,0});
        robe.setPosition(pos.x, pos.y-2+bob);
        robe.setFillColor(flash(sf::Color(220,140,20,100),fl)); w.draw(robe);
        // Monk stripes
        for(int i=0;i<2;i++){
            sf::RectangleShape str({26,3}); str.setOrigin(13,1.5f);
            str.setPosition(pos.x, pos.y-25+bob+i*15);
            str.setFillColor(flash(sf::Color(255,160,30,150),fl)); w.draw(str);
        }
    }
    if(e == Element::Earth){
        // Shoulder pads
        for(int i=-1;i<=1;i+=2){
            sf::RectangleShape pad({12,7}); pad.setOrigin(6,3.5f);
            pad.setPosition(pos.x+i*17, pos.y-30+bob);
            pad.setFillColor(flash(sf::Color(100,70,35),fl)); w.draw(pad);
        }
    }
    if(e == Element::Water){
        // Water tribe fur trim on collar
        sf::RectangleShape fur({28,4}); fur.setOrigin(14,2);
        fur.setPosition(pos.x, pos.y-34+bob);
        fur.setFillColor(flash(sf::Color(230,225,220),fl)); w.draw(fur);
    }
    if(e == Element::Metal){
        // Metal plate lines on chest
        for(int yi=-1;yi<=1;yi+=2){
            sf::RectangleShape pl({24,2}); pl.setOrigin(12,1);
            pl.setPosition(pos.x, pos.y-28+bob+yi*8);
            pl.setFillColor(flash(sf::Color(100,100,110),fl)); w.draw(pl);
        }
    }
}

void CartoonRenderer::drawArm(sf::RenderWindow& w, sf::Vector2f pos, Element e, int facing, float angle, bool atk, float fl) {
    sf::Color oc = flash(darkCol(e), fl);
    sf::Color sc = flash(bodyCol(e), fl);

    sf::RectangleShape arm({8,26}); arm.setOrigin(4,0);
    float ax = pos.x + facing*14, ay = pos.y-30;
    arm.setPosition(ax,ay); arm.setRotation(angle); arm.setFillColor(oc);
    arm.setOutlineThickness(1.5f); arm.setOutlineColor(flash(sf::Color(20,20,20),fl)); w.draw(arm);
    float rad = angle*(float)M_PI/180.f;
    float fx = ax+std::sin(rad)*26, fy = ay+std::cos(rad)*26;
    // Fist (more rounded)
    drawEllipse(w, fx, fy, 5.5f, 5, sc, flash(sf::Color(30,20,20),fl), 1.f);
    if(atk){
        for(int i=0;i<3;i++){
            float la=angle+(i-1)*20; float lr=la*(float)M_PI/180.f;
            sf::RectangleShape ln({2,10}); ln.setOrigin(1,0);
            ln.setPosition(fx+std::sin(lr)*7, fy+std::cos(lr)*7);
            ln.setRotation(la); ln.setFillColor(sf::Color(255,255,200,160)); w.draw(ln);
        }
    }
}

void CartoonRenderer::drawLeg(sf::RenderWindow& w, sf::Vector2f pos, Element e, float angle, float xo, float fl) {
    sf::Color oc = flash(darkCol(e), fl);
    sf::Color shoe = flash(accentCol(e), fl);

    sf::RectangleShape leg({9,28}); leg.setOrigin(4.5f,0);
    float lx=pos.x+xo, ly=pos.y+5;
    leg.setPosition(lx,ly); leg.setRotation(angle); leg.setFillColor(oc);
    leg.setOutlineThickness(1.5f); leg.setOutlineColor(flash(sf::Color(15,15,15),fl)); w.draw(leg);
    float rad=angle*(float)M_PI/180.f;
    float sx=lx+std::sin(rad)*28, sy=ly+std::cos(rad)*28;
    // Rounded shoe
    drawRoundRect(w, sx, sy, 14, 6, 2, shoe);
}

// =====================================================================
//  AIR BALL & AURA
// =====================================================================
void CartoonRenderer::drawAirBall(sf::RenderWindow& w, sf::Vector2f pos, float t) {
    drawEllipse(w, pos.x, pos.y+10, 35, 22, sf::Color(180,220,255,45));
    for(int i=0;i<8;i++){
        float a=t*8+i*45*(float)M_PI/180.f;
        float r=22+std::sin(t*6+i)*4;
        float px=pos.x+std::cos(a)*r, py=pos.y+10+std::sin(a)*r*0.6f;
        sf::CircleShape p(2.5f); p.setOrigin(2.5f,2.5f); p.setPosition(px,py);
        p.setFillColor(sf::Color(200,230,255,(sf::Uint8)(120+60*std::sin(t*4+i)))); w.draw(p);
    }
    drawEllipse(w, pos.x, pos.y+10, 18, 11, sf::Color(200,230,255,75),sf::Color(150,200,255,120),2.f);
}

void CartoonRenderer::drawElementalAura(sf::RenderWindow& w, sf::Vector2f pos, Element e, float t) {
    int n=6;
    for(int i=0;i<n;i++){
        float a=t*2+i*(2*(float)M_PI/n);
        float r=32+std::sin(t*3+i)*7;
        float px=pos.x+std::cos(a)*r, py=pos.y-20+std::sin(a)*r*.5f;
        float sz=2.5f+std::sin(t*5+i*1.3f)*1.5f;
        sf::Uint8 al=(sf::Uint8)(80+60*std::sin(t*4+i));
        sf::CircleShape p(sz); p.setOrigin(sz,sz); p.setPosition(px,py);
        switch(e){
            case Element::Air:       p.setFillColor(sf::Color(180,220,255,al)); break;
            case Element::Fire:      p.setFillColor(sf::Color(255,120,0,al)); break;
            case Element::Water:     p.setFillColor(sf::Color(60,160,255,al)); break;
            case Element::Earth:     p.setFillColor(sf::Color(130,100,50,al)); break;
            case Element::Metal:     p.setFillColor(sf::Color(180,180,200,al)); break;
            case Element::Lightning: p.setFillColor(sf::Color(255,230,100,al)); break;
            case Element::Plant:     p.setFillColor(sf::Color(100,220,80,al)); break;
            case Element::Ice:       p.setFillColor(sf::Color(180,240,255,al)); break;
            case Element::Sword:     p.setFillColor(sf::Color(200,220,255,al)); break;
            case Element::Fan:       p.setFillColor(sf::Color(255,230,120,al)); break;
        }
        w.draw(p);
    }
    if(e==Element::Fire){
        for(int i=0;i<4;i++){
            float fy=pos.y-10-std::fmod(t*80+i*20,60);
            float fx=pos.x+std::sin(t*3+i*2)*12;
            sf::CircleShape f(3-i*.5f); f.setOrigin(f.getRadius(),f.getRadius()); f.setPosition(fx,fy);
            f.setFillColor(sf::Color(255,180-i*30,0,(sf::Uint8)(150-i*30))); w.draw(f);
        }
    }
    if(e==Element::Water){
        for(int i=0;i<3;i++){
            float dy=std::fmod(t*40+i*25,50);
            sf::CircleShape d(2); d.setOrigin(2,2);
            d.setPosition(pos.x+std::sin(i*2.5f)*15, pos.y+dy-10);
            d.setFillColor(sf::Color(80,180,255,(sf::Uint8)(180-dy*3))); w.draw(d);
        }
    }
    if(e==Element::Lightning){
        for(int i=0;i<4;i++){
            float la=t*6+i*1.6f; float lr2=25+std::sin(t*4+i)*8;
            sf::RectangleShape arc({2,8}); arc.setOrigin(1,0);
            arc.setPosition(pos.x+std::cos(la)*lr2, pos.y-20+std::sin(la)*lr2*.5f);
            arc.setRotation(la*57.3f);
            arc.setFillColor(sf::Color(255,255,100,(sf::Uint8)(100+60*std::sin(t*10+i)))); w.draw(arc);
        }
    }
}

// =====================================================================
//  MAIN FIGHT CHARACTER DRAW
// =====================================================================
void CartoonRenderer::drawCharacter(sf::RenderWindow& w, sf::Vector2f pos, Element e,
    AnimationState anim, int facing, float t, float fl, float hpR, bool airBall, float abTimer) {
    float bob=0,af=0,ab=0,ll=0,lr=0;
    bool atk=(anim==AnimationState::Attack);
    switch(anim){
        case AnimationState::Idle:
            bob=std::sin(t*3)*3; af=std::sin(t*2)*5; ab=-af; break;
        case AnimationState::Run:
            bob=std::abs(std::sin(t*10))*4; af=std::sin(t*10)*40*facing; ab=-af;
            ll=std::sin(t*10)*30; lr=-ll; break;
        case AnimationState::Jump:
            bob=-8; af=-30*facing; ab=30*facing; ll=15; lr=-15; break;
        case AnimationState::Attack:
            bob=std::sin(t*20)*2; af=-70*facing; ab=20*facing; ll=10*facing; lr=-5*facing; break;
        case AnimationState::AirBall:
            bob=std::sin(t*6)*5; af=std::sin(t*4)*15; ab=-af; break;
        case AnimationState::WaterRide:
            bob=std::sin(t*5)*4-6; af=std::sin(t*3)*20; ab=-af; ll=10; lr=-10; break;
        case AnimationState::Block:
            af=40*facing; ab=-20*facing; break;
    }
    // Shadow
    drawEllipse(w, pos.x, 498, 18, 5, sf::Color(0,0,0,50));
    if(airBall) drawAirBall(w,pos,t);
    if(anim==AnimationState::WaterRide){
        for(int i=0;i<8;i++){
            float wy=pos.y+10+i*8, ww=14-i*1.2f+std::sin(t*8+i)*3;
            sf::RectangleShape wp({ww,8}); wp.setOrigin(ww/2,4);
            wp.setPosition(pos.x+std::sin(t*6+i)*4, wy);
            wp.setFillColor(sf::Color(40,120,240,(sf::Uint8)(180-i*18))); w.draw(wp);
        }
        for(int i=0;i<5;i++){
            float sa2=t*4+i*1.3f;
            sf::CircleShape sp(3-i*.4f); sp.setOrigin(sp.getRadius(),sp.getRadius());
            sp.setPosition(pos.x+std::cos(sa2)*15, pos.y+70+std::sin(sa2)*5);
            sp.setFillColor(sf::Color(100,200,255,(sf::Uint8)(150-i*25))); w.draw(sp);
        }
    }
    drawArm(w,pos,e,-facing,ab,false,fl);
    drawLeg(w,pos,e,ll,-6,fl);
    drawLeg(w,pos,e,lr,6,fl);
    drawOutfit(w,pos,e,bob,fl);
    drawArm(w,pos,e,facing,af,atk,fl);

    float armRad = af * (float)M_PI / 180.f;
    sf::Vector2f frontElbow(pos.x + facing*14.f, pos.y - 30.f);
    sf::Vector2f frontHand(frontElbow.x + std::sin(armRad) * 26.f,
                           frontElbow.y + std::cos(armRad) * 26.f);

    if(e == Element::Sword){
        float swordAngle = af - facing * 12.f;
        drawHeldSword(w, frontHand + sf::Vector2f(facing*2.f, -2.f), facing, swordAngle, fl);
    } else if(e == Element::Fan){
        float fanAngle = af + facing * 8.f;
        drawHeldFan(w, frontHand + sf::Vector2f(facing*1.f, -1.f), facing, fanAngle, fl);
    }

    switch(e){
        case Element::Air:       drawAirBenderHead(w,pos,facing,bob,fl); break;
        case Element::Fire:      drawFireBenderHead(w,pos,facing,bob,fl); break;
        case Element::Water:     drawWaterBenderHead(w,pos,facing,bob,fl); break;
        case Element::Earth:     drawEarthBenderHead(w,pos,facing,bob,fl); break;
        case Element::Metal:     drawMetalBenderHead(w,pos,facing,bob,fl); break;
        case Element::Lightning: drawLightningBenderHead(w,pos,facing,bob,fl); break;
        case Element::Plant:     drawPlantBenderHead(w,pos,facing,bob,fl); break;
        case Element::Ice:       drawIceBenderHead(w,pos,facing,bob,fl); break;
        case Element::Sword:     drawSwordBenderHead(w,pos,facing,bob,fl); break;
        case Element::Fan:       drawFanBenderHead(w,pos,facing,bob,fl); break;
    }
    if(hpR<.3f){
        float pulse=(std::sin(t*8)+1)*.5f;
        sf::CircleShape warn(28); warn.setOrigin(28,28); warn.setPosition(pos.x,pos.y-20);
        warn.setFillColor(sf::Color(255,0,0,(sf::Uint8)(35*pulse))); w.draw(warn);
    }
}

// =====================================================================
//  PORTRAIT: LARGE DETAILED CHARACTER FOR SELECT SCREEN
//  This is entirely re-done: the portrait draws a BIGGER character
//  with an elemental glow backdrop and much cleaner proportions.
// =====================================================================
void CartoonRenderer::drawPortrait(sf::RenderWindow& w, sf::Vector2f pos, Element e,
                                   bool sel, int pIdx, float t) {
    // The card is 74 wide x 250 tall, pos = card centre
    // We scale the character up to fill the card nicely

    // --- Elemental aura backdrop circle behind character ---
    sf::Color auraCol;
    switch(e){
        case Element::Air:       auraCol = sf::Color(100,180,240); break;
        case Element::Fire:      auraCol = sf::Color(255,80,20);   break;
        case Element::Water:     auraCol = sf::Color(40,140,240);  break;
        case Element::Earth:     auraCol = sf::Color(60,160,50);   break;
        case Element::Metal:     auraCol = sf::Color(160,160,180); break;
        case Element::Lightning: auraCol = sf::Color(180,100,255); break;
        case Element::Plant:     auraCol = sf::Color(80,200,60);   break;
        case Element::Ice:       auraCol = sf::Color(160,220,255); break;
        case Element::Sword:     auraCol = sf::Color(150,170,200); break;
        case Element::Fan:       auraCol = sf::Color(255,200,60);  break;
    }
    float pulse = sel ? (std::sin(t*4)+1)*.5f : 0.3f;

    // Soft glow rings behind character
    for(int i=3;i>=0;i--){
        float gr = 32+i*6;
        sf::Uint8 ga = (sf::Uint8)((8+i*6)*pulse + (sel?12:4));
        sf::Color gc(auraCol.r, auraCol.g, auraCol.b, ga);
        drawEllipse(w, pos.x, pos.y, gr, gr, gc);
    }

    // --- Draw the character larger (scale 1.5x by shifting reference point up) ---
    // We push 'pos' up so the character's feet land near the bottom of the card
    // and the head is well within the card top
    sf::Vector2f cp(pos.x, pos.y + 38); // feet-ish centre

    // Scale trick: instead of real scaling, we draw bigger primitives inline.
    // The head draw functions use pos.y-52, legs use pos.y+5..33
    // So total height ~90px. Card content area ~200px → scale ~1.55
    // We'll draw a scaled version by adjusting cp vertically and using
    // separate larger draw calls below.

    const float S = 1.45f; // scale factor
    sf::Vector2f sc = cp; // scaled centre

    // Shadow
    drawEllipse(w, sc.x, sc.y+42, 22*S, 6*S, sf::Color(0,0,0,60));

    // --- Legs (scaled) ---
    float bob = std::sin(t*3)*2.5f;
    float legAng = std::sin(t*2)*4;
    // Leg function uses: lx=pos.x+xo, ly=pos.y+5, length 28
    auto drawPortLeg = [&](float xo, float angle) {
        sf::Color oc = darkCol(e);
        sf::Color shoe = accentCol(e);
        float lx = sc.x + xo*S, ly = sc.y + 5*S;
        sf::RectangleShape leg({9*S, 28*S}); leg.setOrigin(4.5f*S, 0);
        leg.setPosition(lx,ly); leg.setRotation(angle); leg.setFillColor(oc);
        leg.setOutlineThickness(1.5f); leg.setOutlineColor(sf::Color(15,15,15)); w.draw(leg);
        float rad = angle*(float)M_PI/180.f;
        float sx=lx+std::sin(rad)*28*S, sy=ly+std::cos(rad)*28*S;
        drawRoundRect(w, sx, sy, 14*S, 6*S, 2, shoe);
    };
    drawPortLeg(-6, legAng);
    drawPortLeg( 6, -legAng);

    // --- Torso (scaled) ---
    {
        sf::Color oc = darkCol(e);
        sf::Color ac = accentCol(e);
        sf::Color lc = lightCol(e);
        sf::ConvexShape torso; torso.setPointCount(4);
        torso.setPoint(0,{-14*S,0}); torso.setPoint(1,{-12*S,38*S});
        torso.setPoint(2,{12*S,38*S}); torso.setPoint(3,{14*S,0});
        torso.setPosition(sc.x, sc.y-34*S+bob); torso.setFillColor(oc); w.draw(torso);
        // Collar
        sf::ConvexShape collar; collar.setPointCount(4);
        collar.setPoint(0,{-8*S,0}); collar.setPoint(1,{-4*S,-5*S}); collar.setPoint(2,{4*S,-5*S}); collar.setPoint(3,{8*S,0});
        collar.setPosition(sc.x, sc.y-34*S+bob); collar.setFillColor(lc); w.draw(collar);
        // Chest emblem
        drawEllipse(w, sc.x, sc.y-18*S+bob, 8*S, 8*S, lc, ac, 1.5f);
        drawEllipse(w, sc.x, sc.y-18*S+bob, 4*S, 4*S, ac);
        // Belt
        sf::RectangleShape belt({30*S,5*S}); belt.setOrigin(15*S,2.5f*S);
        belt.setPosition(sc.x, sc.y+3*S+bob); belt.setFillColor(ac); w.draw(belt);
        drawEllipse(w, sc.x, sc.y+3*S+bob, 5*S, 5*S, lc, ac, 1.f);

        // Element details
        if(e==Element::Fire){
            for(int i=-1;i<=1;i+=2){
                sf::ConvexShape sp; sp.setPointCount(3);
                sp.setPoint(0,{0,0}); sp.setPoint(1,{(float)(i*9*S),-10*S}); sp.setPoint(2,{(float)(i*5*S),0});
                sp.setPosition(sc.x+i*13*S, sc.y-32*S+bob);
                sp.setFillColor(sf::Color(180,30,20)); w.draw(sp);
            }
            sf::RectangleShape trim({26*S,2.5f}); trim.setOrigin(13*S,1.25f);
            trim.setPosition(sc.x, sc.y-28*S+bob); trim.setFillColor(sf::Color(255,200,40)); w.draw(trim);
        }
        if(e==Element::Air){
            sf::ConvexShape robe; robe.setPointCount(4);
            robe.setPoint(0,{-14*S,0}); robe.setPoint(1,{-20*S,40*S}); robe.setPoint(2,{20*S,40*S}); robe.setPoint(3,{14*S,0});
            robe.setPosition(sc.x, sc.y-2*S+bob); robe.setFillColor(sf::Color(220,140,20,90)); w.draw(robe);
            for(int i=0;i<2;i++){
                sf::RectangleShape str({26*S,3}); str.setOrigin(13*S,1.5f);
                str.setPosition(sc.x, sc.y-25*S+bob+i*15*S); str.setFillColor(sf::Color(255,160,30,130)); w.draw(str);
            }
        }
        if(e==Element::Earth){
            for(int i=-1;i<=1;i+=2){
                sf::RectangleShape pad({12*S,7*S}); pad.setOrigin(6*S,3.5f*S);
                pad.setPosition(sc.x+i*17*S, sc.y-30*S+bob); pad.setFillColor(sf::Color(100,70,35)); w.draw(pad);
            }
        }
    }

    // --- Arms (scaled) ---
    auto drawPortArm = [&](int facing, float angle) {
        sf::Color oc = darkCol(e);
        sf::Color sk = bodyCol(e);
        float ax = sc.x + facing*14*S, ay = sc.y-30*S;
        sf::RectangleShape arm({8*S,26*S}); arm.setOrigin(4*S,0);
        arm.setPosition(ax,ay); arm.setRotation(angle); arm.setFillColor(oc);
        arm.setOutlineThickness(1.5f); arm.setOutlineColor(sf::Color(20,20,20)); w.draw(arm);
        float rad = angle*(float)M_PI/180.f;
        float fx = ax+std::sin(rad)*26*S, fy = ay+std::cos(rad)*26*S;
        drawEllipse(w, fx, fy, 5.5f*S, 5*S, sk, sf::Color(30,20,20), 1.f);
    };
    float armSwing = std::sin(t*2)*8;
    drawPortArm(-1, -armSwing);
    drawPortArm( 1,  armSwing);

    // --- Head (scaled via offset — head funcs use p.y-52+bob internally) ---
    // To get a scaled head we call the head functions with a modified pos
    // such that hp = sc + (0, -52*S+bob) => we pass pos where pos.y+52 = sc.y+52*S
    // Trick: pass pos = (sc.x, sc.y - (52*S - 52) ) so internal -52+bob lands at sc.y-52*S+bob
    float headOffset = (S - 1.f) * 52.f;
    sf::Vector2f headPos(sc.x, sc.y - headOffset);

    // We still call the same head functions — they draw relative to p.y-52+bob
    // which will now be sc.y - headOffset - 52 + bob = sc.y - S*52 + bob ✓
    // The head shapes themselves aren't scaled (circles still have fixed radius)
    // so we pass a slightly larger bob to compensate visually, and the call is clean.
    switch(e){
        case Element::Air:       drawAirBenderHead(w,headPos,1,bob,0); break;
        case Element::Fire:      drawFireBenderHead(w,headPos,1,bob,0); break;
        case Element::Water:     drawWaterBenderHead(w,headPos,1,bob,0); break;
        case Element::Earth:     drawEarthBenderHead(w,headPos,1,bob,0); break;
        case Element::Metal:     drawMetalBenderHead(w,headPos,1,bob,0); break;
        case Element::Lightning: drawLightningBenderHead(w,headPos,1,bob,0); break;
        case Element::Plant:     drawPlantBenderHead(w,headPos,1,bob,0); break;
        case Element::Ice:       drawIceBenderHead(w,headPos,1,bob,0); break;
        case Element::Sword:     drawSwordBenderHead(w,headPos,1,bob,0); break;
        case Element::Fan:       drawFanBenderHead(w,headPos,1,bob,0); break;
    }

    // --- Animated elemental particle for selected character ---
    if(sel){
        int np = 5;
        for(int i=0;i<np;i++){
            float a = t*3 + i*(2*(float)M_PI/np);
            float pr = 28 + std::sin(t*4+i)*5;
            float px = sc.x + std::cos(a)*pr;
            float py = sc.y - 20 + std::sin(a)*pr*0.4f;
            float psz = 2.5f + std::sin(t*6+i)*1.f;
            sf::Uint8 pa = (sf::Uint8)(160 + 60*std::sin(t*5+i));
            sf::CircleShape p2(psz); p2.setOrigin(psz,psz); p2.setPosition(px,py);
            p2.setFillColor(sf::Color(auraCol.r,auraCol.g,auraCol.b,pa)); w.draw(p2);
        }
    }
}

// =====================================================================
//  DELEGATES TO INLINE HELPERS
// =====================================================================
void CartoonRenderer::drawBackground(sf::RenderWindow& w, float t) { CBG::drawBg(w,t); }
void CartoonRenderer::drawHPBar(sf::RenderWindow& w, sf::Vector2f p, float hp, float en, Element e, bool r) { CBG::drawHPBar(w,p,hp,en,e,r); }

void CartoonRenderer::drawCloud(sf::RenderWindow& w, float x, float y, float s) { CBG::drawCloud(w,x,y,s); }
void CartoonRenderer::drawMountain(sf::RenderWindow& w, float x, float by, float wd, float ht, sf::Color c) { CBG::drawMtn(w,x,by,wd,ht,c); }
void CartoonRenderer::drawTree(sf::RenderWindow& w, float x, float by, float s) {
    sf::CircleShape top(14*s); top.setOrigin(14*s,14*s); top.setPosition(x,by-14*s);
    top.setFillColor(sf::Color(35,115,35)); w.draw(top);
    sf::RectangleShape trunk({4*s,12*s}); trunk.setOrigin(2*s,0); trunk.setPosition(x,by-14*s);
    trunk.setFillColor(sf::Color(100,70,40)); w.draw(trunk);
}