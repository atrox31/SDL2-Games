
//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <cmath>
#include <sstream>
#include <sys/stat.h>


#include "font.h"
#include "LTimer.h"
#include "program.cpp"

#define FPS_INTERVAL 1.0 //seconds.

Uint32 fps_lasttime = 0; //the last recorded time.
Uint32 fps_current  = 0; //the current FPS.
Uint32 fps_frames   = 0; //frames passed since the last recorded fps.

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

LTimer fpsTimer;

//The frames per second cap timer
LTimer capTimer;

//Start counting frames per second
int countedFrames = 0;
/*
string to_string( int* str ){
    char buffer [50];
    stringstream ss;
    ss << sprintf (buffer, "%d", str);
    return ss.str();
}
*/
bool init()
{
    fpsTimer.start();
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Gra w zycie v1.1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Nothing to load
	return success;
}

SDL_Texture* mTexture;
void close()
{
	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	SDL_DestroyTexture(mTexture);
	mTexture = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

//Main loop flag
bool quit = false;
int main(int argc, char* args[])
{
	program Program;
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Event handler
			SDL_Event e;
			Program.start();
			SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
            font	Font;
            Font.setup();

			//While application is running
			while (!quit)
			{
                capTimer.start();
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}else
					if (e.type == SDL_KEYDOWN) {
						Program.KeyPressed(e.key.keysym.sym);
					}else
					if (e.type == SDL_KEYUP) {
						Program.KeyRelased(e.key.keysym.sym);
					}else
					if (e.type == SDL_MOUSEBUTTONDOWN) {
						int mouseX, mouseY;
						SDL_GetMouseState(&mouseX, &mouseY);
						Program.MouseButtonDown(e.button, mouseX, mouseY);
					}else
					if (e.type == SDL_MOUSEBUTTONUP) {
						int mouseX, mouseY;
						SDL_GetMouseState(&mouseX, &mouseY);
						Program.MouseButtonUp(e.button, mouseX, mouseY);
					}
				}

				Program.loop();

                SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
                SDL_RenderClear(gRenderer);
                Program.draw();

                Font.draw(1,4,to_string(fps_current),4,{255,255,255,255}, 6);

                SDL_RenderPresent(gRenderer);

                //If frame finished early
                int frameTicks = capTimer.getTicks();
                if( frameTicks < SCREEN_TICKS_PER_FRAME )
                {
                    SDL_Delay( SCREEN_TICKS_PER_FRAME - frameTicks );
                }

                fps_frames++;
                if (fps_lasttime < SDL_GetTicks() - FPS_INTERVAL*1000)
                {
                   fps_lasttime = SDL_GetTicks();
                   fps_current = fps_frames;
                   fps_frames = 0;
                }
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
