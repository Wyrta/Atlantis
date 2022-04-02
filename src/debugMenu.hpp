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
         * @param render the actual render of the screen 
        */
        DebugMenu(SDL_Renderer *render);
        /**
         * Print the debug menu on screen
        */
        void print(SDL_Renderer *render, int fps, SDL_Point playerCoo);

        //If it's display on show or not
        bool active = false;

    private:
        

        TTF_Font *debugFont = NULL;

        SDL_Texture *fpsDisplay = NULL;
        SDL_Rect rectFpsDisplay;
        SDL_Texture *fpsDisplay2 = NULL;
        SDL_Rect rectFpsDisplay2;

        SDL_Texture *texturePosPLayer = NULL;
        SDL_Rect rectTexturePosPLayer;
        SDL_Texture *texturePosPLayerX = NULL;
        SDL_Rect rectTexturePosPLayerX;
        SDL_Texture *texturePosPLayerY = NULL;
        SDL_Rect rectTexturePosPLayerY;
};

#endif //MAP_TEST