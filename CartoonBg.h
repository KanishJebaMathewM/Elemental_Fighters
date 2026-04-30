#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
enum class Element;

namespace CBG {

inline void drawCloud(sf::RenderWindow& w, float x, float y, float s, sf::Uint8 a=180) {
    sf::Color cc(255,255,255,a);
    float r[]={18,26,20,15}; float ox[]={-20,0,20,32};
    for(int i=0;i<4;i++){
        sf::CircleShape c(r[i]*s); c.setOrigin(r[i]*s,r[i]*s);
        c.setPosition(x+ox[i]*s, y-(i==1?9*s:(i==2?5*s:0))); c.setFillColor(cc); w.draw(c);
    }
}

inline void drawMtn(sf::RenderWindow& w, float x, float by, float wd, float ht, sf::Color c) {
    sf::ConvexShape m; m.setPointCount(3);
    m.setPoint(0,{x,by}); m.setPoint(1,{x+wd/2,by-ht}); m.setPoint(2,{x+wd,by});
    m.setFillColor(c); w.draw(m);
    sf::ConvexShape cap; cap.setPointCount(3);
    cap.setPoint(0,{x+wd*.35f,by-ht*.7f}); cap.setPoint(1,{x+wd/2,by-ht}); cap.setPoint(2,{x+wd*.65f,by-ht*.7f});
    cap.setFillColor(sf::Color(230,235,255,160)); w.draw(cap);
}

inline void drawBg(sf::RenderWindow& w, float t) {
    // Gradient sky — deep blue top to warm horizon
    sf::RectangleShape sky({800,600}); sky.setFillColor(sf::Color(25,60,140)); w.draw(sky);
    // Mid sky
    sf::RectangleShape mid({800,350}); mid.setPosition(0,150);
    mid.setFillColor(sf::Color(60,100,180,180)); w.draw(mid);
    // Horizon glow (warm orange/pink)
    sf::RectangleShape hz({800,200}); hz.setPosition(0,300);
    hz.setFillColor(sf::Color(180,120,80,90)); w.draw(hz);
    sf::RectangleShape hz2({800,100}); hz2.setPosition(0,380);
    hz2.setFillColor(sf::Color(200,140,60,60)); w.draw(hz2);

    // Sun with rays
    float sp=std::sin(t*.4f)*4;
    // Sun glow layers
    for(int i=3;i>=0;i--){
        float r=60-i*12+sp;
        sf::CircleShape gl(r); gl.setOrigin(r,r); gl.setPosition(680,100);
        sf::Uint8 a=(sf::Uint8)(20+i*8);
        gl.setFillColor(sf::Color(255,220,80,a)); w.draw(gl);
    }
    sf::CircleShape sun(22+sp); sun.setOrigin(22+sp,22+sp); sun.setPosition(680,100);
    sun.setFillColor(sf::Color(255,235,120)); w.draw(sun);
    // Sun rays
    for(int i=0;i<8;i++){
        float a=t*.3f+i*M_PI/4;
        float len=40+std::sin(t+i)*10;
        sf::RectangleShape ray({2.f,len}); ray.setOrigin(1,0);
        ray.setPosition(680,100); ray.setRotation(a*180/M_PI);
        ray.setFillColor(sf::Color(255,230,100,30)); w.draw(ray);
    }

    // Clouds — multiple layers, different speeds
    float cs=12;
    drawCloud(w,std::fmod(60+t*cs,920)-60,70,1.1f,140);
    drawCloud(w,std::fmod(300+t*cs*.6f,920)-60,110,.9f,160);
    drawCloud(w,std::fmod(550+t*cs*.4f,920)-60,55,1.3f,120);
    drawCloud(w,std::fmod(180+t*cs*.8f,920)-60,140,.7f,100);
    drawCloud(w,std::fmod(700+t*cs*.5f,920)-60,90,.6f,90);

    // Far mountains (misty, blue)
    drawMtn(w,-80,430,280,180,sf::Color(50,60,100));
    drawMtn(w,100,430,350,220,sf::Color(40,50,85));
    drawMtn(w,350,430,300,190,sf::Color(55,65,105));
    drawMtn(w,550,430,280,170,sf::Color(45,55,90));
    drawMtn(w,720,430,200,140,sf::Color(60,70,110));

    // Mist between mountains and hills
    sf::RectangleShape mist({800,30}); mist.setPosition(0,410);
    mist.setFillColor(sf::Color(180,200,230,50)); w.draw(mist);

    // Near hills (green)
    drawMtn(w,-50,500,220,110,sf::Color(35,105,35));
    drawMtn(w,150,500,280,90,sf::Color(45,125,40));
    drawMtn(w,400,500,250,100,sf::Color(38,115,38));
    drawMtn(w,600,500,300,85,sf::Color(42,120,42));

    // Ground with subtle texture
    sf::RectangleShape gnd({800,115}); gnd.setPosition(0,488); gnd.setFillColor(sf::Color(50,125,40)); w.draw(gnd);
    // Ground highlight stripe
    sf::RectangleShape gs({800,3}); gs.setPosition(0,488); gs.setFillColor(sf::Color(70,160,55)); w.draw(gs);
    sf::RectangleShape gs2({800,2}); gs2.setPosition(0,491); gs2.setFillColor(sf::Color(40,100,30)); w.draw(gs2);

    // Ground texture dots
    for(int i=0;i<30;i++){
        float gx=i*27+5+std::sin(i*1.7f)*8;
        float gy=495+std::sin(i*2.3f)*4;
        sf::CircleShape dot(1.5f); dot.setOrigin(1.5f,1.5f); dot.setPosition(gx,gy);
        dot.setFillColor(sf::Color(40,100+((i*7)%30),30,120)); w.draw(dot);
    }

    // Animated grass tufts
    for(int i=0;i<30;i++){
        float gx=i*27+8;
        float sw=std::sin(t*2.5f+i*.8f)*4;
        for(int j=0;j<3;j++){
            float h=6+j*3;
            sf::RectangleShape g({1.5f,h}); g.setOrigin(.75f,h); g.setPosition(gx+j*3-3,496);
            g.setRotation(sw+j*5-5);
            g.setFillColor(sf::Color(25+j*15,110+((i+j)%3)*25,20)); w.draw(g);
        }
    }

    // Distant birds (V shapes)
    for(int i=0;i<3;i++){
        float bx=std::fmod(100+i*250+t*30,850);
        float by=60+i*30+std::sin(t*2+i)*8;
        for(int s=-1;s<=1;s+=2){
            sf::RectangleShape wing({8,1.5f}); wing.setOrigin(0,.75f);
            wing.setPosition(bx,by); wing.setRotation(s*25+std::sin(t*4+i)*10);
            wing.setFillColor(sf::Color(20,20,40,120)); w.draw(wing);
        }
    }
}

inline void drawHPBar(sf::RenderWindow& w, sf::Vector2f p, float hp, float en, Element e, bool right) {
    float bw=300,bh=16;
    // Bar background with gradient effect
    sf::RectangleShape bg({bw+4,bh+4}); bg.setPosition(p.x-2,p.y-2);
    bg.setFillColor(sf::Color(15,15,20,220));
    bg.setOutlineThickness(1); bg.setOutlineColor(sf::Color(60,60,70)); w.draw(bg);
    sf::RectangleShape inner({bw,bh}); inner.setPosition(p);
    inner.setFillColor(sf::Color(30,30,35,200)); w.draw(inner);
    // HP fill with color
    float fw=bw*hp;
    sf::RectangleShape fill({fw,bh});
    fill.setPosition(right?p.x+bw-fw:p.x, p.y);
    sf::Color hc=hp>.6f?sf::Color(40,200,50):(hp>.3f?sf::Color(240,180,20):sf::Color(220,35,35));
    fill.setFillColor(hc); w.draw(fill);
    // HP shine
    sf::RectangleShape shine({fw,3}); shine.setPosition(right?p.x+bw-fw:p.x,p.y);
    shine.setFillColor(sf::Color(255,255,255,40)); w.draw(shine);
    // Energy bar
    sf::RectangleShape eb({bw,6}); eb.setPosition(p.x,p.y+bh+3); eb.setFillColor(sf::Color(20,20,35,180)); w.draw(eb);
    float ew2=bw*en;
    sf::RectangleShape ef({ew2,6}); ef.setPosition(right?p.x+bw-ew2:p.x,p.y+bh+3);
    ef.setFillColor(sf::Color(60,160,240)); w.draw(ef);
    sf::RectangleShape eshine({ew2,2}); eshine.setPosition(right?p.x+bw-ew2:p.x,p.y+bh+3);
    eshine.setFillColor(sf::Color(150,220,255,60)); w.draw(eshine);
}

} // namespace CBG
