#ifndef DEBUG_MENU
#define DEBUG_MENU

#include <iostream>
#include <string>

#include "../include/SDL/SDL.h"
//#include "../include/SDL_image/SDL_image.h"
//#include "../include/SDL_mixer/SDL_mixer.h"
#include "../include/SDL_ttf/SDL_ttf.h"

#include "someFunction.hpp"

using namespace std;

class DebugMenu {
    public:
        /**
         * Constuctor of DebugMenu
        */
        DebugMenu(string font);
        /**
         * Print the debug menu on screen
        */
        void print(int fps);

        //If it's display or not
        bool active = false;

        static TTF_Font *debugFont;

    private:
        SDL_Texture *fpsTxt = NULL;
        SDL_Rect     rectFpsTxt;
        SDL_Texture *fpsValue = NULL;
        SDL_Rect     rectFpsValue;


        SDL_Rect rect_atk;
        SDL_Rect rect_hp;
        SDL_Rect rect_speed;
        SDL_Rect rect_name;
        SDL_Rect rect_coo;


        SDL_Texture *name_texture  = NULL;
        SDL_Texture *coo_texture   = NULL;
        SDL_Texture *atk_texture   = NULL;
        SDL_Texture *hp_texture    = NULL;
        SDL_Texture *speed_texture = NULL;

        

};

#endif //MAP_TEST