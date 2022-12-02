#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <time.h>
#include <vector>

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

class tail{
public:
    int x;
    int y;
    int gs;
    int lt;
    int alpha=250;
    bool alive;
    SDL_Color col;
    bool deleteme = false;

    tail(int sX, int sY, int gridSize, int liveTime){
        x = sX;
        y = sY;
        lt = liveTime;
        gs = gridSize;
        col.r = 100+ rand()%100;
        col.g = 100+ rand()%100;
        col.b = 100+ rand()%100;
        col.a = 255;
        alive = true;
    }

    bool live(int cGS){
        //lt--;
        if(--lt <= 0 || cGS < gs){
            alive = false;
            return false;
        }
        return true;
    }

    int distance(int x1, int y1, int x2, int y2){
        return (abs(x2 - x1) + abs(y2 - y1));
    }

    void draw(){
        if(alive==false){
            if((alpha-=10) <= 0){
                alpha = 0;
                deleteme = true;
            }
        }
        SDL_SetRenderDrawColor(gRenderer, col.r,col.g,col.b,alpha);
        if(gs<=2){
            SDL_RenderDrawPoint(gRenderer, x,y);
        }else{
            SDL_Rect r;
            r.x = x-gs/2;
            r.y = y-gs/2;
            r.w = gs;
            r.h = gs;
            for(int i=r.x; i<r.x+r.w; i++){
                for(int j=r.y; j<r.y+r.h; j++){
                    if(rand()%(int)((float)gs/1.5f) > distance(x,y,i,j)){
                        SDL_RenderDrawPoint(gRenderer, i, j);
                    }
                }
            }
        }

    }
};

