#include "Game.h"
#include "Fighter.h"
#include "Attack.h"
#include "ParticleSystem.h"
#include "InputHandler.h"
#include <cmath>
#include <cstdlib>

Game::Game() 
    : mWindow(sf::VideoMode(800,600),"Elemental Fighters"), 
      mState(GameState::TitleScreen), mP1SelectIndex(0), mP2SelectIndex(1),
      mShakeTime(0), mShakeMagnitude(0), mFontLoaded(false),
      mMenuChoice(0), mSinglePlayer(false), mCpuActionTimer(0), mCpuSpecialTimer(0),
      mP1Wins(0), mP2Wins(0), mRound(1), mFightTimer(90.f), mRoundEndTimer(0)
{
    mWindow.setFramerateLimit(60);
    mDefaultView=mWindow.getDefaultView();
    mCharacterColors={sf::Color::Yellow,sf::Color::Red,sf::Color::Blue,sf::Color::Green};
    mParticleSystem=std::make_unique<ParticleSystem>();
    if(mFont.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")||
       mFont.loadFromFile("C:\\Windows\\Fonts\\segoeui.ttf"))
        mFontLoaded=true;
}
Game::~Game()=default;
void Game::addAttack(std::unique_ptr<Attack> a){ mAttacks.push_back(std::move(a)); }

void Game::run(){
    sf::Clock clock;
    while(mWindow.isOpen()){
        sf::Time dt=clock.restart();
        processEvents(); update(dt); render();
    }
}

void Game::processEvents(){
    sf::Event ev;
    while(mWindow.pollEvent(ev)){
        if(ev.type==sf::Event::Closed) mWindow.close();
        if(ev.type!=sf::Event::KeyPressed) continue;
        
        if(mState==GameState::TitleScreen){
            if(ev.key.code==sf::Keyboard::W||ev.key.code==sf::Keyboard::Up) mMenuChoice=0;
            if(ev.key.code==sf::Keyboard::S||ev.key.code==sf::Keyboard::Down) mMenuChoice=1;
            if(ev.key.code==sf::Keyboard::Enter){
                mSinglePlayer=(mMenuChoice==0);
                mState=GameState::CharacterSelect;
                if(mSinglePlayer) mP2SelectIndex=rand()%10; // CPU picks random
            }
        }
        else if(mState==GameState::CharacterSelect){
            if(ev.key.code==sf::Keyboard::A) mP1SelectIndex=(mP1SelectIndex+9)%10;
            if(ev.key.code==sf::Keyboard::D) mP1SelectIndex=(mP1SelectIndex+1)%10;
            if(!mSinglePlayer){
                if(ev.key.code==sf::Keyboard::Left) mP2SelectIndex=(mP2SelectIndex+9)%10;
                if(ev.key.code==sf::Keyboard::Right) mP2SelectIndex=(mP2SelectIndex+1)%10;
            }
            if(ev.key.code==sf::Keyboard::Enter){ resetGame(); mState=GameState::Playing; }
            if(ev.key.code==sf::Keyboard::Escape) mState=GameState::TitleScreen;
        }
        else if(mState==GameState::GameOver){
            if(ev.key.code==sf::Keyboard::Enter){
                mP1Wins=0; mP2Wins=0; mRound=1; mFightTimer=90.f;
                mState=GameState::CharacterSelect;
            }
            if(ev.key.code==sf::Keyboard::Escape){
                mP1Wins=0; mP2Wins=0; mRound=1; mFightTimer=90.f;
                mState=GameState::TitleScreen;
            }
        }
    }
}

void Game::update(sf::Time dt){
    if(mState==GameState::Playing) updatePlaying(dt);
}
void Game::updateCharacterSelect(sf::Time){}

// --- CPU AI ---
void Game::updateCPU(sf::Time deltaTime){
    if(!mSinglePlayer||!mPlayer2) return;
    float dt=deltaTime.asSeconds();
    mCpuActionTimer-=dt; mCpuSpecialTimer-=dt;
    
    sf::Vector2f p1=mPlayer1->getPosition(), p2=mPlayer2->getPosition();
    float dist=p1.x-p2.x;
    float absDist=std::abs(dist);
    int dir=dist>0?1:-1;
    
    // Face the player
    mPlayer2->setFacing(dir);
    // Movement: approach but keep fighting distance
    if(absDist>150){
        mPlayer2->setVelX(dir*280);
    } else if(absDist<70){
        mPlayer2->setVelX(-dir*200); // Back off
    } else {
        mPlayer2->setVelX(0);
    }
    
    // Attack when in range
    if(mCpuActionTimer<=0 && absDist<180){
        mCpuActionTimer=0.3f+(rand()%4)*0.15f;
        int action=rand()%10;
        if(action<5) mPlayer2->cpuAttack();        // 50% normal
        else if(action<7) mPlayer2->cpuSpecial();   // 20% special
        else if(action<9) mPlayer2->cpuBlock();     // 20% block
        else mPlayer2->cpuUltimate();               // 10% ultimate
    }
    
    // Jump to dodge or reposition
    if(rand()%80==0) mPlayer2->cpuJump();
    
    // Use special timer for bigger moves
    if(mCpuSpecialTimer<=0 && absDist<250 && mPlayer2->getEnergy()>40){
        mCpuSpecialTimer=3.0f+(rand()%3);
        mPlayer2->cpuUltimate();
    }
}

void Game::updatePlaying(sf::Time deltaTime){
    mPlayer1->update(deltaTime);
    mPlayer2->update(deltaTime);
    
    // CPU AI
    if(mSinglePlayer) updateCPU(deltaTime);
    
    for(auto it=mAttacks.begin();it!=mAttacks.end();){
        (*it)->update(deltaTime,*mParticleSystem);
        sf::FloatRect ab=(*it)->getHitbox();
        if((*it)->getOwner()!=mPlayer1.get()&&ab.intersects(mPlayer1->getHitbox())){
            mPlayer1->takeDamage((*it)->getDamage(),(*it)->getKnockback().x,(*it)->getKnockback().y);
            (*it)->destroy(); mShakeTime=0.15f; mShakeMagnitude=4;
        } else if((*it)->getOwner()!=mPlayer2.get()&&ab.intersects(mPlayer2->getHitbox())){
            mPlayer2->takeDamage((*it)->getDamage(),(*it)->getKnockback().x,(*it)->getKnockback().y);
            (*it)->destroy(); mShakeTime=0.15f; mShakeMagnitude=4;
        }
        if((*it)->isDead()) it=mAttacks.erase(it); else ++it;
    }
    mParticleSystem->update(deltaTime);
    
    if(mShakeTime>0){
        mShakeTime-=deltaTime.asSeconds();
        float ox=(rand()%100-50)/50.f*mShakeMagnitude;
        float oy=(rand()%100-50)/50.f*mShakeMagnitude;
        sf::View sv=mDefaultView; sv.move(ox,oy); mWindow.setView(sv);
    } else { mWindow.setView(mDefaultView); }
    
    // Fight timer
    mFightTimer-=deltaTime.asSeconds();
    if(mFightTimer<=0) mFightTimer=0;
    
    // Round end
    if(mPlayer1->isDead()||mPlayer2->isDead()||mFightTimer<=0){
        if(mRoundEndTimer<=0){
            mRoundEndTimer=2.0f; // 2 second delay
            if(mPlayer1->isDead()) mP2Wins++;
            else mP1Wins++;
        }
        mRoundEndTimer-=deltaTime.asSeconds();
        if(mRoundEndTimer<=0){
            if(mP1Wins>=2||mP2Wins>=2){
                mState=GameState::GameOver;
            } else {
                mRound++;
                mFightTimer=90.f;
                mRoundEndTimer=0;
                resetGame();
            }
        }
    }
}

void Game::render(){
    mWindow.clear();
    if(mState==GameState::TitleScreen) renderTitleScreen();
    else if(mState==GameState::CharacterSelect) renderCharacterSelect();
    else renderPlaying();
    mWindow.display();
}

// ===== TITLE SCREEN =====
void Game::renderTitleScreen(){
    float t=mGameClock.getElapsedTime().asSeconds();
    
    // Night sky
    sf::RectangleShape bg({800,600}); bg.setFillColor(sf::Color(5,5,18)); mWindow.draw(bg);
    // Nebula
    sf::CircleShape n1(220,30); n1.setOrigin(220,220); n1.setPosition(200,300);
    n1.setFillColor(sf::Color(35,10,55,20)); mWindow.draw(n1);
    sf::CircleShape n2(180,30); n2.setOrigin(180,180); n2.setPosition(600,200);
    n2.setFillColor(sf::Color(10,15,45,25)); mWindow.draw(n2);
    // Stars
    srand(99);
    for(int i=0;i<100;i++){
        float sx=rand()%800,sy=rand()%600;
        float tw=(std::sin(t*2+i*1.3f)+1)*.5f;
        sf::CircleShape s(0.7f+tw*1.3f,5); s.setOrigin(s.getRadius(),s.getRadius()); s.setPosition(sx,sy);
        s.setFillColor(sf::Color(220,220,255,(sf::Uint8)(60+195*tw))); mWindow.draw(s);
    }
    srand((unsigned)(t*1000));
    // Moon
    sf::CircleShape moon(40,30); moon.setOrigin(40,40); moon.setPosition(650,90);
    moon.setFillColor(sf::Color(210,215,235)); mWindow.draw(moon);
    sf::CircleShape ms(37,30); ms.setOrigin(37,37); ms.setPosition(668,82);
    ms.setFillColor(sf::Color(5,5,18)); mWindow.draw(ms);
    sf::CircleShape mg(65,30); mg.setOrigin(65,65); mg.setPosition(650,90);
    mg.setFillColor(sf::Color(160,180,230,12)); mWindow.draw(mg);

    // 4 element orbs floating
    sf::Color ec[]={sf::Color(100,180,240),sf::Color(255,100,20),sf::Color(40,140,240),sf::Color(60,170,50)};
    for(int i=0;i<4;i++){
        float a=t*.8f+i*1.57f;
        float ox=400+std::cos(a)*180, oy=220+std::sin(a)*60;
        // Glow
        sf::CircleShape gl(20,20); gl.setOrigin(20,20); gl.setPosition(ox,oy);
        gl.setFillColor(sf::Color(ec[i].r,ec[i].g,ec[i].b,25)); mWindow.draw(gl);
        // Orb
        sf::CircleShape orb(8,20); orb.setOrigin(8,8); orb.setPosition(ox,oy);
        orb.setFillColor(sf::Color(ec[i].r,ec[i].g,ec[i].b,160)); mWindow.draw(orb);
        // Core
        sf::CircleShape core(3,12); core.setOrigin(3,3); core.setPosition(ox,oy);
        core.setFillColor(sf::Color(255,255,255,120)); mWindow.draw(core);
    }

    if(mFontLoaded){
        // Title
        sf::Text title("ELEMENTAL  FIGHTERS",mFont,42);
        title.setFillColor(sf::Color(255,220,80));
        title.setOutlineColor(sf::Color(160,80,0)); title.setOutlineThickness(3);
        title.setStyle(sf::Text::Bold);
        sf::FloatRect tb=title.getLocalBounds(); title.setOrigin(tb.width/2,tb.height/2);
        title.setPosition(400,120+std::sin(t*1.2f)*4); mWindow.draw(title);

        // Decorative lines
        float lw=350+std::sin(t)*20;
        sf::RectangleShape line1({lw,2}); line1.setOrigin(lw/2,1); line1.setPosition(400,160);
        line1.setFillColor(sf::Color(255,200,50,80)); mWindow.draw(line1);

        // Menu options
        for(int i=0;i<2;i++){
            float my=340+i*70;
            bool sel=(i==mMenuChoice);
            float pulse=sel?std::sin(t*4)*.5f+.5f:0;
            
            // Option box
            sf::RectangleShape box({280+pulse*20, 50+pulse*6});
            box.setOrigin(box.getSize().x/2, box.getSize().y/2);
            box.setPosition(400,my);
            box.setFillColor(sel?sf::Color(25,25,45):sf::Color(12,12,22));
            box.setOutlineThickness(sel?2.5f:1);
            box.setOutlineColor(sel?sf::Color(255,200,50,(sf::Uint8)(150+105*pulse)):sf::Color(50,50,70));
            mWindow.draw(box);
            
            // Selection arrow
            if(sel){
                sf::ConvexShape arr; arr.setPointCount(3);
                arr.setPoint(0,{0,-8}); arr.setPoint(1,{12,0}); arr.setPoint(2,{0,8});
                arr.setPosition(400-120-10+std::sin(t*6)*5,my);
                arr.setFillColor(sf::Color(255,200,50)); mWindow.draw(arr);
            }
            
            const char* labels[]={"1  PLAYER","2  PLAYERS"};
            sf::Text opt(labels[i],mFont,20);
            opt.setFillColor(sel?sf::Color(255,220,80):sf::Color(120,120,140));
            if(sel) opt.setStyle(sf::Text::Bold);
            sf::FloatRect ob=opt.getLocalBounds(); opt.setOrigin(ob.width/2,ob.height/2);
            opt.setPosition(400,my-2); mWindow.draw(opt);
            
            // Subtitle
            const char* subs[]={"vs CPU","Local Multiplayer"};
            sf::Text sub(subs[i],mFont,10);
            sub.setFillColor(sf::Color(80,80,100));
            sf::FloatRect sb=sub.getLocalBounds(); sub.setOrigin(sb.width/2,sb.height/2);
            sub.setPosition(400,my+18); mWindow.draw(sub);
        }

        // Bottom hint
        float p2=(std::sin(t*3)+1)*.5f;
        sf::Text hint("PRESS  ENTER  TO  SELECT",mFont,13);
        hint.setFillColor(sf::Color(180,180,200,(sf::Uint8)(100+155*p2)));
        sf::FloatRect hb=hint.getLocalBounds(); hint.setOrigin(hb.width/2,hb.height/2);
        hint.setPosition(400,530); mWindow.draw(hint);

        sf::Text esc("ESC - Back",mFont,10);
        esc.setFillColor(sf::Color(60,60,80)); esc.setPosition(10,580); mWindow.draw(esc);
    }
}

// ===== CHARACTER SELECT =====
void Game::renderCharacterSelect(){
    float t=mGameClock.getElapsedTime().asSeconds();
    Element elements[]={Element::Air,Element::Fire,Element::Water,Element::Earth,Element::Metal,Element::Lightning,Element::Plant,Element::Ice,Element::Sword,Element::Fan};
    const char* names[]={"AIR","FIRE","WATER","EARTH","METAL","THUNDER","PLANT","ICE","SWORD","WAR FAN"};
    const char* titles[]={"The Monk","The Prince","The Healer","The Champion","The Guard","The Storm","The Swamp","The Freeze","The Warrior","The Leader"};
    sf::Color elCols[]={sf::Color(100,180,240),sf::Color(255,80,20),sf::Color(40,140,240),sf::Color(60,160,50),sf::Color(140,140,150),sf::Color(255,220,50),sf::Color(80,180,60),sf::Color(160,220,255),sf::Color(150,170,190),sf::Color(255,200,80)};

    // Night sky
    sf::RectangleShape bg({800,600}); bg.setFillColor(sf::Color(5,5,18)); mWindow.draw(bg);
    sf::CircleShape neb1(200,30); neb1.setOrigin(200,200); neb1.setPosition(150,200);
    neb1.setFillColor(sf::Color(40,10,60,25)); mWindow.draw(neb1);
    sf::CircleShape neb2(180,30); neb2.setOrigin(180,180); neb2.setPosition(650,350);
    neb2.setFillColor(sf::Color(10,20,50,30)); mWindow.draw(neb2);
    srand(42);
    for(int i=0;i<80;i++){
        float sx=rand()%800,sy=rand()%600;
        float tw=(std::sin(t*2.5f+i*1.7f)+1)*.5f;
        sf::CircleShape star(0.8f+tw*1.2f,6); star.setOrigin(star.getRadius(),star.getRadius()); star.setPosition(sx,sy);
        star.setFillColor(sf::Color(220,220,255,(sf::Uint8)(80+175*tw))); mWindow.draw(star);
    }
    srand((unsigned)t);
    // Moon
    sf::CircleShape moon(35,30); moon.setOrigin(35,35); moon.setPosition(680,80);
    moon.setFillColor(sf::Color(220,220,240)); mWindow.draw(moon);
    sf::CircleShape msh(32,30); msh.setOrigin(32,32); msh.setPosition(695,75);
    msh.setFillColor(sf::Color(5,5,18)); mWindow.draw(msh);

    if(mFontLoaded){
        sf::Text title("CHOOSE YOUR BENDER",mFont,28);
        title.setFillColor(sf::Color(255,220,80)); title.setOutlineThickness(2); title.setOutlineColor(sf::Color(150,80,0));
        title.setStyle(sf::Text::Bold);
        sf::FloatRect tb=title.getLocalBounds(); title.setOrigin(tb.width/2,tb.height/2);
        title.setPosition(400,50); mWindow.draw(title);
        
        sf::Text mode(mSinglePlayer?"1 PLAYER  vs  CPU":"2 PLAYERS  LOCAL",mFont,12);
        mode.setFillColor(sf::Color(140,140,160));
        sf::FloatRect mb=mode.getLocalBounds(); mode.setOrigin(mb.width/2,mb.height/2);
        mode.setPosition(400,80); mWindow.draw(mode);
    }

    // Character cards
    float cardW=74, cardH=250;
    float totalW=10*cardW+9*6; // 10 cards with 6px spacing
    float startX=(800-totalW)/2+cardW/2;

    for(int i=0;i<10;i++){
        float cx=startX+i*(cardW+6), cy=230;
        bool p1=(i==mP1SelectIndex), p2=(i==mP2SelectIndex);
        bool sel=p1||p2;

        // Glow
        if(sel){
            float gp=std::sin(t*4)*6;
            sf::RectangleShape glow({cardW+16+gp,cardH+16+gp});
            glow.setOrigin(glow.getSize().x/2,glow.getSize().y/2); glow.setPosition(cx,cy);
            glow.setFillColor(p1?sf::Color(255,200,40,30):sf::Color(80,180,255,30)); mWindow.draw(glow);
        }
        // Card bg
        sf::RectangleShape card({cardW,cardH}); card.setOrigin(cardW/2,cardH/2); card.setPosition(cx,cy);
        card.setFillColor(sel?sf::Color(20,20,35):sf::Color(10,10,18));
        card.setOutlineThickness(sel?3:1);
        card.setOutlineColor(sel?elCols[i]:sf::Color(40,40,55)); mWindow.draw(card);
        
        // Color strip
        sf::RectangleShape strip({cardW-4,3}); strip.setOrigin((cardW-4)/2,1.5f);
        strip.setPosition(cx,cy-cardH/2+6); strip.setFillColor(elCols[i]); mWindow.draw(strip);

        // Full character in card
        mCartoon.drawPortrait(mWindow, sf::Vector2f(cx,cy-10), elements[i], sel, p1?0:(p2?1:-1), t);

        if(mFontLoaded){
            sf::Text nm(names[i],mFont,15); nm.setFillColor(elCols[i]); nm.setStyle(sf::Text::Bold);
            sf::FloatRect nb=nm.getLocalBounds(); nm.setOrigin(nb.width/2,nb.height/2);
            nm.setPosition(cx,cy+75); mWindow.draw(nm);
            sf::Text ti(titles[i],mFont,9); ti.setFillColor(sf::Color(120,120,140));
            sf::FloatRect tib=ti.getLocalBounds(); ti.setOrigin(tib.width/2,tib.height/2);
            ti.setPosition(cx,cy+93); mWindow.draw(ti);
        }
        // P1/P2 tags
        if(p1){
            sf::RectangleShape tag({36,16}); tag.setOrigin(18,8);
            tag.setPosition(cx-25,cy-cardH/2-12); tag.setFillColor(sf::Color(255,200,40)); mWindow.draw(tag);
            if(mFontLoaded){ sf::Text pt("P1",mFont,10); pt.setFillColor(sf::Color(20,20,20)); pt.setStyle(sf::Text::Bold);
                sf::FloatRect pb=pt.getLocalBounds(); pt.setOrigin(pb.width/2,pb.height/2); pt.setPosition(cx-25,cy-cardH/2-14); mWindow.draw(pt); }
        }
        if(p2){
            sf::RectangleShape tag({36,16}); tag.setOrigin(18,8);
            tag.setPosition(cx+25,cy-cardH/2-12); tag.setFillColor(mSinglePlayer?sf::Color(200,60,60):sf::Color(80,180,255)); mWindow.draw(tag);
            if(mFontLoaded){ sf::Text pt(mSinglePlayer?"CPU":"P2",mFont,10); pt.setFillColor(sf::Color(255,255,255)); pt.setStyle(sf::Text::Bold);
                sf::FloatRect pb=pt.getLocalBounds(); pt.setOrigin(pb.width/2,pb.height/2); pt.setPosition(cx+25,cy-cardH/2-14); mWindow.draw(pt); }
        }
    }

    // Controls panel
    if(mFontLoaded){
        sf::RectangleShape panel({700,80}); panel.setOrigin(350,0); panel.setPosition(400,400);
        panel.setFillColor(sf::Color(10,10,20,200)); panel.setOutlineThickness(1); panel.setOutlineColor(sf::Color(40,40,55)); mWindow.draw(panel);
        sf::Text p1h("P1: A/D Select | WASD Move | J Atk | K Block | L Special | I Ult",mFont,10);
        p1h.setFillColor(sf::Color(255,200,40,180)); p1h.setPosition(65,410); mWindow.draw(p1h);
        if(!mSinglePlayer){
            sf::Text p2h("P2: Arrows Select | Arrows Move | Num1 Atk | Num2 Block | Num3 Spc | Num0 Ult",mFont,10);
            p2h.setFillColor(sf::Color(80,180,255,180)); p2h.setPosition(65,430); mWindow.draw(p2h);
        } else {
            sf::Text cpu("CPU will fight automatically!",mFont,10);
            cpu.setFillColor(sf::Color(200,80,80,180)); cpu.setPosition(65,430); mWindow.draw(cpu);
        }
        float p=(std::sin(t*3)+1)*.5f;
        sf::Text start("PRESS  ENTER  TO  FIGHT",mFont,15);
        start.setFillColor(sf::Color(255,220,80,(sf::Uint8)(120+135*p))); start.setStyle(sf::Text::Bold);
        sf::FloatRect sb=start.getLocalBounds(); start.setOrigin(sb.width/2,sb.height/2);
        start.setPosition(400,468); mWindow.draw(start);
        sf::Text esc("ESC - Back",mFont,10); esc.setFillColor(sf::Color(60,60,80)); esc.setPosition(10,580); mWindow.draw(esc);
    }
}

// ===== GAMEPLAY =====
void Game::renderPlaying(){
    float t=mGameClock.getElapsedTime().asSeconds();
    mCartoon.drawBackground(mWindow,t);
    mCartoon.drawElementalAura(mWindow,mPlayer1->getPosition(),mPlayer1->getElement(),t);
    mCartoon.drawElementalAura(mWindow,mPlayer2->getPosition(),mPlayer2->getElement(),t);
    mCartoon.drawCharacter(mWindow,mPlayer1->getPosition(),mPlayer1->getElement(),
        mPlayer1->getAnimState(),mPlayer1->getFacing(),t,mPlayer1->getFlashTimer(),
        mPlayer1->getHP()/mPlayer1->getMaxHP(),mPlayer1->isInAirBall(),mPlayer1->getAirBallTimer());
    mCartoon.drawCharacter(mWindow,mPlayer2->getPosition(),mPlayer2->getElement(),
        mPlayer2->getAnimState(),mPlayer2->getFacing(),t,mPlayer2->getFlashTimer(),
        mPlayer2->getHP()/mPlayer2->getMaxHP(),mPlayer2->isInAirBall(),mPlayer2->getAirBallTimer());
    for(auto& a:mAttacks) a->render(mWindow);
    mParticleSystem->render(mWindow);
    mCartoon.drawHPBar(mWindow,{20,12},mPlayer1->getHP()/mPlayer1->getMaxHP(),
        mPlayer1->getEnergy()/mPlayer1->getMaxEnergy(),mPlayer1->getElement(),false);
    mCartoon.drawHPBar(mWindow,{480,12},mPlayer2->getHP()/mPlayer2->getMaxHP(),
        mPlayer2->getEnergy()/mPlayer2->getMaxEnergy(),mPlayer2->getElement(),true);
    if(mFontLoaded){
        const char* en[]={"AIR","FIRE","WATER","EARTH","METAL","THUNDER"};
        sf::Text n1(en[(int)mPlayer1->getElement()],mFont,11);
        n1.setFillColor(sf::Color(255,220,80)); n1.setPosition(22,36); mWindow.draw(n1);
        sf::Text n2(mSinglePlayer?"CPU":"P2",mFont,11);
        n2.setFillColor(sf::Color(80,180,255)); n2.setPosition(750,36); mWindow.draw(n2);

        // Timer display (center top)
        int secs=(int)mFightTimer;
        sf::Text timer(std::to_string(secs),mFont,28);
        timer.setFillColor(secs<=10?sf::Color(255,60,60):sf::Color(220,220,240));
        timer.setStyle(sf::Text::Bold);
        sf::FloatRect ttb=timer.getLocalBounds(); timer.setOrigin(ttb.width/2,ttb.height/2);
        timer.setPosition(400,18); mWindow.draw(timer);

        // Round indicator
        sf::Text rnd("Round " + std::to_string(mRound),mFont,10);
        rnd.setFillColor(sf::Color(160,160,180));
        sf::FloatRect rnb=rnd.getLocalBounds(); rnd.setOrigin(rnb.width/2,rnb.height/2);
        rnd.setPosition(400,46); mWindow.draw(rnd);

        // Round win dots P1
        for(int i=0;i<2;i++){
            sf::CircleShape dot(5); dot.setOrigin(5,5);
            dot.setPosition(170+i*14,22);
            dot.setFillColor(i<mP1Wins?sf::Color(255,200,40):sf::Color(40,40,50));
            dot.setOutlineThickness(1); dot.setOutlineColor(sf::Color(100,100,120));
            mWindow.draw(dot);
        }
        // Round win dots P2
        for(int i=0;i<2;i++){
            sf::CircleShape dot(5); dot.setOrigin(5,5);
            dot.setPosition(616+i*14,22);
            dot.setFillColor(i<mP2Wins?sf::Color(80,180,255):sf::Color(40,40,50));
            dot.setOutlineThickness(1); dot.setOutlineColor(sf::Color(100,100,120));
            mWindow.draw(dot);
        }
    }
    if(mState==GameState::GameOver){
        sf::RectangleShape ov({800,600}); ov.setFillColor(sf::Color(0,0,0,180)); mWindow.draw(ov);
        if(mFontLoaded){
            std::string w=mP1Wins>=2?(mSinglePlayer?"YOU  WIN!":"PLAYER 1  WINS!"):(mSinglePlayer?"CPU  WINS!":"PLAYER 2  WINS!");
            sf::Text wt(w,mFont,44); wt.setStyle(sf::Text::Bold);
            wt.setFillColor(mP1Wins>=2?sf::Color(255,220,80):sf::Color(200,60,60));
            wt.setOutlineColor(sf::Color(0,0,0)); wt.setOutlineThickness(3);
            sf::FloatRect wb=wt.getLocalBounds(); wt.setOrigin(wb.width/2,wb.height/2);
            wt.setPosition(400,240); mWindow.draw(wt);

            // Score display
            std::string score=std::to_string(mP1Wins)+" - "+std::to_string(mP2Wins);
            sf::Text sc(score,mFont,30); sc.setFillColor(sf::Color(200,200,220));
            sf::FloatRect sb=sc.getLocalBounds(); sc.setOrigin(sb.width/2,sb.height/2);
            sc.setPosition(400,290); mWindow.draw(sc);

            float p=(std::sin(t*3)+1)*.5f;
            sf::Text r("ENTER - Rematch  |  ESC - Menu",mFont,14);
            r.setFillColor(sf::Color(200,200,220,(sf::Uint8)(100+155*p)));
            sf::FloatRect rb=r.getLocalBounds(); r.setOrigin(rb.width/2,rb.height/2);
            r.setPosition(400,340); mWindow.draw(r);
        }
    }
}

void Game::resetGame(){
    Element e1=static_cast<Element>(mP1SelectIndex);
    Element e2=static_cast<Element>(mP2SelectIndex);
    mPlayer1=std::make_unique<Fighter>(e1,sf::Vector2f(200,400),InputHandler::getPlayer1Controls(),this);
    mPlayer2=std::make_unique<Fighter>(e2,sf::Vector2f(600,400),InputHandler::getPlayer2Controls(),this);
    if(mSinglePlayer) mPlayer2->setCpuMode(true);
    mAttacks.clear();
    mCpuActionTimer=0; mCpuSpecialTimer=2.0f;
    mRoundEndTimer=0;
}
