#include "CartoonRenderer.h"
#include "CartoonBg.h"
#include "Fighter.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

CartoonRenderer::CartoonRenderer(){}

sf::Color CartoonRenderer::bodyCol(Element e) const {
    switch(e){
        case Element::Air: return sf::Color(255,200,130);
        case Element::Fire: return sf::Color(240,200,150);
        case Element::Water: return sf::Color(140,100,70);
        case Element::Earth: return sf::Color(210,180,140);
        case Element::Metal: return sf::Color(200,200,210);
        case Element::Lightning: return sf::Color(240,220,180);
        case Element::Plant: return sf::Color(100,200,80);
        case Element::Ice: return sf::Color(180,240,255);
        case Element::Sword: return sf::Color(150,170,190);
        case Element::Fan: return sf::Color(255,200,80);
    } return sf::Color::White;
}
sf::Color CartoonRenderer::darkCol(Element e) const {
    switch(e){
        case Element::Air: return sf::Color(255,160,30);
        case Element::Fire: return sf::Color(140,20,20);
        case Element::Water: return sf::Color(30,60,160);
        case Element::Earth: return sf::Color(50,120,40);
        case Element::Metal: return sf::Color(80,80,95);
        case Element::Lightning: return sf::Color(30,30,40);
        case Element::Plant: return sf::Color(30,80,20);
        case Element::Ice: return sf::Color(60,100,160);
        case Element::Sword: return sf::Color(80,100,120);
        case Element::Fan: return sf::Color(160,120,40);
    } return sf::Color::White;
}
sf::Color CartoonRenderer::lightCol(Element e) const {
    switch(e){
        case Element::Air: return sf::Color(255,230,100);
        case Element::Fire: return sf::Color(255,100,30);
        case Element::Water: return sf::Color(100,180,255);
        case Element::Earth: return sf::Color(130,200,80);
        case Element::Metal: return sf::Color(200,200,220);
        case Element::Lightning: return sf::Color(255,220,50);
        case Element::Plant: return sf::Color(150,255,100);
        case Element::Ice: return sf::Color(220,255,255);
        case Element::Sword: return sf::Color(200,220,255);
        case Element::Fan: return sf::Color(255,240,150);
    } return sf::Color::White;
}
sf::Color CartoonRenderer::accentCol(Element e) const {
    switch(e){
        case Element::Air: return sf::Color(100,180,240);
        case Element::Fire: return sf::Color(255,200,0);
        case Element::Water: return sf::Color(200,230,255);
        case Element::Earth: return sf::Color(139,90,43);
        case Element::Metal: return sf::Color(170,170,180);
        case Element::Lightning: return sf::Color(255,255,255);
        case Element::Plant: return sf::Color(50,150,30);
        case Element::Ice: return sf::Color(255,255,255);
        case Element::Sword: return sf::Color(100,120,150);
        case Element::Fan: return sf::Color(200,80,40);
    } return sf::Color::White;
}
sf::Color CartoonRenderer::flash(sf::Color c, float f) const {
    return f>0?sf::Color(255,255,255,c.a):c;
}

// --- ANIME EYES (big expressive) ---
static void drawAnimeEyes(sf::RenderWindow& w, sf::Vector2f hp, int face, Element e) {
    float eo = face*4.f;
    for(int i=-1;i<=1;i+=2){
        // White
        sf::CircleShape ew(6.f); ew.setOrigin(6,6);
        ew.setPosition(hp.x+i*7+eo, hp.y-2); ew.setFillColor(sf::Color::White);
        w.draw(ew);
        // Iris color
        sf::Color ic;
        switch(e){
            case Element::Air: ic=sf::Color(150,150,150); break;
            case Element::Fire: ic=sf::Color(200,160,0); break;
            case Element::Water: ic=sf::Color(40,100,200); break;
            case Element::Earth: ic=sf::Color(80,160,50); break;
            case Element::Metal: ic=sf::Color(140,140,160); break;
            case Element::Lightning: ic=sf::Color(255,200,0); break;
        }
        sf::CircleShape ir(3.5f); ir.setOrigin(3.5f,3.5f);
        ir.setPosition(hp.x+i*7+eo+face*1.5f, hp.y-2); ir.setFillColor(ic);
        w.draw(ir);
        // Pupil
        sf::CircleShape pu(1.8f); pu.setOrigin(1.8f,1.8f);
        pu.setPosition(hp.x+i*7+eo+face*2, hp.y-2); pu.setFillColor(sf::Color::Black);
        w.draw(pu);
        // Highlight
        sf::CircleShape hl(1.2f); hl.setOrigin(1.2f,1.2f);
        hl.setPosition(hp.x+i*7+eo+face*0.5f, hp.y-4); hl.setFillColor(sf::Color(255,255,255,200));
        w.draw(hl);
    }
}

// --- AIR BENDER HEAD: Bald monk, blue arrow tattoo ---
void CartoonRenderer::drawAirBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y-52+bob);
    float r=19;
    // Head
    sf::CircleShape h(r); h.setOrigin(r,r); h.setPosition(hp);
    h.setFillColor(flash(bodyCol(Element::Air),fl));
    h.setOutlineThickness(2); h.setOutlineColor(flash(sf::Color(180,130,60),fl)); w.draw(h);
    // Blue arrow tattoo pointing down forehead
    sf::ConvexShape arr; arr.setPointCount(4);
    arr.setPoint(0,{0,-r-2}); arr.setPoint(1,{-8,-r+10}); arr.setPoint(2,{0,-r+6}); arr.setPoint(3,{8,-r+10});
    arr.setPosition(hp); arr.setFillColor(flash(sf::Color(80,170,240),fl)); w.draw(arr);
    // Arrow line down back of head
    sf::RectangleShape al({4,12}); al.setOrigin(2,0); al.setPosition(hp.x,hp.y-r-2);
    al.setFillColor(flash(sf::Color(80,170,240),fl)); w.draw(al);
    // Eyes & mouth
    drawAnimeEyes(w,hp,face,Element::Air);
    sf::RectangleShape mo({6,2}); mo.setOrigin(3,1); mo.setPosition(hp.x+face*3,hp.y+9);
    mo.setFillColor(sf::Color(120,60,60)); w.draw(mo);
    // Ears
    for(int i=-1;i<=1;i+=2){
        sf::CircleShape ear(4); ear.setOrigin(4,4); ear.setPosition(hp.x+i*r,hp.y);
        ear.setFillColor(flash(bodyCol(Element::Air),fl)); w.draw(ear);
    }
}

// --- FIRE BENDER HEAD: Pointed topknot, scar eye, angular features ---
void CartoonRenderer::drawFireBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y-52+bob);
    float r=18;
    sf::CircleShape h(r); h.setOrigin(r,r); h.setPosition(hp);
    h.setFillColor(flash(bodyCol(Element::Fire),fl));
    h.setOutlineThickness(2); h.setOutlineColor(flash(sf::Color(160,120,80),fl)); w.draw(h);
    // Black spiky hair / topknot
    sf::ConvexShape hair; hair.setPointCount(5);
    hair.setPoint(0,{-r+2,-4}); hair.setPoint(1,{-6,-r-14}); hair.setPoint(2,{0,-r-18});
    hair.setPoint(3,{6,-r-14}); hair.setPoint(4,{r-2,-4});
    hair.setPosition(hp); hair.setFillColor(flash(sf::Color(30,20,20),fl)); w.draw(hair);
    // Side hair
    for(int i=-1;i<=1;i+=2){
        sf::ConvexShape sh; sh.setPointCount(3);
        sh.setPoint(0,{(float)(i*r),2.f}); sh.setPoint(1,{(float)(i*(r+6)),-8.f}); sh.setPoint(2,{(float)(i*(r-4)),-10.f});
        sh.setPosition(hp); sh.setFillColor(flash(sf::Color(30,20,20),fl)); w.draw(sh);
    }
    drawAnimeEyes(w,hp,face,Element::Fire);
    // Scar over left eye
    sf::RectangleShape sc({10,3}); sc.setOrigin(5,1.5f); sc.setPosition(hp.x-7,hp.y-2);
    sc.setRotation(15); sc.setFillColor(flash(sf::Color(200,80,60,180),fl)); w.draw(sc);
    // Mouth - determined smirk
    sf::RectangleShape mo({7,2}); mo.setOrigin(3.5f,1); mo.setPosition(hp.x+face*3,hp.y+9);
    mo.setFillColor(sf::Color(100,40,40)); w.draw(mo);
}

// --- WATER BENDER HEAD: Dark skin, hair loopies, blue beads ---
void CartoonRenderer::drawWaterBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y-52+bob);
    float r=18;
    sf::CircleShape h(r); h.setOrigin(r,r); h.setPosition(hp);
    h.setFillColor(flash(bodyCol(Element::Water),fl));
    h.setOutlineThickness(2); h.setOutlineColor(flash(sf::Color(100,70,45),fl)); w.draw(h);
    // Long flowing brown hair
    sf::ConvexShape hair; hair.setPointCount(6);
    hair.setPoint(0,{-r-2,0}); hair.setPoint(1,{-r+4,-r-4}); hair.setPoint(2,{0,-r-2});
    hair.setPoint(3,{r-4,-r-4}); hair.setPoint(4,{r+2,0}); hair.setPoint(5,{0,r+8});
    hair.setPosition(hp); hair.setFillColor(flash(sf::Color(50,30,20),fl)); w.draw(hair);
    // Hair loopies (signature side curls)
    for(int i=-1;i<=1;i+=2){
        sf::CircleShape loop(5,16); loop.setOrigin(5,5);
        loop.setPosition(hp.x+i*(r+4),hp.y+2); loop.setScale(0.6f,1.f);
        loop.setFillColor(flash(sf::Color(50,30,20),fl)); w.draw(loop);
        // Blue bead
        sf::CircleShape bead(2.5f); bead.setOrigin(2.5f,2.5f);
        bead.setPosition(hp.x+i*(r+4),hp.y+8);
        bead.setFillColor(flash(sf::Color(60,140,230),fl)); w.draw(bead);
    }
    // Redraw face area on top of hair
    sf::CircleShape face2(r-3); face2.setOrigin(r-3,r-3); face2.setPosition(hp.x,hp.y+2);
    face2.setFillColor(flash(bodyCol(Element::Water),fl)); w.draw(face2);
    drawAnimeEyes(w,hp,face,Element::Water);
    // Gentle smile
    sf::ConvexShape smile; smile.setPointCount(3);
    smile.setPoint(0,{-4,0}); smile.setPoint(1,{0,3}); smile.setPoint(2,{4,0});
    smile.setPosition(hp.x+face*2,hp.y+9);
    smile.setFillColor(sf::Color(100,50,50)); w.draw(smile);
}

// --- EARTH BENDER HEAD: Stocky, green headband, strong jaw ---
void CartoonRenderer::drawEarthBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y-50+bob);
    float r=20; // Bigger head, stocky
    sf::CircleShape h(r,6); h.setOrigin(r,r); h.setPosition(hp); // Hex shape = angular
    h.setFillColor(flash(bodyCol(Element::Earth),fl));
    h.setOutlineThickness(2); h.setOutlineColor(flash(sf::Color(150,120,80),fl)); w.draw(h);
    // Short dark hair
    sf::ConvexShape hair; hair.setPointCount(4);
    hair.setPoint(0,{-r,0}); hair.setPoint(1,{-r+6,-r-4}); hair.setPoint(2,{r-6,-r-4}); hair.setPoint(3,{r,0});
    hair.setPosition(hp); hair.setFillColor(flash(sf::Color(30,30,20),fl)); w.draw(hair);
    // Green headband
    sf::RectangleShape hb({r*2+6,5}); hb.setOrigin(r+3,2.5f); hb.setPosition(hp.x,hp.y-8);
    hb.setFillColor(flash(sf::Color(40,140,40),fl)); w.draw(hb);
    // Headband knot
    sf::CircleShape knot(3); knot.setOrigin(3,3); knot.setPosition(hp.x-r-2,hp.y-8);
    knot.setFillColor(flash(sf::Color(30,100,30),fl)); w.draw(knot);
    drawAnimeEyes(w,hp,face,Element::Earth);
    // Tough mouth
    sf::RectangleShape mo({9,2.5f}); mo.setOrigin(4.5f,1.25f); mo.setPosition(hp.x+face*2,hp.y+10);
    mo.setFillColor(sf::Color(100,60,50)); w.draw(mo);
}

// --- METAL BENDER HEAD: Metallic helmet, visor, industrial ---
void CartoonRenderer::drawMetalBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y-52+bob);
    float r=18;
    // Helmet
    sf::CircleShape h(r,8); h.setOrigin(r,r); h.setPosition(hp);
    h.setFillColor(flash(sf::Color(140,140,155),fl));
    h.setOutlineThickness(2); h.setOutlineColor(flash(sf::Color(90,90,100),fl)); w.draw(h);
    // Visor slit
    sf::RectangleShape visor({r*1.6f,6}); visor.setOrigin(r*.8f,3); visor.setPosition(hp.x+face*2,hp.y-2);
    visor.setFillColor(flash(sf::Color(180,200,220),fl)); w.draw(visor);
    // Metal plate lines
    for(int i=-1;i<=1;i+=2){
        sf::RectangleShape pl({2,r}); pl.setOrigin(1,0); pl.setPosition(hp.x+i*10,hp.y-r+4);
        pl.setFillColor(flash(sf::Color(100,100,110),fl)); w.draw(pl);
    }
    // Chin guard
    sf::RectangleShape chin({16,5}); chin.setOrigin(8,0); chin.setPosition(hp.x,hp.y+12);
    chin.setFillColor(flash(sf::Color(120,120,135),fl)); w.draw(chin);
    drawAnimeEyes(w,hp,face,Element::Metal);
}

// --- LIGHTNING BENDER HEAD: Spiky electric hair, glowing marks ---
void CartoonRenderer::drawLightningBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl) {
    sf::Vector2f hp(p.x, p.y-52+bob);
    float r=18;
    sf::CircleShape h(r); h.setOrigin(r,r); h.setPosition(hp);
    h.setFillColor(flash(bodyCol(Element::Lightning),fl));
    h.setOutlineThickness(2); h.setOutlineColor(flash(sf::Color(120,80,180),fl)); w.draw(h);
    // Spiky electric hair
    for(int i=-2;i<=2;i++){
        sf::ConvexShape spike; spike.setPointCount(3);
        float sx=i*8.f, sy=-r-2;
        spike.setPoint(0,{sx-4,sy+8}); spike.setPoint(1,{sx,sy-10-(std::abs(i)==2?4.f:8.f)}); spike.setPoint(2,{sx+4,sy+8});
        spike.setPosition(hp); spike.setFillColor(flash(sf::Color(120,60,200),fl)); w.draw(spike);
    }
    // Electric glow marks on cheeks
    for(int i=-1;i<=1;i+=2){
        sf::RectangleShape mark({3,6}); mark.setOrigin(1.5f,3);
        mark.setPosition(hp.x+i*12,hp.y+2); mark.setRotation(i*15.f);
        mark.setFillColor(flash(sf::Color(255,255,100,160),fl)); w.draw(mark);
    }
    drawAnimeEyes(w,hp,face,Element::Lightning);
    sf::RectangleShape mo({6,2}); mo.setOrigin(3,1); mo.setPosition(hp.x+face*3,hp.y+9);
    mo.setFillColor(sf::Color(100,60,80)); w.draw(mo);
}

// --- OUTFIT (torso) ---
void CartoonRenderer::drawOutfit(sf::RenderWindow& w, sf::Vector2f pos, Element e, float bob, float fl) {
    sf::Color oc = flash(darkCol(e),fl);
    sf::Color ac = flash(accentCol(e),fl);
    // Torso
    sf::RectangleShape torso({26,38}); torso.setOrigin(13,0);
    torso.setPosition(pos.x,pos.y-34+bob); torso.setFillColor(oc);
    torso.setOutlineThickness(2); torso.setOutlineColor(flash(sf::Color(20,20,20),fl)); w.draw(torso);
    // Element emblem on chest
    sf::CircleShape emb(6); emb.setOrigin(6,6); emb.setPosition(pos.x,pos.y-18+bob);
    emb.setFillColor(flash(lightCol(e),fl)); emb.setOutlineThickness(1);
    emb.setOutlineColor(ac); w.draw(emb);
    // Belt
    sf::RectangleShape belt({30,5}); belt.setOrigin(15,2.5f);
    belt.setPosition(pos.x,pos.y+3+bob); belt.setFillColor(ac); w.draw(belt);
    // Fire: shoulder spikes. Earth: wider shoulders
    if(e==Element::Fire){
        for(int i=-1;i<=1;i+=2){
            sf::ConvexShape sp; sp.setPointCount(3);
            sp.setPoint(0,{0,0}); sp.setPoint(1,{(float)(i*8),-10}); sp.setPoint(2,{(float)(i*4),0});
            sp.setPosition(pos.x+i*13,pos.y-32+bob);
            sp.setFillColor(flash(sf::Color(180,30,20),fl)); w.draw(sp);
        }
    }
    if(e==Element::Earth){
        for(int i=-1;i<=1;i+=2){
            sf::RectangleShape pad({10,6}); pad.setOrigin(5,3);
            pad.setPosition(pos.x+i*16,pos.y-30+bob);
            pad.setFillColor(flash(sf::Color(100,70,35),fl)); w.draw(pad);
        }
    }
    if(e==Element::Air){
        // Robe/cape flowing down
        sf::ConvexShape cape; cape.setPointCount(4);
        cape.setPoint(0,{-14,0}); cape.setPoint(1,{-18,38}); cape.setPoint(2,{18,38}); cape.setPoint(3,{14,0});
        cape.setPosition(pos.x,pos.y+3+bob);
        cape.setFillColor(flash(sf::Color(220,140,20,120),fl)); w.draw(cape);
    }
}

// --- ARM ---
void CartoonRenderer::drawArm(sf::RenderWindow& w, sf::Vector2f pos, Element e, int facing, float angle, bool atk, float fl) {
    sf::RectangleShape arm({8,26}); arm.setOrigin(4,0);
    float ax=pos.x+facing*15, ay=pos.y-30;
    arm.setPosition(ax,ay); arm.setRotation(angle);
    arm.setFillColor(flash(darkCol(e),fl));
    arm.setOutlineThickness(1.5f); arm.setOutlineColor(flash(sf::Color(20,20,20),fl)); w.draw(arm);
    float rad=angle*(float)M_PI/180.f;
    float fx=ax+std::sin(rad)*26, fy=ay+std::cos(rad)*26;
    sf::CircleShape fist(5); fist.setOrigin(5,5); fist.setPosition(fx,fy);
    fist.setFillColor(flash(bodyCol(e),fl)); w.draw(fist);
    if(atk){
        for(int i=0;i<3;i++){
            float la=angle+(i-1)*20; float lr=la*(float)M_PI/180.f;
            sf::RectangleShape ln({2,10}); ln.setOrigin(1,0);
            ln.setPosition(fx+std::sin(lr)*7, fy+std::cos(lr)*7);
            ln.setRotation(la); ln.setFillColor(sf::Color(255,255,200,160)); w.draw(ln);
        }
    }
}

// --- LEG ---
void CartoonRenderer::drawLeg(sf::RenderWindow& w, sf::Vector2f pos, Element e, float angle, float xo, float fl) {
    sf::RectangleShape leg({9,28}); leg.setOrigin(4.5f,0);
    float lx=pos.x+xo, ly=pos.y+5;
    leg.setPosition(lx,ly); leg.setRotation(angle);
    leg.setFillColor(flash(darkCol(e),fl));
    leg.setOutlineThickness(1.5f); leg.setOutlineColor(flash(sf::Color(15,15,15),fl)); w.draw(leg);
    float rad=angle*(float)M_PI/180.f;
    float sx=lx+std::sin(rad)*28, sy=ly+std::cos(rad)*28;
    sf::RectangleShape shoe({12,5}); shoe.setOrigin(6,2.5f); shoe.setPosition(sx,sy);
    shoe.setFillColor(flash(accentCol(e),fl)); w.draw(shoe);
}

// --- AIR BALL (spinning air sphere) ---
void CartoonRenderer::drawAirBall(sf::RenderWindow& w, sf::Vector2f pos, float t) {
    // Outer glow
    sf::CircleShape glow(35); glow.setOrigin(35,35); glow.setPosition(pos.x,pos.y+10);
    glow.setFillColor(sf::Color(180,220,255,50)); w.draw(glow);
    // Spinning air lines
    for(int i=0;i<8;i++){
        float a=t*8+i*45*(float)M_PI/180.f;
        float r=22+std::sin(t*6+i)*4;
        float px=pos.x+std::cos(a)*r, py=pos.y+10+std::sin(a)*r*0.6f;
        sf::CircleShape p(2.5f); p.setOrigin(2.5f,2.5f); p.setPosition(px,py);
        p.setFillColor(sf::Color(200,230,255,(sf::Uint8)(120+60*std::sin(t*4+i))));
        w.draw(p);
    }
    // Core sphere
    sf::CircleShape core(18); core.setOrigin(18,18); core.setPosition(pos.x,pos.y+10);
    core.setFillColor(sf::Color(200,230,255,80)); core.setOutlineThickness(2);
    core.setOutlineColor(sf::Color(150,200,255,120)); w.draw(core);
}

// --- ELEMENTAL AURA ---
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
            case Element::Air: p.setFillColor(sf::Color(180,220,255,al)); break;
            case Element::Fire: p.setFillColor(sf::Color(255,120,0,al)); break;
            case Element::Water: p.setFillColor(sf::Color(60,160,255,al)); break;
            case Element::Earth: p.setFillColor(sf::Color(130,100,50,al)); break;
            case Element::Metal: p.setFillColor(sf::Color(180,180,200,al)); break;
            case Element::Lightning: p.setFillColor(sf::Color(255,230,100,al)); break;
            case Element::Plant: p.setFillColor(sf::Color(100,220,80,al)); break;
            case Element::Ice: p.setFillColor(sf::Color(180,240,255,al)); break;
            case Element::Sword: p.setFillColor(sf::Color(200,220,255,al)); break;
            case Element::Fan: p.setFillColor(sf::Color(255,230,120,al)); break;
        }
        w.draw(p);
    }
    // Fire: extra flame particles rising
    if(e==Element::Fire){
        for(int i=0;i<4;i++){
            float fy=pos.y-10-std::fmod(t*80+i*20,60);
            float fx=pos.x+std::sin(t*3+i*2)*12;
            sf::CircleShape f(3-i*.5f); f.setOrigin(f.getRadius(),f.getRadius());
            f.setPosition(fx,fy);
            f.setFillColor(sf::Color(255,180-i*30,0,(sf::Uint8)(150-i*30)));
            w.draw(f);
        }
    }
    // Water: dripping particles
    if(e==Element::Water){
        for(int i=0;i<3;i++){
            float dy=std::fmod(t*40+i*25,50);
            sf::CircleShape d(2); d.setOrigin(2,2);
            d.setPosition(pos.x+std::sin(i*2.5f)*15, pos.y+dy-10);
            d.setFillColor(sf::Color(80,180,255,(sf::Uint8)(180-dy*3)));
            w.draw(d);
        }
    }
    // Metal: orbiting sparks
    if(e==Element::Metal){
        for(int i=0;i<3;i++){
            float ma=t*5+i*2.1f;
            sf::CircleShape s(2); s.setOrigin(2,2);
            s.setPosition(pos.x+std::cos(ma)*20, pos.y-15+std::sin(ma)*12);
            s.setFillColor(sf::Color(255,220,120,(sf::Uint8)(140+50*std::sin(t*8+i))));
            w.draw(s);
        }
    }
    // Lightning: electric arcs
    if(e==Element::Lightning){
        for(int i=0;i<4;i++){
            float la=t*6+i*1.6f;
            float lr2=25+std::sin(t*4+i)*8;
            sf::RectangleShape arc({2,8}); arc.setOrigin(1,0);
            arc.setPosition(pos.x+std::cos(la)*lr2, pos.y-20+std::sin(la)*lr2*.5f);
            arc.setRotation(la*57.3f);
            arc.setFillColor(sf::Color(255,255,100,(sf::Uint8)(100+60*std::sin(t*10+i))));
            w.draw(arc);
        }
    }
}

// --- MAIN CHARACTER DRAW ---
void CartoonRenderer::drawCharacter(sf::RenderWindow& w, sf::Vector2f pos, Element e,
    AnimationState anim, int facing, float t, float fl, float hpR, bool airBall, float abTimer) {
    float bob=0, af=0, ab=0, ll=0, lr=0;
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
    sf::CircleShape shd(18,12); shd.setOrigin(18,4); shd.setPosition(pos.x,498);
    shd.setScale(1,.3f); shd.setFillColor(sf::Color(0,0,0,50)); w.draw(shd);
    // Air ball sphere if active
    if(airBall) drawAirBall(w,pos,t);
    // Water ride pillar visual
    if(anim==AnimationState::WaterRide){
        for(int i=0;i<8;i++){
            float wy=pos.y+10+i*8;
            float ww=14-i*1.2f+std::sin(t*8+i)*3;
            sf::RectangleShape wp({ww,8}); wp.setOrigin(ww/2,4);
            wp.setPosition(pos.x+std::sin(t*6+i)*4, wy);
            wp.setFillColor(sf::Color(40,120,240,(sf::Uint8)(180-i*18)));
            w.draw(wp);
        }
        // Splash at base
        for(int i=0;i<5;i++){
            float sa2=t*4+i*1.3f;
            sf::CircleShape sp(3-i*.4f); sp.setOrigin(sp.getRadius(),sp.getRadius());
            sp.setPosition(pos.x+std::cos(sa2)*15, pos.y+70+std::sin(sa2)*5);
            sp.setFillColor(sf::Color(100,200,255,(sf::Uint8)(150-i*25)));
            w.draw(sp);
        }
    }
    // Draw order: back arm, legs, body, front arm, head
    drawArm(w,pos,e,-facing,ab,false,fl);
    drawLeg(w,pos,e,ll,-6,fl);
    drawLeg(w,pos,e,lr,6,fl);
    drawOutfit(w,pos,e,bob,fl);
    drawArm(w,pos,e,facing,af,atk,fl);
    switch(e){
        case Element::Air: drawAirBenderHead(w,pos,facing,bob,fl); break;
        case Element::Fire: drawFireBenderHead(w,pos,facing,bob,fl); break;
        case Element::Water: drawWaterBenderHead(w,pos,facing,bob,fl); break;
        case Element::Earth: drawEarthBenderHead(w,pos,facing,bob,fl); break;
        case Element::Metal: drawMetalBenderHead(w,pos,facing,bob,fl); break;
        case Element::Lightning: drawLightningBenderHead(w,pos,facing,bob,fl); break;
        case Element::Plant: drawPlantBenderHead(w,pos,facing,bob,fl); break;
        case Element::Ice: drawIceBenderHead(w,pos,facing,bob,fl); break;
        case Element::Sword: drawSwordBenderHead(w,pos,facing,bob,fl); break;
        case Element::Fan: drawFanBenderHead(w,pos,facing,bob,fl); break;
    }
    // Low HP warning pulse
    if(hpR<.3f){
        float pulse=(std::sin(t*8)+1)*.5f;
        sf::CircleShape warn(28); warn.setOrigin(28,28); warn.setPosition(pos.x,pos.y-20);
        warn.setFillColor(sf::Color(255,0,0,(sf::Uint8)(35*pulse))); w.draw(warn);
    }
}

// --- PORTRAIT: Full mini character ---
void CartoonRenderer::drawPortrait(sf::RenderWindow& w, sf::Vector2f pos, Element e, bool sel, int pIdx, float t) {
    // Draw full mini character with idle animation
    float bob=std::sin(t*3)*2;
    sf::Vector2f cp(pos.x, pos.y+8); // character center
    // Mini legs
    drawLeg(w,cp,e,std::sin(t*2)*3,-4,0);
    drawLeg(w,cp,e,-std::sin(t*2)*3,4,0);
    // Mini outfit
    drawOutfit(w,cp,e,bob,0);
    // Mini arms
    drawArm(w,cp,e,-1,-std::sin(t*2)*8,false,0);
    drawArm(w,cp,e,1,std::sin(t*2)*8,false,0);
    // Head
    switch(e){
        case Element::Air: drawAirBenderHead(w,cp,1,bob,0); break;
        case Element::Fire: drawFireBenderHead(w,cp,1,bob,0); break;
        case Element::Water: drawWaterBenderHead(w,cp,1,bob,0); break;
        case Element::Earth: drawEarthBenderHead(w,cp,1,bob,0); break;
        case Element::Metal: drawMetalBenderHead(w,cp,1,bob,0); break;
        case Element::Lightning: drawLightningBenderHead(w,cp,1,bob,0); break;
        case Element::Plant: drawPlantBenderHead(w,cp,1,bob,0); break;
        case Element::Ice: drawIceBenderHead(w,cp,1,bob,0); break;
        case Element::Sword: drawSwordBenderHead(w,cp,1,bob,0); break;
        case Element::Fan: drawFanBenderHead(w,cp,1,bob,0); break;
    }
}

// --- DELEGATES TO INLINE HELPERS ---
void CartoonRenderer::drawBackground(sf::RenderWindow& w, float t) { CBG::drawBg(w,t); }
void CartoonRenderer::drawHPBar(sf::RenderWindow& w, sf::Vector2f p, float hp, float en, Element e, bool r) { CBG::drawHPBar(w,p,hp,en,e,r); }


void CartoonRenderer::drawPlantBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl){
    // Swamp plant hat (lilypad style)
    sf::CircleShape hat(16); hat.setOrigin(16,16); hat.setPosition(p.x, p.y-12);
    hat.setScale(1.f, 0.3f); hat.setFillColor(flash(sf::Color(60,140,50),fl));
    hat.setOutlineThickness(1.5f); hat.setOutlineColor(flash(sf::Color(30,80,20),fl));
    w.draw(hat);
    // Face
    sf::CircleShape f(10); f.setOrigin(10,10); f.setPosition(p.x, p.y);
    f.setFillColor(flash(bodyCol(Element::Earth),fl)); w.draw(f);
    // Vine mask over eyes
    sf::RectangleShape mask({20,4}); mask.setOrigin(10,2); mask.setPosition(p.x, p.y-2);
    mask.setFillColor(flash(sf::Color(40,100,30),fl)); w.draw(mask);
    // Glowing green eyes
    sf::CircleShape eye(1.5f); eye.setOrigin(1.5f,1.5f);
    eye.setPosition(p.x+face*4, p.y-2); eye.setFillColor(flash(sf::Color(150,255,100),fl)); w.draw(eye);
}

void CartoonRenderer::drawIceBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl){
    // Fur-lined parka hood
    sf::CircleShape hood(13); hood.setOrigin(13,13); hood.setPosition(p.x-face*2, p.y-2);
    hood.setFillColor(flash(sf::Color(200,230,255),fl)); 
    hood.setOutlineThickness(1.5f); hood.setOutlineColor(flash(sf::Color(100,160,200),fl));
    w.draw(hood);
    // Face
    sf::CircleShape f(9); f.setOrigin(9,9); f.setPosition(p.x, p.y+2);
    f.setFillColor(flash(bodyCol(Element::Water),fl)); w.draw(f);
    // Frostbite warpaint
    sf::RectangleShape wp({4,2}); wp.setOrigin(2,1); wp.setPosition(p.x+face*3, p.y+4);
    wp.setFillColor(flash(sf::Color(100,200,255),fl)); w.draw(wp);
    // Eyes
    sf::CircleShape eye(1.5f); eye.setOrigin(1.5f,1.5f);
    eye.setPosition(p.x+face*4, p.y+1); eye.setFillColor(flash(sf::Color(30,30,40),fl)); w.draw(eye);
}

void CartoonRenderer::drawSwordBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl){
    sf::Vector2f hp(p.x, p.y-52+bob);
    // Base Face
    sf::CircleShape h(18); h.setOrigin(18,18); h.setPosition(hp);
    h.setFillColor(flash(bodyCol(Element::Water),fl));
    h.setOutlineThickness(2); h.setOutlineColor(flash(sf::Color(100,70,45),fl)); w.draw(h);
    // Wolf tail shaved sides
    sf::ConvexShape hair; hair.setPointCount(4);
    hair.setPoint(0,{-18,0}); hair.setPoint(1,{-18,-18}); hair.setPoint(2,{18,-18}); hair.setPoint(3,{18,0});
    hair.setPosition(hp); hair.setFillColor(flash(sf::Color(40,30,20),fl)); w.draw(hair);
    // Wolf tail bun
    sf::CircleShape bun(6); bun.setOrigin(6,6); bun.setPosition(hp.x-face*16, hp.y-14);
    bun.setFillColor(flash(sf::Color(30,20,10),fl)); w.draw(bun);
    drawAnimeEyes(w,hp,face,Element::Water);
    // Smirk
    sf::RectangleShape mo({8,2}); mo.setOrigin(4,1); mo.setPosition(hp.x+face*4,hp.y+10);
    mo.setFillColor(sf::Color(100,50,50)); mo.setRotation(face*-10); w.draw(mo);
}

void CartoonRenderer::drawFanBenderHead(sf::RenderWindow& w, sf::Vector2f p, int face, float bob, float fl){
    sf::Vector2f hp(p.x, p.y-52+bob);
    // Base Face (White Kyoshi Makeup)
    sf::CircleShape h(18); h.setOrigin(18,18); h.setPosition(hp);
    h.setFillColor(flash(sf::Color(250,245,245),fl));
    h.setOutlineThickness(2); h.setOutlineColor(flash(sf::Color(200,180,180),fl)); w.draw(h);
    // Red eye shadow makeup
    sf::RectangleShape shadow({14,8}); shadow.setOrigin(7,4); shadow.setPosition(hp.x+face*5, hp.y-4);
    shadow.setFillColor(flash(sf::Color(180,40,40),fl)); w.draw(shadow);
    drawAnimeEyes(w,hp,face,Element::Earth);
    // Red lips
    sf::CircleShape lip(3); lip.setOrigin(3,3); lip.setPosition(hp.x+face*2,hp.y+10);
    lip.setScale(1.5f,0.5f); lip.setFillColor(sf::Color(150,30,30)); w.draw(lip);
    // Golden Headdress
    sf::ConvexShape headress; headress.setPointCount(3);
    headress.setPoint(0,{-12,-18}); headress.setPoint(1,{0,-28}); headress.setPoint(2,{12,-18});
    headress.setPosition(hp); headress.setFillColor(flash(sf::Color(255,200,40),fl)); w.draw(headress);
}
