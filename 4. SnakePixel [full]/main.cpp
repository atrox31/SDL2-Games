/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <cmath>
#include <sys/stat.h>
#include "program.cpp"
#include <exception>

//Screen dimension constants
const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const int RES_SCREEN_WIDTH = 500;
const int RES_SCREEN_HEIGHT = 500;

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


string toString(int l){
        stringstream a;
        a<<l;
        return a.str();
    };

string code(string s){
    srand(time(NULL));
    int slen = s.length();
    string r="";
    if(slen>0){
        r+=(40+rand()%40);
        for(int i=0; i<slen; i++){
            char t = s.at(i);
            t^=5;
            t-=3;
            r+=t;
            r+=(t+rand()%10);
        }
        r+=(30+rand()%50);
    }else{
        r="err";
    }
    return r;
}
string decode(string s){
    int slen = s.length();
    string r="";
    if(slen>0){
        for(int i=1; i<slen-1; i+=2){
            char t = s.at(i);
            t+=3;
            t^=5;
            r+=t;
        }
    }else{
        r="err";
    }
    return r;
}
int GetValue(){
    string s;
    ifstream f;;//"%APPDATA%\\atrox games\\snakepixel\\v"
    string p = getenv ("appdata");
    p+="\\atrox games\\snakepixel\\v";
    f.open(p.c_str(), ios::out);
    if (f.is_open())
    {
        getline (f,s);
    }else{
        s="0";

    }
    cout<<s;
    f.close();
    return atoi(decode(s).c_str());
}

void SetValue(string value){
    ofstream f;;//"%APPDATA%\\atrox games\\snakepixel\\v"
    string p = getenv ("appdata");
    p+="\\atrox games\\snakepixel\\v";
    f.open(p.c_str(), ios::in);
    if (f.is_open())
    {
        f << code(value);
    }
    f.close();
}

inline bool fExists (const string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

bool init()
{
	//Initialization flag
	bool success = true;
	string p = getenv ("appdata");
    p+="\\atrox games\\snakepixel\\v";
    if(!fExists(p.c_str())){
        system("mkdir \"%APPDATA%\\atrox games\\snakepixel\\\"");
        system("echo. 2>\"%APPDATA%\\atrox games\\snakepixel\\v\"");
    }

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "PixelSnake 1.0", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, RES_SCREEN_WIDTH, RES_SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );


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
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;
	SDL_DestroyTexture( mTexture );
	mTexture = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

//Main loop flag
bool quit = false;
int main( int argc, char* args[] )
{
    mTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT );
    program Program;
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{

			//Event handler
			SDL_Event e;
			Program.start();
			SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					if (e.type == SDL_KEYDOWN){
                        Program.KeyPressed(e.key.keysym.sym);
					}
					if (e.type == SDL_KEYUP){
                        Program.KeyRelased(e.key.keysym.sym);
					}
				}
				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
				SDL_RenderClear( gRenderer );
				Program.loop();
				SDL_RenderPresent( gRenderer );

			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
