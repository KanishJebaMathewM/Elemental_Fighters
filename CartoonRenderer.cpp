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
        case Element::Air:       return sf::Color(255,218,170);  // Aang - warm peach
        case Element::Fire:      return sf::Color(245,210,165);  // Zuko - pale
        case Element::Water:     return sf::Color(150,105,75);   // Katara - brown
        case Element::Earth:     return sf::Color(235,210,175);  // Toph - fair/pale
        case Element::Metal:     return sf::Color(200,200,210);  // Metal Toph
        case Element::Lightning: return sf::Color(245,225,195);  // Azula - porcelain
        case Element::Plant:     return sf::Color(180,150,110);  // Swamp - tan
        case Element::Ice:       return sf::Color(150,105,75);   // Sokka - Water Tribe brown
        case Element::Sword:     return sf::Color(220,185,150);  // Sword - tanned
        case Element::Fan:       return sf::Color(250,245,238);  // Kyoshi - white paint
    } return sf::Color::White;
}
sf::Color CartoonRenderer::darkCol(Element e) const {
    switch(e){
        case Element::Air:       return sf::Color(230,140,30);   // orange monk robe
        case Element::Fire:      return sf::Color(120,15,15);    // Zuko dark red armor
        case Element::Water:     return sf::Color(25,55,145);    // Katara deep blue
        case Element::Earth:     return sf::Color(45,115,35);    // Toph green
        case Element::Metal:     return sf::Color(75,80,90);     // dark steel
        case Element::Lightning: return sf::Color(130,15,20);    // Azula dark crimson
        case Element::Plant:     return sf::Color(30,75,20);     // swamp dark green
        case Element::Ice:       return sf::Color(35,65,135);    // Sokka dark blue
        case Element::Sword:     return sf::Color(60,60,70);     // dark armor
        case Element::Fan:       return sf::Color(35,100,55);    // Kyoshi deep green
    } return sf::Color::White;
}
sf::Color CartoonRenderer::lightCol(Element e) const {
    switch(e){
        case Element::Air:       return sf::Color(255,235,110);  // yellow robe accent
        case Element::Fire:      return sf::Color(255,100,30);   // fire orange
        case Element::Water:     return sf::Color(100,175,245);  // light blue
        case Element::Earth:     return sf::Color(140,200,80);   // light green
        case Element::Metal:     return sf::Color(195,200,215);  // light steel
        case Element::Lightning: return sf::Color(255,200,40);   // Azula gold trim
        case Element::Plant:     return sf::Color(140,240,90);   // bright leaf
        case Element::Ice:       return sf::Color(215,245,255);  // ice white-blue
        case Element::Sword:     return sf::Color(200,215,240);  // steel blue
        case Element::Fan:       return sf::Color(255,210,50);   // Kyoshi gold
    } return sf::Color::White;
}
sf::Color CartoonRenderer::accentCol(Element e) const {
    switch(e){
        case Element::Air:       return sf::Color(80,170,235);   // arrow blue
        case Element::Fire:      return sf::Color(255,195,30);   // Zuko gold
        case Element::Water:     return sf::Color(220,235,255);  // fur white
        case Element::Earth:     return sf::Color(139,95,45);    // earth brown
        case Element::Metal:     return sf::Color(165,170,180);  // metal grey
        case Element::Lightning: return sf::Color(255,195,30);   // Azula gold
        case Element::Plant:     return sf::Color(50,140,30);    // vine green
        case Element::Ice:       return sf::Color(240,240,255);  // ice white
        case Element::Sword:     return sf::Color(90,100,120);   // dark steel
        case Element::Fan:       return sf::Color(190,40,40);    // Kyoshi red
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
    float r = 20;
    sf::Color skin = flash(bodyCol(Element::Air), fl);
    sf::Color outline = flash(sf::Color(190,140,70), fl);
    sf::Color arrowBlue = flash(sf::Color(55,155,230), fl);

    // Bald round head — Aang's signature smooth dome
    drawEllipse(w, hp.x, hp.y, r, r+2, skin, outline, 2.f);
    // Subtle head shine (bald highlight)
    drawEllipse(w, hp.x-3, hp.y-6, 8, 5, sf::Color(255,240,220,60));

    // Blue arrow tattoo — prominent V-shape pointing down forehead
    sf::ConvexShape arr; arr.setPointCount(7);
    arr.setPoint(0, {0, -r-3});      // tip top
    arr.setPoint(1, {-10, -r+8});    // left wing outer
    arr.setPoint(2, {-5, -r+5});     // left wing inner
    arr.setPoint(3, {0, -r+10});     // center notch
    arr.setPoint(4, {5, -r+5});      // right wing inner
    arr.setPoint(5, {10, -r+8});     // right wing outer
    arr.setPoint(6, {0, -r-3});      // back to tip
    arr.setPosition(hp); arr.setFillColor(arrowBlue); w.draw(arr);
    // Arrow shaft going over crown of head
    sf::RectangleShape al({4, 14}); al.setOrigin(2.f, 0);
    al.setPosition(hp.x, hp.y-r-3);
    al.setFillColor(arrowBlue); w.draw(al);
    // Arrow side lines extending down temples
    for(int i=-1;i<=1;i+=2){
        sf::RectangleShape sideLine({2.5f, 8}); sideLine.setOrigin(1.25f, 0);
        sideLine.setPosition(hp.x+i*8, hp.y-r+9); sideLine.setRotation(i*25.f);
        sideLine.setFillColor(flash(sf::Color(55,155,230,160),fl)); w.draw(sideLine);
    }

    // Ears — prominent, rounded (Aang's big ears)
    for(int i=-1;i<=1;i+=2){
        drawEllipse(w, hp.x+i*(r+1), hp.y+2, 5, 6, skin, outline, 1.2f);
        // Inner ear detail
        drawEllipse(w, hp.x+i*(r+1), hp.y+3, 2.5f, 3.5f, sf::Color(240,180,140,100));
    }

    // Cheek blush — anime style
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*11, hp.y+6, 5.5f, 3, sf::Color(255,175,135,70));

    // Big expressive grey eyes — Aang's warm gentle look
    drawEyePair(w, hp.x, hp.y-1, 7.5f, 5.5f, 4.5f, flash(sf::Color(140,145,155),fl), face);

    // Gentle arched eyebrows (no hair, just subtle skin-tone brows)
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*7, hp.y-8, 9, flash(sf::Color(200,160,110),fl), i*-3.f);

    // Happy calm smile
    drawMouth(w, hp.x+face*2, hp.y+10, 7, true, sf::Color(170,85,70));
}

void CartoonRenderer::drawFireBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 19;
    sf::Color skin = flash(bodyCol(Element::Fire), fl);
    sf::Color hair = flash(sf::Color(20,12,10), fl);

    // Sharp angular topknot — Zuko's signature hairstyle
    sf::ConvexShape topknot; topknot.setPointCount(5);
    topknot.setPoint(0,{-r+5,-2}); topknot.setPoint(1,{-6,-r-14});
    topknot.setPoint(2,{0,-r-22}); topknot.setPoint(3,{6,-r-14}); topknot.setPoint(4,{r-5,-2});
    topknot.setPosition(hp); topknot.setFillColor(hair); w.draw(topknot);
    // Topknot tie band
    sf::RectangleShape tband({10, 3}); tband.setOrigin(5,1.5f);
    tband.setPosition(hp.x, hp.y-r-8); tband.setFillColor(flash(sf::Color(180,30,20),fl)); w.draw(tband);

    // Side swept bangs (angular, covering sides of forehead)
    for(int i=-1;i<=1;i+=2){
        sf::ConvexShape sh; sh.setPointCount(5);
        sh.setPoint(0,{(float)(i*(r-3)),2}); sh.setPoint(1,{(float)(i*(r+4)),-6});
        sh.setPoint(2,{(float)(i*(r+6)),-14}); sh.setPoint(3,{(float)(i*(r+2)),-20});
        sh.setPoint(4,{(float)(i*(r-6)),-16});
        sh.setPosition(hp); sh.setFillColor(hair); w.draw(sh);
    }
    // Spiky fringe detail
    for(int i=-1;i<=1;i+=2){
        drawSpike(w, hp.x+i*6, hp.y-r+2, hp.x+i*8, hp.y-r-6, 3, hair);
    }

    // Face (angular jaw line)
    sf::CircleShape faceShape(r, 7);
    faceShape.setOrigin(r,r); faceShape.setPosition(hp);
    faceShape.setFillColor(skin); faceShape.setOutlineThickness(2);
    faceShape.setOutlineColor(flash(sf::Color(170,130,85),fl)); w.draw(faceShape);

    // Gold Fire Nation crown piece on topknot
    sf::ConvexShape crown; crown.setPointCount(7);
    crown.setPoint(0,{-10,-r-5}); crown.setPoint(1,{-7,-r-10}); crown.setPoint(2,{-3,-r-13});
    crown.setPoint(3,{0,-r-15}); crown.setPoint(4,{3,-r-13});
    crown.setPoint(5,{7,-r-10}); crown.setPoint(6,{10,-r-5});
    crown.setPosition(hp); crown.setFillColor(flash(sf::Color(255,200,40),fl)); w.draw(crown);
    // Crown gem
    drawEllipse(w, hp.x, hp.y-r-12, 2.5f, 2.5f, flash(sf::Color(255,60,30),fl));

    // BURN SCAR — Zuko's left eye (drawn on -x side = character's left)
    // Layered scar: outer redness, inner pink, scarred texture
    sf::ConvexShape scarOuter; scarOuter.setPointCount(6);
    scarOuter.setPoint(0,{-16,-8}); scarOuter.setPoint(1,{-14,-2});
    scarOuter.setPoint(2,{-8,4}); scarOuter.setPoint(3,{-3,2});
    scarOuter.setPoint(4,{-5,-5}); scarOuter.setPoint(5,{-12,-10});
    scarOuter.setPosition(hp); scarOuter.setFillColor(flash(sf::Color(185,65,50,180),fl)); w.draw(scarOuter);
    // Inner scar tissue (pink/lighter)
    sf::ConvexShape scarInner; scarInner.setPointCount(4);
    scarInner.setPoint(0,{-14,-5}); scarInner.setPoint(1,{-10,1});
    scarInner.setPoint(2,{-5,0}); scarInner.setPoint(3,{-9,-6});
    scarInner.setPosition(hp); scarInner.setFillColor(flash(sf::Color(220,110,90,150),fl)); w.draw(scarInner);
    // Scar affects left eye — make it slightly squinted
    // Right eye (good eye) — large, intense gold
    drawBigEye(w, hp.x+7+face*2, hp.y-2, 6.5f, 5, flash(sf::Color(200,155,10),fl));
    // Left eye (scarred) — smaller, narrower
    drawEllipse(w, hp.x-7+face*2, hp.y-1, 5, 3.5f, sf::Color(250,250,255), sf::Color(40,20,20), 1.f);
    drawEllipse(w, hp.x-7+face*2, hp.y-0.5f, 3, 2.5f, flash(sf::Color(200,155,10),fl));
    drawEllipse(w, hp.x-7+face*2, hp.y, 1.5f, 1.5f, sf::Color(15,10,10));

    // Angry intense brows
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*7, hp.y-9, 10, hair, i*-10.f);

    // Tight serious/angry mouth
    drawMouth(w, hp.x+face*3, hp.y+11, 9, false, sf::Color(120,55,45));
    // Ears
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*(r), hp.y+2, 4, 5, skin);
}

void CartoonRenderer::drawWaterBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 19;
    sf::Color skin = flash(bodyCol(Element::Water), fl);
    sf::Color hairCol = flash(sf::Color(40,22,12), fl);
    sf::Color beadBlue = flash(sf::Color(45,125,215), fl);

    // Thick voluminous hair behind head (dark brown)
    drawEllipse(w, hp.x, hp.y+3, r+5, r+6, hairCol);
    // Long flowing braids/hair tails hanging down
    for(int i=-1;i<=1;i+=2){
        sf::RectangleShape braid({4, 18}); braid.setOrigin(2, 0);
        braid.setPosition(hp.x+i*(r-2), hp.y+r-2); braid.setRotation(i*5.f);
        braid.setFillColor(hairCol); w.draw(braid);
        // Braid tip bead
        drawEllipse(w, hp.x+i*(r-1), hp.y+r+16, 2.5f, 2.5f, beadBlue);
    }

    // Face — warm brown Water Tribe skin
    drawEllipse(w, hp.x, hp.y, r, r+1, skin, flash(sf::Color(110,72,48),fl), 2.f);

    // Hair loopies — Katara's signature hair loops, more prominent and curvy
    for(int i=-1;i<=1;i+=2){
        // Loop tube (thicker, curving outward then down)
        drawEllipse(w, hp.x+i*(r+2), hp.y-4, 6, 10, hairCol);
        // Inner loop hollow (to show loop shape)
        drawEllipse(w, hp.x+i*(r+2), hp.y-3, 3, 6, skin);
        // Blue Water Tribe bead at bottom of loop
        drawEllipse(w, hp.x+i*(r+2), hp.y+8, 3.5f, 3.5f, beadBlue);
        // Bead shine
        drawEllipse(w, hp.x+i*(r+1), hp.y+7, 1.5f, 1.5f, sf::Color(140,200,255,140));
    }
    // Hair top with center part
    drawEllipse(w, hp.x, hp.y-r+2, r-2, 6, hairCol);
    sf::RectangleShape part({2, 12}); part.setOrigin(1,0);
    part.setPosition(hp.x, hp.y-r-1); part.setFillColor(flash(sf::Color(25,12,8,140),fl)); w.draw(part);
    // Fringe bangs swept to sides
    for(int i=-1;i<=1;i+=2){
        sf::ConvexShape bang; bang.setPointCount(4);
        bang.setPoint(0,{0,-r+2}); bang.setPoint(1,{(float)(i*8),-r+6});
        bang.setPoint(2,{(float)(i*12),-4}); bang.setPoint(3,{(float)(i*4),-6});
        bang.setPosition(hp); bang.setFillColor(hairCol); w.draw(bang);
    }

    // Water Tribe necklace choker
    sf::RectangleShape necklace({16, 2.5f}); necklace.setOrigin(8, 1.25f);
    necklace.setPosition(hp.x, hp.y+r-2); necklace.setFillColor(flash(sf::Color(40,60,120),fl)); w.draw(necklace);
    // Necklace pendant
    drawEllipse(w, hp.x, hp.y+r, 3, 3.5f, beadBlue, flash(sf::Color(30,50,100),fl), 1.f);

    // Big expressive blue eyes — warm and kind
    drawEyePair(w, hp.x, hp.y-2, 7.5f, 6.5f, 5, flash(sf::Color(35,85,195),fl), face);
    // Gentle arched brows
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*7, hp.y-10, 9, hairCol, i*-3.f);
    // Cheek blush — warm on brown skin
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*10, hp.y+5, 5.5f, 3, sf::Color(200,120,90,70));
    // Gentle warm smile
    drawMouth(w, hp.x+face*2, hp.y+10, 7, true, sf::Color(130,65,55));
    // Ears peeking from hair
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*(r-1), hp.y+2, 4, 5, skin);
}

void CartoonRenderer::drawEarthBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 51 + bob);
    float r = 19;
    sf::Color skin = flash(bodyCol(Element::Earth), fl);
    sf::Color hairCol = flash(sf::Color(18,14,8), fl);

    // Large black hair bun on top of head — Toph's signature
    drawEllipse(w, hp.x, hp.y-r-6, 10, 9, hairCol);
    // Bun band (green tie)
    sf::RectangleShape bunBand({14, 3}); bunBand.setOrigin(7, 1.5f);
    bunBand.setPosition(hp.x, hp.y-r-1); bunBand.setFillColor(flash(sf::Color(45,130,40),fl)); w.draw(bunBand);

    // Thick hair framing face (bangs swept across forehead)
    sf::ConvexShape bangs; bangs.setPointCount(6);
    bangs.setPoint(0,{-r-1, 0}); bangs.setPoint(1,{-r+2, -r+2});
    bangs.setPoint(2,{-4, -r-2}); bangs.setPoint(3,{6, -r-2});
    bangs.setPoint(4,{r-2, -r+4}); bangs.setPoint(5,{r+1, 0});
    bangs.setPosition(hp); bangs.setFillColor(hairCol); w.draw(bangs);
    // Side hair strands hanging down
    for(int i=-1;i<=1;i+=2){
        sf::RectangleShape strand({4, 14}); strand.setOrigin(2, 0);
        strand.setPosition(hp.x+i*(r+1), hp.y-4); strand.setRotation(i*3.f);
        strand.setFillColor(hairCol); w.draw(strand);
    }

    // Face — rounder, younger looking
    drawEllipse(w, hp.x, hp.y, r, r+1, skin, flash(sf::Color(180,150,110),fl), 2.f);

    // Green Earth Kingdom headband
    sf::RectangleShape hb({r*2+4, 5}); hb.setOrigin(r+2, 2.5f); hb.setPosition(hp.x, hp.y-r+5);
    hb.setFillColor(flash(sf::Color(45,135,40),fl)); w.draw(hb);
    // Headband gold emblem center
    drawEllipse(w, hp.x, hp.y-r+5, 3.5f, 3.5f, flash(sf::Color(200,180,60),fl));

    // BLIND WHITE EYES — Toph's signature! No irises, just pale milky white
    for(int i=-1;i<=1;i+=2){
        float ex = hp.x + i*7 + face*2;
        float ey = hp.y - 1;
        // Sclera (slightly off-white, milky)
        drawEllipse(w, ex, ey, 6.5f, 5, sf::Color(235,240,240), sf::Color(40,20,20), 1.2f);
        // Very faint iris hint (pale grey-green, barely visible)
        drawEllipse(w, ex, ey+0.5f, 4, 3.5f, sf::Color(210,220,215,120));
        // No pupil! Just a subtle pale center
        drawEllipse(w, ex, ey+1, 2, 2, sf::Color(225,230,228,100));
        // Catchlight (dimmer since blind)
        sf::CircleShape hl(1.5f); hl.setOrigin(1.5f, 1.5f);
        hl.setPosition(ex-1.5f, ey-2); hl.setFillColor(sf::Color(255,255,255,100)); w.draw(hl);
        // Top eyelid line
        sf::RectangleShape lid({14.f, 1.5f}); lid.setOrigin(7, 0.75f);
        lid.setPosition(ex, ey-4.5f); lid.setFillColor(sf::Color(30,15,15,180)); w.draw(lid);
    }

    // Confident/smug brows (slightly raised)
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*7, hp.y-9, 10, hairCol, i*-4.f);
    // Smug confident smirk
    drawMouth(w, hp.x+face*2, hp.y+11, 8, false, sf::Color(160,95,75));
    // One corner turned up (smirk)
    sf::RectangleShape smirk({3, 1.5f}); smirk.setOrigin(0, 0.75f);
    smirk.setPosition(hp.x+face*6, hp.y+10.5f); smirk.setRotation(face*-20.f);
    smirk.setFillColor(sf::Color(160,95,75)); w.draw(smirk);
    // Ears
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*(r-1), hp.y+3, 4.5f, 5.5f, skin);
}

void CartoonRenderer::drawMetalBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 19;
    sf::Color grey = flash(sf::Color(160,160,170), fl);

    // Grey hair peeking out from under helmet
    drawEllipse(w, hp.x, hp.y+4, r-2, 8, flash(sf::Color(140,140,150),fl));
    for(int i=-1;i<=1;i+=2){
        sf::RectangleShape strand({3, 10}); strand.setOrigin(1.5f, 0);
        strand.setPosition(hp.x+i*(r-4), hp.y+2); strand.setRotation(i*8.f);
        strand.setFillColor(flash(sf::Color(130,130,145),fl)); w.draw(strand);
    }

    // Angular metal helmet (octagonal)
    sf::CircleShape helm(r+2, 8); helm.setOrigin(r+2,r+2); helm.setPosition(hp);
    helm.setFillColor(flash(sf::Color(130,135,145),fl));
    helm.setOutlineThickness(2.5f); helm.setOutlineColor(flash(sf::Color(70,75,85),fl)); w.draw(helm);
    // Helmet crest ridge on top
    sf::ConvexShape crest; crest.setPointCount(4);
    crest.setPoint(0,{-5,0}); crest.setPoint(1,{-3,-8}); crest.setPoint(2,{3,-8}); crest.setPoint(3,{5,0});
    crest.setPosition(hp.x, hp.y-r-1); crest.setFillColor(flash(sf::Color(100,105,115),fl)); w.draw(crest);
    // Forehead plate detail
    sf::RectangleShape fplate({r*1.4f, 3}); fplate.setOrigin(r*0.7f, 1.5f);
    fplate.setPosition(hp.x, hp.y-r+6); fplate.setFillColor(flash(sf::Color(110,110,125),fl)); w.draw(fplate);

    // Visor — glowing green-teal (Earth Kingdom metal)
    sf::RectangleShape visor({r*1.8f, 9}); visor.setOrigin(r*.9f, 4.5f); visor.setPosition(hp.x+face*2, hp.y-2);
    visor.setFillColor(flash(sf::Color(120,210,180,210),fl)); w.draw(visor);
    // Visor glow line
    sf::RectangleShape vgl({r*1.8f, 2}); vgl.setOrigin(r*.9f, 1); vgl.setPosition(hp.x+face*2, hp.y-5);
    vgl.setFillColor(flash(sf::Color(180,245,220,120),fl)); w.draw(vgl);
    // Visor bottom edge
    sf::RectangleShape vbl({r*1.8f, 1.5f}); vbl.setOrigin(r*.9f, 0.75f); vbl.setPosition(hp.x+face*2, hp.y+2.5f);
    vbl.setFillColor(flash(sf::Color(60,65,75),fl)); w.draw(vbl);

    // Rivets (4 total)
    for(int i=-1;i<=1;i+=2){
        drawEllipse(w, hp.x+i*15, hp.y+10, 2.5f, 2.5f, flash(sf::Color(85,85,95),fl));
        drawEllipse(w, hp.x+i*12, hp.y-10, 2, 2, flash(sf::Color(85,85,95),fl));
    }
    // Cheek plates
    for(int i=-1;i<=1;i+=2){
        sf::RectangleShape cp({8, 10}); cp.setOrigin(4, 5);
        cp.setPosition(hp.x+i*14, hp.y+5); cp.setFillColor(flash(sf::Color(115,118,128),fl)); w.draw(cp);
    }
    // Chin guard (more angular)
    sf::ConvexShape chin; chin.setPointCount(4);
    chin.setPoint(0,{-10,0}); chin.setPoint(1,{-6,8}); chin.setPoint(2,{6,8}); chin.setPoint(3,{10,0});
    chin.setPosition(hp.x, hp.y+r-6); chin.setFillColor(flash(sf::Color(115,118,130),fl)); w.draw(chin);
    // Wrinkle lines near mouth (older Toph)
    for(int i=-1;i<=1;i+=2){
        sf::RectangleShape wrinkle({4, 1}); wrinkle.setOrigin(2, 0.5f);
        wrinkle.setPosition(hp.x+i*8, hp.y+r-2); wrinkle.setFillColor(sf::Color(90,90,100,80)); w.draw(wrinkle);
    }
}

void CartoonRenderer::drawLightningBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 18;
    sf::Color skin = flash(bodyCol(Element::Lightning), fl);
    sf::Color hairCol = flash(sf::Color(15,10,8), fl); // Black hair, not purple

    // Azula's sharp topknot with red/gold tie
    sf::ConvexShape topknot; topknot.setPointCount(5);
    topknot.setPoint(0,{-6,-4}); topknot.setPoint(1,{-4,-r-10});
    topknot.setPoint(2,{0,-r-16}); topknot.setPoint(3,{4,-r-10}); topknot.setPoint(4,{6,-4});
    topknot.setPosition(hp); topknot.setFillColor(hairCol); w.draw(topknot);
    // Gold topknot ring
    sf::RectangleShape tring({8, 2.5f}); tring.setOrigin(4, 1.25f);
    tring.setPosition(hp.x, hp.y-r-6); tring.setFillColor(flash(sf::Color(255,200,40),fl)); w.draw(tring);

    // Sharp straight bangs framing face — two angular pieces
    for(int i=-1;i<=1;i+=2){
        sf::ConvexShape bang; bang.setPointCount(5);
        bang.setPoint(0,{(float)(i*3),-r+2}); bang.setPoint(1,{(float)(i*(r-2)),-r+4});
        bang.setPoint(2,{(float)(i*(r+3)),-2}); bang.setPoint(3,{(float)(i*(r+1)),6});
        bang.setPoint(4,{(float)(i*6),2});
        bang.setPosition(hp); bang.setFillColor(hairCol); w.draw(bang);
    }
    // Top hair volume
    drawEllipse(w, hp.x, hp.y-r+2, r-2, 5, hairCol);

    // Face — angular, sharp, porcelain skin
    sf::CircleShape faceShape(r, 6); // hexagonal = sharp jawline
    faceShape.setOrigin(r,r); faceShape.setPosition(hp);
    faceShape.setFillColor(skin); faceShape.setOutlineThickness(2);
    faceShape.setOutlineColor(flash(sf::Color(180,150,110),fl)); w.draw(faceShape);

    // Fire Nation crown piece — smaller, more elegant than Zuko's
    sf::ConvexShape crown; crown.setPointCount(5);
    crown.setPoint(0,{-6,-r-3}); crown.setPoint(1,{-3,-r-7});
    crown.setPoint(2,{0,-r-9}); crown.setPoint(3,{3,-r-7}); crown.setPoint(4,{6,-r-3});
    crown.setPosition(hp); crown.setFillColor(flash(sf::Color(255,195,30),fl)); w.draw(crown);

    // Sharp eyeliner (red/dark) around eyes
    for(int i=-1;i<=1;i+=2){
        sf::RectangleShape liner({10, 1.5f}); liner.setOrigin(5, 0.75f);
        liner.setPosition(hp.x+i*7+face*2, hp.y-4); liner.setRotation(i*-5.f);
        liner.setFillColor(flash(sf::Color(120,20,30,180),fl)); w.draw(liner);
    }
    // Gold/amber eyes — cold, calculating
    drawEyePair(w, hp.x, hp.y-2, 7, 5.5f, 4.5f, flash(sf::Color(210,165,15),fl), face);
    // Sharp thin angular brows
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*7, hp.y-9, 9, hairCol, i*-8.f);
    // Cruel smirk
    drawMouth(w, hp.x+face*3, hp.y+10, 7, false, sf::Color(160,50,55));
    // Smirk corner
    sf::RectangleShape smirk({3, 1.5f}); smirk.setOrigin(0, 0.75f);
    smirk.setPosition(hp.x+face*6, hp.y+9.5f); smirk.setRotation(face*-18.f);
    smirk.setFillColor(sf::Color(160,50,55)); w.draw(smirk);
    // Red lipstick hint
    drawEllipse(w, hp.x+face*1, hp.y+10, 4, 2, sf::Color(180,55,55,80));
    // Ears
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*(r-1), hp.y+2, 4, 5, skin);
}

void CartoonRenderer::drawPlantBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 18;
    sf::Color skin = flash(bodyCol(Element::Plant), fl);

    // Wide swamp leaf hat (layered lily pad)
    drawEllipse(w, hp.x, hp.y-r-4, r+8, 6, flash(sf::Color(50,125,40),fl), flash(sf::Color(28,75,18),fl), 2.f);
    drawEllipse(w, hp.x, hp.y-r-2, r+4, 4, flash(sf::Color(60,140,48),fl));
    // Hat stem/stalk on top
    sf::RectangleShape stem({5,10}); stem.setOrigin(2.5f,10); stem.setPosition(hp.x,hp.y-r-3);
    stem.setFillColor(flash(sf::Color(38,95,28),fl)); w.draw(stem);
    // Small leaf on stem top
    drawEllipse(w, hp.x+2, hp.y-r-12, 4, 3, flash(sf::Color(55,140,40),fl));

    // Face
    drawEllipse(w, hp.x, hp.y, r, r+1, skin, flash(sf::Color(120,95,60),fl), 2.f);

    // Thick vine mask covering upper face
    sf::ConvexShape mask; mask.setPointCount(6);
    mask.setPoint(0,{-r+2,-8}); mask.setPoint(1,{-r+4,-2}); mask.setPoint(2,{-r+2,4});
    mask.setPoint(3,{r-2,4}); mask.setPoint(4,{r-4,-2}); mask.setPoint(5,{r-2,-8});
    mask.setPosition(hp); mask.setFillColor(flash(sf::Color(35,92,25),fl)); w.draw(mask);
    // Vine texture lines on mask
    for(int i=0;i<3;i++){
        sf::RectangleShape vl({r*1.6f, 1.5f}); vl.setOrigin(r*0.8f, 0.75f);
        vl.setPosition(hp.x, hp.y-6+i*4);
        vl.setFillColor(flash(sf::Color(28,70,18,120),fl)); w.draw(vl);
    }

    // Bright glowing green eyes through mask
    for(int i=-1;i<=1;i+=2){
        // Glow halo
        drawEllipse(w, hp.x+i*7+face*1.5f, hp.y-3, 5, 4.5f, sf::Color(80,255,60,50));
        // Eye
        drawEllipse(w, hp.x+i*7+face*1.5f, hp.y-3, 3.5f, 3, flash(sf::Color(100,255,70),fl));
        // Bright center
        drawEllipse(w, hp.x+i*7+face*1.5f, hp.y-3, 1.5f, 1.5f, sf::Color(200,255,180));
    }

    // Vine tendrils hanging from hat/mask
    for(int i=-2;i<=2;i++){
        float vx = hp.x + i*6.f;
        float vlen = 10.f + std::abs(i)*3.f;
        sf::RectangleShape vine({2.5f, vlen}); vine.setOrigin(1.25f, 0);
        vine.setPosition(vx, hp.y-r-2); vine.setRotation(i*4.f);
        vine.setFillColor(flash(sf::Color(42,115,32),fl)); w.draw(vine);
    }

    // Mossy beard/chin area
    for(int i=-1;i<=1;i++){
        drawEllipse(w, hp.x+i*5, hp.y+r-4, 5, 4, flash(sf::Color(55,100,35,160),fl));
    }
    // Mouth — barely visible through vines
    drawMouth(w, hp.x, hp.y+10, 7, false, sf::Color(90,65,45));
}

void CartoonRenderer::drawIceBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 19;
    sf::Color skin = flash(bodyCol(Element::Ice), fl); // Brown Water Tribe skin
    sf::Color fur = flash(sf::Color(230,230,245),fl);
    sf::Color parkaBlue = flash(sf::Color(30,45,65),fl);

    // Fur-lined parka hood (outer ring)
    drawEllipse(w, hp.x, hp.y-2, r+6, r+6, fur, flash(sf::Color(190,200,220),fl), 2.f);
    // Inner hood (dark blue parka)
    drawEllipse(w, hp.x, hp.y, r+2, r+2, parkaBlue);

    // Wolf-tail hair on top (shaved sides, ponytail pulled back)
    sf::Color hairCol = flash(sf::Color(35,20,12),fl);
    // Hair top strip (mohawk-like leading to ponytail)
    sf::ConvexShape hairTop; hairTop.setPointCount(4);
    hairTop.setPoint(0,{-8,-r+3}); hairTop.setPoint(1,{-4,-r-4});
    hairTop.setPoint(2,{4,-r-4}); hairTop.setPoint(3,{8,-r+3});
    hairTop.setPosition(hp); hairTop.setFillColor(hairCol); w.draw(hairTop);
    // Wolf-tail ponytail going back
    sf::RectangleShape ponytail({4, 16}); ponytail.setOrigin(2, 0);
    ponytail.setPosition(hp.x-face*3, hp.y-r-2); ponytail.setRotation(-face*20.f);
    ponytail.setFillColor(hairCol); w.draw(ponytail);
    // Ponytail tie
    sf::RectangleShape tie({6, 2.5f}); tie.setOrigin(3, 1.25f);
    tie.setPosition(hp.x-face*3, hp.y-r-1); tie.setFillColor(flash(sf::Color(50,100,180),fl)); w.draw(tie);

    // Face (brown skin, exposed from hood)
    drawEllipse(w, hp.x, hp.y+2, r-4, r-2, skin);

    // Blue war paint streaks on cheeks — Sokka's warrior look
    for(int i=-1;i<=1;i+=2){
        // Diagonal streak
        sf::RectangleShape wp({7, 2.5f}); wp.setOrigin(3.5f, 1.25f);
        wp.setPosition(hp.x+i*6, hp.y+5); wp.setRotation(i*-12.f);
        wp.setFillColor(flash(sf::Color(70,180,245),fl)); w.draw(wp);
        // Second smaller streak below
        sf::RectangleShape wp2({5, 2}); wp2.setOrigin(2.5f, 1);
        wp2.setPosition(hp.x+i*5, hp.y+8); wp2.setRotation(i*-8.f);
        wp2.setFillColor(flash(sf::Color(70,180,245),fl)); w.draw(wp2);
    }

    // Eyes — dark blue-brown, confident and sharp
    drawEyePair(w, hp.x, hp.y+1, 6, 5.5f, 4, flash(sf::Color(35,45,70),fl), face);
    // Confident raised brows
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*6, hp.y-5, 8, flash(sf::Color(25,15,10),fl), i*-5.f);
    // Confident smirk
    drawMouth(w, hp.x+face*2, hp.y+11, 7, false, sf::Color(110,70,55));
    sf::RectangleShape smirk({3, 1.5f}); smirk.setOrigin(0, 0.75f);
    smirk.setPosition(hp.x+face*5, hp.y+10.5f); smirk.setRotation(face*-15.f);
    smirk.setFillColor(sf::Color(110,70,55)); w.draw(smirk);
}

void CartoonRenderer::drawSwordBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 18;
    sf::Color skin = flash(bodyCol(Element::Sword), fl);
    sf::Color hairCol = flash(sf::Color(30,18,10), fl);

    // Sleek samurai hair — swept back, angular
    sf::ConvexShape hair; hair.setPointCount(5);
    hair.setPoint(0,{-r-1,2}); hair.setPoint(1,{-r+2,-r}); hair.setPoint(2,{0,-r-4});
    hair.setPoint(3,{r-2,-r}); hair.setPoint(4,{r+1,2});
    hair.setPosition(hp); hair.setFillColor(hairCol); w.draw(hair);

    // Face — angular, warrior look
    drawEllipse(w, hp.x, hp.y, r, r+1, skin, flash(sf::Color(160,130,95),fl), 2.f);

    // Short ponytail at back (samurai style)
    sf::RectangleShape tail({4,12}); tail.setOrigin(2,0);
    tail.setPosition(hp.x-face*(r-6), hp.y-r+4); tail.setRotation(-face*18.f);
    tail.setFillColor(hairCol); w.draw(tail);
    // Ponytail tie
    sf::RectangleShape ptie({6, 2}); ptie.setOrigin(3, 1);
    ptie.setPosition(hp.x-face*(r-6), hp.y-r+5); ptie.setFillColor(flash(sf::Color(80,80,90),fl)); w.draw(ptie);

    // Headband — thin dark steel
    sf::RectangleShape hband({r*2+8, 4}); hband.setOrigin(r+4, 2);
    hband.setPosition(hp.x, hp.y-r+7); hband.setFillColor(flash(sf::Color(70,75,85),fl)); w.draw(hband);
    // Headband knot on side
    drawEllipse(w, hp.x+face*(r+2), hp.y-r+7, 3, 3, flash(sf::Color(60,65,75),fl));
    // Headband tails
    sf::RectangleShape htail({2, 8}); htail.setOrigin(1, 0);
    htail.setPosition(hp.x+face*(r+3), hp.y-r+8); htail.setRotation(face*10.f);
    htail.setFillColor(flash(sf::Color(70,75,85),fl)); w.draw(htail);

    // Eyes — sharp steel-blue, focused
    drawEyePair(w, hp.x, hp.y-2, 7, 5.5f, 4, flash(sf::Color(55,90,145),fl), face);
    // Determined angular brows
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*7, hp.y-9, 10, hairCol, i*-7.f);
    // Thin determined mouth
    drawMouth(w, hp.x+face*3, hp.y+10, 8, false, sf::Color(110,60,50));
    // Ears
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*(r-1), hp.y+2, 4, 5, skin);
}

void CartoonRenderer::drawFanBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y - 52 + bob);
    float r = 18;
    sf::Color skin = flash(sf::Color(248,243,236),fl); // Kyoshi white foundation
    sf::Color hairCol = flash(sf::Color(20,12,8), fl);

    // Elaborate hair updo (tall, structured)
    drawEllipse(w, hp.x, hp.y-r-8, 11, 10, hairCol);
    // Side hair buns/ornaments
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*12, hp.y-r+2, 6, 6, hairCol);
    // Hair framing face
    for(int i=-1;i<=1;i+=2){
        sf::ConvexShape sideHair; sideHair.setPointCount(4);
        sideHair.setPoint(0,{(float)(i*(r-4)),-r+2}); sideHair.setPoint(1,{(float)(i*(r+2)),-4});
        sideHair.setPoint(2,{(float)(i*(r+1)),6}); sideHair.setPoint(3,{(float)(i*(r-2)),4});
        sideHair.setPosition(hp); sideHair.setFillColor(hairCol); w.draw(sideHair);
    }

    // Face with white foundation paint
    drawEllipse(w, hp.x, hp.y, r, r+1, skin, flash(sf::Color(195,185,175),fl), 2.f);

    // Bold green eye shadow band — extending from eye to temple (Kyoshi signature)
    for(int i=-1;i<=1;i+=2){
        sf::ConvexShape eyeShadow; eyeShadow.setPointCount(5);
        eyeShadow.setPoint(0,{(float)(i*2),-7}); eyeShadow.setPoint(1,{(float)(i*14),-8});
        eyeShadow.setPoint(2,{(float)(i*16),-2}); eyeShadow.setPoint(3,{(float)(i*14),2});
        eyeShadow.setPoint(4,{(float)(i*2),0});
        eyeShadow.setPosition(hp); eyeShadow.setFillColor(flash(sf::Color(15,90,50,180),fl)); w.draw(eyeShadow);
    }

    // Red accent under each eye shadow
    for(int i=-1;i<=1;i+=2){
        sf::RectangleShape redAccent({12, 2.5f}); redAccent.setOrigin(6, 1.25f);
        redAccent.setPosition(hp.x+i*8, hp.y-1); redAccent.setRotation(i*-3.f);
        redAccent.setFillColor(flash(sf::Color(175,35,35,160),fl)); w.draw(redAccent);
    }

    // Eyes — sharp amber/green through the paint
    drawEyePair(w, hp.x, hp.y-3, 7, 6, 4.5f, flash(sf::Color(165,115,5),fl), face);
    // Bold dark brows
    for(int i=-1;i<=1;i+=2)
        drawBrow(w, hp.x+i*7, hp.y-11, 10, hairCol, 0);

    // Bold red lips — Kyoshi warrior signature
    drawEllipse(w, hp.x+face*1.5f, hp.y+10, 6, 3.5f, flash(sf::Color(175,30,30),fl));
    // Lip highlight
    drawEllipse(w, hp.x+face*1-1, hp.y+9, 2, 1.5f, sf::Color(220,80,80,100));

    // Gold fan-shaped headdress crown
    sf::ConvexShape hd; hd.setPointCount(7);
    hd.setPoint(0,{-14,-r-2}); hd.setPoint(1,{-10,-r-10}); hd.setPoint(2,{-4,-r-15});
    hd.setPoint(3,{0,-r-18}); hd.setPoint(4,{4,-r-15});
    hd.setPoint(5,{10,-r-10}); hd.setPoint(6,{14,-r-2});
    hd.setPosition(hp); hd.setFillColor(flash(sf::Color(255,210,40),fl)); w.draw(hd);
    // Headdress inner detail
    sf::ConvexShape hdInner; hdInner.setPointCount(5);
    hdInner.setPoint(0,{-8,-r-4}); hdInner.setPoint(1,{-4,-r-10});
    hdInner.setPoint(2,{0,-r-13}); hdInner.setPoint(3,{4,-r-10}); hdInner.setPoint(4,{8,-r-4});
    hdInner.setPosition(hp); hdInner.setFillColor(flash(sf::Color(240,190,30),fl)); w.draw(hdInner);
    // Headdress center gem
    drawEllipse(w, hp.x, hp.y-r-14, 4, 4, flash(sf::Color(255,70,70),fl));
    drawEllipse(w, hp.x-1, hp.y-r-15, 1.5f, 1.5f, sf::Color(255,160,160,150));

    // Gold hair ornament pins
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*12, hp.y-r+2, 3, 3, flash(sf::Color(255,200,40),fl));

    // Ears (barely visible under hair)
    for(int i=-1;i<=1;i+=2)
        drawEllipse(w, hp.x+i*(r-1), hp.y+2, 3.5f, 4.5f, skin);
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
        // Rivets on shoulders
        for(int i=-1;i<=1;i+=2)
            drawEllipse(w, pos.x+i*13, pos.y-32+bob, 2, 2, flash(sf::Color(90,90,100),fl));
    }
    if(e == Element::Lightning){
        // Azula Fire Nation armor — shoulder spikes + gold trim
        for(int i=-1;i<=1;i+=2){
            sf::ConvexShape sp; sp.setPointCount(3);
            sp.setPoint(0,{0,0}); sp.setPoint(1,{(float)(i*8),-9}); sp.setPoint(2,{(float)(i*4),0});
            sp.setPosition(pos.x+i*13, pos.y-32+bob);
            sp.setFillColor(flash(sf::Color(160,20,20),fl)); w.draw(sp);
        }
        sf::RectangleShape trim({26,2}); trim.setOrigin(13,1);
        trim.setPosition(pos.x, pos.y-28+bob); trim.setFillColor(flash(sf::Color(255,195,30),fl)); w.draw(trim);
    }
    if(e == Element::Plant){
        // Vine wraps around torso
        for(int i=0;i<3;i++){
            sf::RectangleShape vine({24,2.5f}); vine.setOrigin(12,1.25f);
            vine.setPosition(pos.x, pos.y-26+bob+i*10); vine.setRotation(i*5.f-5.f);
            vine.setFillColor(flash(sf::Color(40,110,28,150),fl)); w.draw(vine);
        }
    }
    if(e == Element::Ice){
        // Fur trim on collar (thicker)
        sf::RectangleShape fur({30,5}); fur.setOrigin(15,2.5f);
        fur.setPosition(pos.x, pos.y-34+bob);
        fur.setFillColor(flash(sf::Color(235,235,248),fl)); w.draw(fur);
    }
    if(e == Element::Sword){
        // Scabbard/strap on back (diagonal)
        sf::RectangleShape strap({3,34}); strap.setOrigin(1.5f,0);
        strap.setPosition(pos.x-6, pos.y-32+bob); strap.setRotation(-15.f);
        strap.setFillColor(flash(sf::Color(70,50,35),fl)); w.draw(strap);
    }
    if(e == Element::Fan){
        // Armor plate details (Kyoshi warrior green armor)
        for(int i=-1;i<=1;i+=2){
            sf::RectangleShape plate({10,6}); plate.setOrigin(5,3);
            plate.setPosition(pos.x+i*11, pos.y-26+bob);
            plate.setFillColor(flash(sf::Color(30,90,50),fl)); w.draw(plate);
        }
        // Gold trim
        sf::RectangleShape gtrim({26,2}); gtrim.setOrigin(13,1);
        gtrim.setPosition(pos.x, pos.y-15+bob); gtrim.setFillColor(flash(sf::Color(255,200,40),fl)); w.draw(gtrim);
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

    if(e == Element::Earth){
        // Toph's bare feet — no shoes!
        sf::Color skin = flash(bodyCol(Element::Earth), fl);
        drawEllipse(w, sx, sy, 8, 4, skin, flash(sf::Color(180,150,110),fl), 1.f);
        // Toes hint
        for(int t2=0;t2<3;t2++)
            drawEllipse(w, sx-3+t2*3, sy-3, 2, 1.5f, skin);
    } else {
        // Rounded shoe
        drawRoundRect(w, sx, sy, 14, 6, 2, shoe);
    }
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
            case Element::Lightning: p.setFillColor(sf::Color(200,220,255,al)); break;
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
        case Element::Lightning: auraCol = sf::Color(200,50,40);  break;
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