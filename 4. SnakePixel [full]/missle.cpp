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

class missle{
public:
    int x;
    int y;
    int dir;
    int gs;
    bool alive = true;
    SDL_Color col;
    missle(int sX, int sY, int sDir, int gridSize){
        x = sX;
        y = sY;
        dir = sDir;
        gs = gridSize;
        col.r = 100+ rand()%100;
        col.g = 100+ rand()%100;
        col.b = 100+ rand()%100;
        col.a = 0xF0;
    }

    void mMove(){
        if(dir == 0){ // UP
            y-= gs;
        } else
        if(dir == 1){ // DOWN
            y+= gs;
        } else
        if(dir == 2){ // LEFT
            x-= gs;
        } else{        // RIGHT
            x+= gs;
        }
        if(y<0 || x<0 || x>SCREEN_WIDTH || y>SCREEN_HEIGHT){
            alive = false;
        }
    }

    void draw(){

        SDL_SetRenderDrawColor(gRenderer, col.r, col.g, col.b, col.a);
        SDL_RenderDrawPoint( gRenderer, x, y);

    }
};
