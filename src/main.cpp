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



    SDL_Surface *surface = IMG_Load("./img/background_menu.png");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(render, surface);
    if(texture == NULL) {cout << "error on texture : " << SDL_GetError() << endl; SDL_Delay(5000); return 1;}
    SDL_FreeSurface(surface);


    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
        //SDL_Scancode Z_key = SDL_GetScancodeFromKey(SDLK_z);
        //SDL_Scancode S_key = SDL_GetScancodeFromKey(SDLK_s);
        //SDL_Scancode Q_key = SDL_GetScancodeFromKey(SDLK_q);
        //SDL_Scancode D_key = SDL_GetScancodeFromKey(SDLK_d);


    SDL_Rect player;
        player.w = 100; player.h = 100;
        player.x = screen.w/2-player.w/2; player.y = screen.h/2-player.h/2;

    SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
    
    SDL_Event event;
    int i = 0;
    while(running) {

        while (SDL_PollEvent(&event)) {  // poll until all events are handled!
            //If keypressed
            if(event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym)
                {
                case SDL_SCANCODE_UP:    {player.y -= 10; cout << "move to top"    << endl;} break;
                case SDL_SCANCODE_DOWN:  {player.y += 10; cout << "move to bottom" << endl;} break;
                case SDL_SCANCODE_LEFT:  {player.x -= 10; cout << "move to left"   << endl;} break;
                case SDL_SCANCODE_RIGHT: {player.x += 10; cout << "move to right"  << endl;} break;

                default:
                    break;
                }
            }
            //If user want to quit
            if(event.type == SDL_QUIT) running = false;
        }
        

        SDL_RenderCopy(render, texture, NULL, NULL);
        SDL_RenderFillRect(render, &player);
        SDL_RenderPresent(render);
        SDL_Delay(16);

        i++;
        cout << i << "player y : " << player.x << "player y : " << player.y<< endl;
        if(i>500) running = false;
    }

    SDL_DestroyTexture(texture);
    
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
