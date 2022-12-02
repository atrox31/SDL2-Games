#pragma once
#include <SDL.h>
#include <string>
#include <map>

extern SDL_Renderer* gRenderer;

using namespace std;


class font {
	map<char, string > fmap;
public:
	void setup();
	void draw(int x, int y, string text, int _size, SDL_Color col, int spacing, int shadow = 0);
};