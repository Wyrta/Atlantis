/**
 * Author : Wyrta
 * Aim : Un rpg
 * License : Sills gaming
 * Date : 25/05/2021 - abort
 *        10/03/2022
 * 
*/

#include <iostream>
#include <string>

#include "../include/SDL/SDL.h"
#include "../include/SDL_image/SDL_image.h"
#include "../include/SDL_mixer/SDL_mixer.h"
#include "../include/SDL_ttf/SDL_ttf.h"

#include "map_test.hpp"


using namespace std;

int main(int argc, char *argv[]) {
    bool running = true;
    
    cout << "Init game utils : ";

    srand(0);   //rand init
    
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);  //SDL init 
    IMG_Init(IMG_INIT_PNG);                     //
    TTF_Init();                                 //

    
    SDL_Renderer *render = NULL;
    SDL_Window   *window = NULL;

    //Init window and render
    cout << "Init window and render" << endl;

    //Init screen size
    SDL_Rect screen;
        screen.h = 800;
        screen.w = 1400;
        screen.x = 0;
        screen.y = 0;

    //init window & render
    window = SDL_CreateWindow("Atlantis", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen.w, screen.h, SDL_WINDOW_RESIZABLE);
    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //Test window and render 
    if(window == NULL) {cout << "error on window" << endl; SDL_Delay(1000); return -1;}
    if(render == NULL) {cout << "error on render" << endl; SDL_Delay(1000); return -1;}


    SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(render, &screen);
    SDL_RenderPresent(render);


    //Init audio
    if( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) < 0 ) { cout << "Error audio SDL" << endl; }
    if( Mix_Init(MIX_INIT_OGG) == 0) { cout << "Error SDL_mixer" << endl; }


    while (running) {
        running = map_test(render, window, screen);
    }
    
    
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
