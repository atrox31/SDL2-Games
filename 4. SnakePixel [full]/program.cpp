#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <time.h>
#include <vector>
#include "tail.cpp"
#include "font.cpp"
#include "AI.cpp"
#include <sstream>

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;
extern bool quit;
extern int GetValue();
extern void SetValue(string);
//SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
//SDL_RenderDrawPoint( gRenderer, a,b );

struct pos2D {
    int x;
    int y;
};

enum direction{
    dUp,dDown,dLeft,dRight,dNone
};

class program {
    string toString(int l){
        stringstream a;
        a<<l;
        return a.str();
    };
    void DrawCircle(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius)
    {
       const int32_t diameter = (radius * 2);

       int32_t x = (radius - 1);
       int32_t y = 0;
       int32_t tx = 1;
       int32_t ty = 1;
       int32_t error = (tx - diameter);

       while (x >= y)
       {
          //  Each of the following renders an octant of the circle
          SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
          SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
          SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
          SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
          SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
          SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
          SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
          SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);
          if(radius > 4){
              SDL_RenderDrawPoint(renderer, centreX + ((-radius/2)+rand()%radius), centreY + ((-radius/2)+rand()%radius));
          }

          if (error <= 0)
          {
             ++y;
             error += ty;
             ty += 2;
          }

          if (error > 0)
          {
             --x;
             tx += 2;
             error += (tx - diameter);
          }
       }
    }

    int Vmin(int a, int b){
        return a>b?b:a;
    }
    int Vmax(int a, int b){
        return a>b?a:b;
    }
    int vBet(int vMin, int vMax, int vVal){
        return Vmin(Vmax(vMin, vVal), vMax);
    }
    void preapare(){
        tailToCreate = 0;
        allow_warp = true;
        tailSize = 3;
        tails.clear();
        gridSize = gridSize_start;

        newPoint();
        pos.x = ((SCREEN_WIDTH/2) / gridSize) * gridSize;
        pos.y = ((SCREEN_HEIGHT/2) / gridSize) * gridSize;
        startMe = false;
        mainMenu = true;
        score=0;
        candie=true;
        frames_max = 10;
    }

    void newPoint(){
        pointRadious = 20;
        bool roll = true;
        while(roll){
            roll = false;
            point.x = (1+rand()%((SCREEN_WIDTH-gridSize)/gridSize))*gridSize;
            point.y = (1+rand()%((SCREEN_HEIGHT-gridSize)/gridSize))*gridSize;
            if(tails.size() > 0){
                for(int m = 0; m<tails.size(); m++){
                    if(tails.at(m)->x == point.x &&  tails.at(m)->y == point.y){
                        roll = true;
                    }
                }
            }
            if(point.x == pos.x && point.y == pos.y){
                roll = true;
            }
        }
    }

public:
    pos2D       pos;
    int         gridSize;
    int         gridSize_start = 50;
    bool        allow_warp;
    bool        alwaysallow_warp = false;
    int         tailSize = 1;
    direction   dir;
    pos2D       point;
    bool        startMe;
    int         score;
    int         bscore = 0;

    int         frames = 0;
    int         frames_max = 10;
    int         frames_bonus = 10;
    font        Font;
    std::vector<tail*> tails;
    int menuSelect = 0;
    int minMenuSelect = 0;
    int maxMenuSelect = 1;
    bool win = false;
    bool mainMenu = true;
    bool bonusMenu = false;
    bool candie = true;
    int secred;
    const float aa=1.2f;
    int pointRadious;
    int tailToCreate;

        void start(){
            srand(time(NULL));
            preapare();
            Font.setup();
            secred=0;
            bscore = GetValue();
            if(bscore>100){
                maxMenuSelect = 2;
            }
        }

    int distance(int x1, int y1, int x2, int y2){
        return (abs(x2 - x1) + abs(y2 - y1));
    }
        void loop(){
            if(mainMenu){
                SDL_Color col = {0x10,0x10,0x10,0x85};

                if(rand()%100>2){
                    col.r = 0xFF;
                    Font.draw(8,18,"pixel snake",7, col, 6);
                }
                if(rand()%100>2){
                    col.g = 0xFF;
                    Font.draw(10,20,"pixel snake",7, col, 6);
                }
                if(rand()%100>2){
                    col.b = 0xFF;
                    Font.draw(12,22,"pixel snake",7, col, 6);
                }

                col = {0xDD,0x11,0x11,0xFF};
                Font.draw(10,480,"made by atrox games 2019",2, col, 10);
                if(bonusMenu){
                    if(menuSelect == 0) col.r = {0xDD};else col.r = {0x55};
                    Font.draw(10,100,"back",6, col, 8, (menuSelect==0?4:2));

                    if(menuSelect == 1) col.r = {0xDD};else col.r = {0x55};
                    Font.draw(10,200,"speed "+toString(20 - frames_bonus),6, col, 8, (menuSelect==1?4:2));

                    if(menuSelect == 2) col.r = {0xDD};else col.r = {0x55};
                    if(bscore <= 2000){
                        col.a = 100+rand()%50;
                        Font.draw(10,300,"2x0x0x0",6, col, 8, (menuSelect==2?4:2));
                    }else{
                        string wrapStr;
                        if(alwaysallow_warp){
                            Font.draw(10,300,"wrap on wall",5, col, 7, (menuSelect==2?4:2));
                        }else{
                            Font.draw(10,300,"block on wall",5, col, 7, (menuSelect==2?4:2));
                        }
                    }

                    if(menuSelect == 3) col.r = {0xDD};else col.r = {0x55};
                    if(bscore >= 3000){
                        Font.draw(10,400,"good mode",6, col, 8, (menuSelect==3?4:2));
                    }else{
                        col.a = 100+rand()%50;
                        Font.draw(10,400,"3x0x0x0",6, col, 8, (menuSelect==3?4:2));
                    }

                }else{

                    if(menuSelect == 0) col.r = {0xDD};else col.r = {0x55};
                    Font.draw(80,100,"start",8, col, 8,(menuSelect==0?3:4));

                    if(menuSelect == 1) col.r = {0xDD};else col.r = {0x55};
                    Font.draw(80,200,"exit",8, col, 8,(menuSelect==1?3:4));

                    if(maxMenuSelect == 2){
                        if(menuSelect == 2) col.r = {0xDD};else col.r = {0x55};
                        Font.draw(80,300,"bonus",8, col, 8,(menuSelect==2?3:4));
                    }

                    if(win){
                        col = {150+rand()%100,150+rand()%100,150+rand()%100,0xFF};
                        Font.draw(60,440,"you win pixel",5, col, 6);
                    }

                    col.r = {0xFF};
                    Font.draw(30,400,"best score "+toString(bscore+secred),4, col, 6, 2);
                }
            }else

            if(startMe){
                if(++frames >= frames_max + frames_bonus){
                    frames = 0;
                    // logic
                    tails.push_back(new tail(pos.x,pos.y,gridSize,tailSize));

                    switch(dir){
                        case dUp:    pos.y-= gridSize; break;
                        case dDown:  pos.y+= gridSize; break;
                        case dLeft:  pos.x-= gridSize; break;
                        case dRight: pos.x+= gridSize; break;
                    }

                    if(allow_warp || alwaysallow_warp){
                        if(pos.x < 0)  pos.x = (SCREEN_WIDTH/gridSize)*gridSize;
                        if(pos.y < 0)  pos.y = (SCREEN_HEIGHT/gridSize)*gridSize;
                        if(pos.x > SCREEN_WIDTH)   pos.x = gridSize;
                        if(pos.y > SCREEN_HEIGHT)   pos.y = gridSize;
                    }else{
                        if(pos.x < gridSize)  preapare();
                        if(pos.y < gridSize)  preapare();
                        if(pos.x > SCREEN_WIDTH) preapare();
                        if(pos.y > SCREEN_HEIGHT) preapare();
                    }


                    if(pos.x == point.x && pos.y == point.y){
                        tailSize++;
                        newPoint();
                        score+=(20 - frames_bonus) + (allow_warp?score*0.1f:0);
                        bscore = Vmax(score, (bscore==99999?score:bscore));
                        SetValue(toString(bscore));
                        if(tailSize%8 == 0){
                            frames_max = Vmax(1,frames_max-1);
                            if(frames_max + frames_bonus < 1){
                                frames_max = 1;
                            }
                        }
                        if(tailSize%4 == 0){
                            gridSize-=2;
                            if(gridSize<=1){
                                // win;
                                win = true;
                                preapare();
                                gridSize = 10;
                            }else{
                                newPoint();
                                pos.x = ((pos.x) / gridSize) * gridSize;
                                pos.y = ((pos.y) / gridSize) * gridSize;
                                if(gridSize <= gridSize_start * 0.75){
                                    allow_warp = false;
                                    maxMenuSelect = 2;
                                }
                            }
                        }
                        if(gridSize < 25){
                            tailSize+=30-frames_max;
                        }

                    }

                    // missle

                    if(tails.size() > 0){
                        for(int m = 0; m<tails.size(); m++){
                            if(tails.at(m)->live(gridSize)){
                                if(tails.at(m)->x == pos.x &&  tails.at(m)->y == pos.y){
                                    if(candie){
                                        preapare();
                                    };
                                }
                            }
                        }
                        for(int m = 0; m<tails.size(); m++){
                            if(tails.at(m)->deleteme){
                                tails.erase(tails.begin() + m);
                            }
                        }
                    }
                }
                if(tails.size() > 0){
                    for(int m = 0; m<tails.size(); m++){
                        tails.at(m)->draw();
                    }
                }

            }
            if(!mainMenu){
                // graph
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 220+rand()%30);
                    SDL_Rect r;
                    r.x = pos.x-gridSize/2;
                    r.y = pos.y-gridSize/2;
                    r.w = gridSize;
                    r.h = gridSize;
                    for(int i=r.x; i<r.x+r.w; i++){
                        for(int j=r.y; j<r.y+r.h; j++){
                            if( (rand()%(int)((float)gridSize/aa) > distance(pos.x,pos.y,i,j)) || gridSize<=2){
                                SDL_RenderDrawPoint(gRenderer, i, j);
                            }
                        }
                    }


                SDL_SetRenderDrawColor( gRenderer, 100+rand()%100, 100+rand()%100, 100+rand()%100, 0xFF );
                DrawCircle(gRenderer, point.x, point.y, pointRadious);

                // frame
                if(!allow_warp){
                    SDL_Rect rr;
                    for(int i=0; i<=gridSize/2; i++){
                        SDL_SetRenderDrawColor( gRenderer, (Uint8)250, (Uint8)20, (Uint8)20, (Uint8)(255-(i*10)) );
                        rr.x = i;
                        rr.y = i;
                        rr.w = SCREEN_WIDTH-gridSize/2;
                        rr.h = SCREEN_HEIGHT-gridSize/2;
                        SDL_RenderDrawRect( gRenderer, &rr );
                    }
                }
                Font.draw(5,5,"score " + toString(score), 4, {0xFF,0xFF,0xFF,0xFF}, 6, 3);
            }
            if(pointRadious > gridSize/2){
                pointRadious--;
            }
            SDL_Delay(20);
        }
        void scode(){
            bscore = 99999;
            secred = 0;
            win = true;
            maxMenuSelect = 2;
        }
        void KeyPressed(SDL_Keycode keysym){
            switch (keysym){
            case SDLK_ESCAPE: preapare(); break;
            case SDLK_p: if(secred==0) secred++; else secred = 0; break;
            case SDLK_i: if(secred==1) secred++; else secred = 0; break;
            case SDLK_x: if(secred==2) secred++; else secred = 0; break;
            case SDLK_e: if(secred==3) secred++; else secred = 0; break;
            case SDLK_l: if(secred==4) scode(); break;
            //case SDLK_z: pos.x = point.x; break;
            case SDLK_LEFT:case SDLK_a:   if(!mainMenu) {startMe = true; dir = dLeft;}  break;
            case SDLK_RIGHT:case SDLK_d:  if(!mainMenu) {startMe = true; dir = dRight;} break;
            case SDLK_UP:case SDLK_w:     if(!mainMenu) {startMe = true; dir = dUp;}    if(mainMenu) {menuSelect = Vmax(minMenuSelect, menuSelect-1);}   break;
            case SDLK_DOWN:case SDLK_s:   if(!mainMenu) {startMe = true; dir = dDown;}  if(mainMenu) {menuSelect = Vmin(maxMenuSelect, menuSelect+1);} break;
            case SDLK_RETURN:
                if(bonusMenu){
                    switch (menuSelect){
                        case 0: bonusMenu = false; maxMenuSelect = 2;   break;
                        case 1: frames_bonus++; if(frames_bonus>15) frames_bonus=-12; break;//speed
                        case 2: if(bscore >= 2000) {alwaysallow_warp = !alwaysallow_warp;} break;//wrap
                        case 3: if(bscore >= 3000){preapare(); mainMenu = false; candie=false; bonusMenu = false;maxMenuSelect = 2;menuSelect=minMenuSelect; }break;//xxxx
                    }
                }else{
                    switch (menuSelect){
                        case 0: preapare(); mainMenu = false;    break;
                        case 1: quit = true;         break;
                        case 2: bonusMenu = true; menuSelect=minMenuSelect; maxMenuSelect = 3; break;
                    }
                }
                break;
            default: secred = 0; break;
            }

        }

        void KeyRelased(SDL_Keycode keysym){

        }
};
