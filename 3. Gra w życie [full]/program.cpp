#include <SDL.h>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <iterator>

#include "font.h"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;
extern bool quit;
//extern string to_string(int*);

struct pos2D {
	int x;
	int y;
};

const int SCALE = 1;
const int SCALE_MAX = 600;

class program {
private:
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
			if (radius > 4) {
				SDL_RenderDrawPoint(renderer, centreX + ((-radius / 2) + rand() % radius), centreY + ((-radius / 2) + rand() % radius));
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
	int Vmin(int a, int b) {
		return a > b ? b : a;
	}
	int Vmax(int a, int b) {
		return a > b ? a : b;
	}
	int Vbet(int vMin, int vMax, int vVal) {
		return Vmin(Vmax(vMin, vVal), vMax);
	}
	int distance(int x1, int y1, int x2, int y2) {
		return (abs(x2 - x1) + abs(y2 - y1));
	}

	font	Font;
	bool	symulation, oneTimeSymulation;
	int     symulationTimeLapse = 0;
	int     symulationTimeScale = 30;

	bool	fields[SCALE_MAX/SCALE][SCALE_MAX/SCALE];

	int		cycle = 0;
	int		population = 0;

	int     cur_pop = 50;
	int     cur_grd = SCALE_MAX/10;

	int     menu_select = 0;
	int     menu_select_min = 0;
	int     menu_select_max = 9;
	string  menu_string[9+1] = { "start", "next gen", "generate", "pop -:+", "scale -:+", "speed -:+", "clear", "grid on","rules", "exit"};

	bool    mouseCreate = false;
	bool    mouseErase = false;
	int     mousex = 0;
	int     mousey = 0;

	bool    show_grid = true;

	bool    rule_window_show = true;
	string  rule_window_string = "23/3";
	int     rule_window_anim = 0;
	int     rule_window_anim_max = 60;

public:
    void creteFieldArray(bool fillArr = true){
        population=0;
        for (int i = 0; i < SCALE_MAX/SCALE; i++)
		{
			for (int j = 0; j < SCALE_MAX/SCALE; j++)
			{
			    if(fillArr){
                    fields[i][j] = (rand() % 100 < cur_pop);
                    if(fields[i][j]){
                        population++;
                    }
			    }else{
                    fields[i][j] = false;
			    }
			}
		}
		cycle = 0;
    }

	void start() {
		srand(time(NULL));
		Font.setup();
		symulation = false;
		oneTimeSymulation = false;
		creteFieldArray(false);
	}

	int coutNeighbers(int x, int y) {
		int cont = 0;
		for (int i = x-1; i <= x+1; i++)
		{
			for (int j = y - 1; j <= y + 1; j++)
			{
				if (i >= 0 && j >= 0 && i<SCALE_MAX/SCALE && j<SCALE_MAX/SCALE) {
                    if(!(i==x && j==y)){
                        if (fields[i][j] == true) {
                            cont++;
                        }
                    }
				}
			}
		}
		return cont;
	}

	void loop() {
	    if(rule_window_show){

	    }else{
            if(mouseCreate || mouseErase){
                SDL_GetMouseState(&mousex, &mousey);
                if(mousex > 0 && mousex < 600){
                    if(mousey > 0 && mousey < 600){
                        fields[mousex/cur_grd][mousey/cur_grd] = mouseCreate?true:mouseErase?false:fields[mousex/cur_grd][mousey/cur_grd];
                    }
                }
            }

            if (symulation || oneTimeSymulation) {
                if(symulationTimeLapse < symulationTimeScale){
                    symulationTimeLapse++;
                }else{
                    symulationTimeLapse=0;
                    if (oneTimeSymulation) {
                        oneTimeSymulation = false;
                    }
                    // nowy cykl
                    cycle++;

                    // obliczanie zycia
                    bool NewFields[SCALE_MAX/SCALE][SCALE_MAX/SCALE];
                    population = 0;
                     //rule_window_string
                    /*
                    przed ukoœnikiem umieszcza siê te liczby komórek w s¹siedztwie, dla których ¿ywe komórki prze¿ywaj¹
                    (dla regu³y Conwaya bêdzie to 23);
                    nastêpnie umieszcza siê ukoœnik: /;
                    po ukoœniku umieszcza siê te liczby komórek w s¹siedztwie,
                    dla których martwe komórki o¿ywaj¹ (dla regu³y Conwaya bêdzie to 3);
                    */
                    string toLive = rule_window_string.substr( 0,   rule_window_string.find('/')    );
                    string toBorn = rule_window_string.substr(      rule_window_string.find('/')+1, rule_window_string.length()-1 );
                    for (int i = 0; i < SCALE_MAX/SCALE; i++)
                    {
                        for (int j = 0; j < SCALE_MAX/SCALE; j++)
                        {
                            NewFields[i][j] = false;
                            char nb = to_string( coutNeighbers(i, j) )[0];

                            for(int tb=0; tb<toBorn.length(); tb++){
                                if (nb == toBorn[tb] && fields[i][j] == false) {
                                    NewFields[i][j] = true;
                                    population++;
                                }
                            }

                            for(int tl=0; tl<toLive.length(); tl++){
                                if (nb == toLive[tl] && fields[i][j] == true) {
                                    NewFields[i][j] = true;
                                    population++;
                                }
                            }
                        }
                    }
                    // kopiowanie nowej tablicy
                    for(int i=0; i<SCALE_MAX/SCALE; i++)
                        for(int j=0; j<SCALE_MAX/SCALE; j++)
                            fields[i][j] = NewFields[i][j];
                }
            }
	    }
	}

	void draw(){
        SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 250);
		Font.draw(610, 10 + (0)*50, "cycle", 3, {255,255,255,255}, 6);
		Font.draw(620, 30 + (0)*50, to_string(cycle), 3, {255,255,255,255}, 6);

		Font.draw(610, 10 + (1)*50, "population", 3, {255,255,255,255}, 6);
		Font.draw(620, 30 + (1)*50, to_string(population), 3, {255,255,255,255}, 6);

		Font.draw(610, 10 + (2)*50, "start pop", 3, {255,255,255,255}, 6);
		Font.draw(620, 30 + (2)*50, to_string(cur_pop), 3, {255,255,255,255}, 6);

		Font.draw(610, 10 + (3)*50, "scale", 3, {255,255,255,255}, 6);
		Font.draw(620, 30 + (3)*50, to_string(cur_grd), 3, {255,255,255,255}, 6);

		Font.draw(610, 10 + (4)*50, "speed", 3, {255,255,255,255}, 6);
		Font.draw(620, 30 + (4)*50, to_string(60-symulationTimeScale), 3, {255,255,255,255}, 6);

        for(int i=menu_select_min; i<=menu_select_max; i++){
            if(i==menu_select){
            SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 40);
                SDL_Rect square = { 610, 80 + (i+6)*30, 800-610, 20 };
                SDL_RenderFillRect(gRenderer, &square);
            }
            Font.draw(610, 80 + (i+6)*30, menu_string[i], 3, {255,255,255,255}, 6);
        }

		// rysowanie siatki
		if(show_grid){
            SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 50);
            for (int i = 0; i <= 600; i += cur_grd)
            {
                SDL_RenderDrawLine(gRenderer, i, 0, i, 600);
                SDL_RenderDrawLine(gRenderer, 0, i, 600, i);
            }
		}

		// rysowanie zycia
		//SDL_SetRenderDrawColor(gRenderer, 250, 180, 180, 100);
        //string toLive = rule_window_string.substr( 0,   rule_window_string.find('/')    );
        //string toBorn = rule_window_string.substr(      rule_window_string.find('/')+1, rule_window_string.length()-1 );
		for (int i = 0; i < 600/cur_grd; i++)
		{
			for (int j = 0; j < 600/cur_grd; j++)
			{
				if (fields[i][j] == true) {

                    int nb = coutNeighbers(i,j);
                    if(nb == 3 || nb == 2)
                        SDL_SetRenderDrawColor(gRenderer, 180, 250, 180, 100);
                    else if( nb > 3)
                        SDL_SetRenderDrawColor(gRenderer, 250, 180, 180, 100);
                    else
                        SDL_SetRenderDrawColor(gRenderer, 180, 180, 250, 100);

					SDL_Rect square = { i * cur_grd, j * cur_grd, cur_grd, cur_grd };
					SDL_RenderFillRect(gRenderer, &square);

				}
			}
		}
		if(rule_window_show){
            rule_window_anim++;
            if(rule_window_anim >= rule_window_anim_max){
                rule_window_anim = 0;
            }

		    int window_width = 300;
		    int window_height = 200;
		    SDL_SetRenderDrawColor(gRenderer, 20, 50, 20, 230);
            SDL_Rect square = {
                SCREEN_WIDTH/2 - window_width,
                SCREEN_HEIGHT/2 - window_height,
                window_width*2,
                window_height*2
                };
            SDL_RenderFillRect(gRenderer, &square);
            SDL_RenderDrawRect(gRenderer, &square);
		    SDL_SetRenderDrawColor(gRenderer, 10, 30, 10, 240);
            SDL_RenderDrawRect(gRenderer, &square);

            Font.draw(
                SCREEN_WIDTH/2 - window_width + 10,
                SCREEN_HEIGHT/2 - window_height + 10,
                "type custom code to game of live",
                3,
                {220,220,220,255},
                6,
                1
            );
            Font.draw(
                SCREEN_WIDTH/2 - window_width + 48,
                SCREEN_HEIGHT/2 - window_height + 38,
                "cell to live / cell to born",
                3,
                {220,220,220,255},
                6,
                1
            );
            Font.draw(
                SCREEN_WIDTH/2 - window_width + 38,
                SCREEN_HEIGHT/2 - window_height + 360,
                "press return to accept",
                4,
                {220,220,220,120 + (Uint8)(((255-120)/8)*(rule_window_anim/8))},
                6
            );


            window_width = 220;
		    window_height = 40;
            SDL_SetRenderDrawColor(gRenderer, 50, 80, 50, 150);
            square = {
                SCREEN_WIDTH/2 - window_width,
                SCREEN_HEIGHT/2 - window_height,
                window_width*2,
                window_height*2
                };
            SDL_RenderFillRect(gRenderer, &square);
            SDL_RenderDrawRect(gRenderer, &square);

            Font.draw(
                SCREEN_WIDTH/2 - window_width + 8,
                SCREEN_HEIGHT/2 - window_height + 30,
                rule_window_string + (rule_window_anim/30==1?"|":""),
                4,
                {220,220,220,240},
                6
            );

		}
	}

	void KeyPressed(SDL_Keycode keysym) {
		switch (keysym) {
			case SDLK_ESCAPE:
				quit=true;
			break;
			case SDLK_RETURN:
			    if(rule_window_show){
                    if(rule_window_string.length() > 1 && count(rule_window_string.begin(), rule_window_string.end(), '/') > 0){
                        rule_window_show = false;
                    }
			    }else{

                    //{ "start", "next gen", "generate", "pop -:+", "grid -:+", "clear", "grid on", "exit"};
                    switch (menu_select){
                    case 0:
                        if(symulation){
                            symulation = false;
                            menu_string[0] = "start";
                        }else{
                            symulation = true;
                            menu_string[0] = "stop";
                        }
                    break;
                    case 1:
                        oneTimeSymulation = true;
                        symulationTimeLapse = symulationTimeScale;
                        break;
                    case 2:
                        creteFieldArray();
                        break;

                    case 6:
                        for (int i = 0; i < SCALE_MAX/SCALE; i++)
                        {
                            for (int j = 0; j < SCALE_MAX/SCALE; j++)
                            {
                                fields[i][j] = false;
                            }
                        }
                        population = 0;
                        cycle = 0;
                        break;
                    case 7:
                        if(show_grid){
                            menu_string[7] = "grid off";
                            show_grid = false;
                        }else{
                            menu_string[7] = "grid on";
                            show_grid = true;
                        }
                        break;
                    case 8:
                        //rules
                        symulation = false;
                        menu_string[0] = "start";
                        rule_window_show= true;
                        break;
                    case 9:
                        quit=true;
                        break;
                    }
			    }
				break;
            case SDLK_LEFT:
                switch (menu_select){
                    case 3:
                        cur_pop = Vmax(0,cur_pop-1);
                    break;
                    case 4:
                        cur_grd = Vmax(600/SCALE_MAX,cur_grd-1);
                    break;
                    case 5:
                        symulationTimeScale = Vmin(60,symulationTimeScale+1);
                    break;
                }
                break;
            case SDLK_RIGHT:
                switch (menu_select){
                    case 3:
                        cur_pop = Vmin(100,cur_pop+1);
                    break;
                    case 4:
                        cur_grd = Vmin(100,cur_grd+1);
                    break;
                    case 5:
                        symulationTimeScale = Vmax(0,symulationTimeScale-1);
                    break;
                }
                break;
            case SDLK_UP:
                if(!rule_window_show)
                    if(menu_select>menu_select_min){
                        menu_select--;
                    }
                break;
            case SDLK_DOWN:
                if(!rule_window_show)
                    if(menu_select<menu_select_max){
                        menu_select++;
                    }
                break;
            default:
                if(keysym == SDLK_BACKSPACE){
                    if(rule_window_string.length() > 0)
                        rule_window_string = rule_window_string.substr(0, rule_window_string.length() - 1);
                }else{
                    if(rule_window_string.length() < 17){
                        const char c = keysym;
                        size_t n = count(rule_window_string.begin(), rule_window_string.end(), '/');
                        if(n==0 && c == '/'){
                            rule_window_string += '/';
                        }
                        else if(c >= '0' && c <= '8'){
                            size_t m = count(rule_window_string.begin() + rule_window_string.find('/'), rule_window_string.end(), c);
                            if( m == 0 ){
                                rule_window_string+=c;
                            }
                        }
                    }
                }
                break;

		}
	}

	void KeyRelased(SDL_Keycode keysym) {
		switch (keysym) {

		}
	}

	void MouseButtonDown(SDL_MouseButtonEvent& mouseS, int mouseX, int mouseY) {
		switch (mouseS.button ) {
		case SDL_BUTTON_LEFT:
                mouseCreate = true;
			break;
		case SDL_BUTTON_RIGHT:
                mouseErase = true;
			break;

		}
	}

	void MouseButtonUp(SDL_MouseButtonEvent& mouseS, int mouseX, int mouseY) {
		switch (mouseS.button) {
		case SDL_BUTTON_LEFT:
                mouseCreate = false;
			break;
		case SDL_BUTTON_RIGHT:
                mouseErase = false;
			break;

		}
	}
};
