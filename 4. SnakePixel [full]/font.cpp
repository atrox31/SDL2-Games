#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <map>
#include <string>

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

using namespace std;
class font{
    map<char, string > fmap;

    public:
    void setup(){
        fmap[' '] = "0000000000000000000000000";
        fmap['-'] = "0000000000111110000000000";
        fmap['p'] = "0111001001011100100001000";
        fmap['i'] = "0010000000001000010000100";
        fmap['x'] = "1000101010001000101010001";
        fmap['e'] = "0111010001111111000001111";
        fmap['l'] = "1000010000100001000011111";
        fmap['s'] = "0111101000011100001011110";
        fmap['n'] = "1000111001101011001110001";
        fmap['a'] = "0010001010011101000110001";
        fmap['k'] = "1001010100110001010010010";
        fmap['t'] = "1111100100001000010000100";
        fmap['r'] = "1111010001111101010010010";
        fmap['t'] = "1111100100001000010000100";
        fmap['w']="0000010001100011010101010";
        fmap['y']="1000101010001000010000100";
        fmap['u']="0000010001100011000101110";
        fmap['o']="0111010001100011000101110";
        fmap['d']="0111001001010010100101110";
        fmap['f']="1111010000111001000010000";
        fmap['g']="0111010000101101001001110";
        fmap['h']="1000110001111111000110001";
        fmap['j']="0010000000001001010011100";
        fmap['z']="0111000010001000100001110";
        fmap['c']="0000001100100001000001100";
        fmap['v']="0000010001010100101000100";
        fmap['b']="1110010010111001001011100";
        fmap['m']="0000011111101011010110101";
        fmap['0']="1111110001100011000111111";
        fmap['1']="0000100001000010000100001";
        fmap['2']="1111100001111111000011111";
        fmap['3']="1111100001111110000111111";
        fmap['4']="1000010100111110010000100";
        fmap['5']="1111110000111110000111111";
        fmap['6']="1111110000111111000111111";
        fmap['7']="1111100001000010000100001";
        fmap['8']="1111110001111111000111111";
        fmap['9']="1111110001111110000111111";
        //snaktr
    }

    void draw(int x, int y, string text, int _size, SDL_Color col, int spacing, int shadow = 0){
        for(int s=0; s<=shadow; s++){
            SDL_SetRenderDrawColor(gRenderer, col.r, col.g, col.b, col.a - s * 50);
            for(int letter=0; letter<text.length(); letter++){
                string cLetter = fmap[text.at(letter)];
                for(int i=0; i<5; i++){
                    for(int j=0; j<5; j++){
                        int index = i+j*5;
                        if(cLetter.at(index)=='1'){
                            SDL_Rect r;
                            r.x = x-(s * 2) + (i * _size) + (_size +  (spacing * _size) * letter);
                            r.y = y-(s * 2) + (j * _size);
                            r.w = _size;
                            r.h = _size;
                            SDL_RenderFillRect( gRenderer, &r );
                        }
                    }
                }
            }

        }
    }

};