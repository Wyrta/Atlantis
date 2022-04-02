//checkBP.png

#ifndef GUI_HPP
#define GUI_HPP

#include <iostream>
#include <string>
#include <cmath>

#include "../include/SDL/SDL.h"
#include "../include/SDL_image/SDL_image.h"
#include "../include/SDL_mixer/SDL_mixer.h"
#include "../include/SDL_ttf/SDL_ttf.h"

#include "someFunction.hpp"
#include "Entity.hpp"

using namespace std;


class GUI {
    public:
        /**
         * Constuctor of Entity
         * @param render the actual render of the screen 
         * @param position position in X and Y of the entity
         * @param entityName the entity name, can be check with isTextureLoad()
        */
        GUI(SDL_Renderer *render, SDL_Rect screen);
        ~GUI();

        /**
         * Follows player check button
         * @param render render of the scree
         * @param screen screen size to display correctly
         * @param isCheck if button is checked or not
        */
        void followPlayer(SDL_Renderer *render, SDL_Rect screen, bool isCheck);
        void inventory(SDL_Renderer *render, SDL_Rect screen, Player *player, bool isActive);

        bool inventoryState = false;

    private:
        SDL_Texture *checkBP;
        SDL_Rect rectCheckBP;
        SDL_Rect rectCheckBPonScreen;

        SDL_Texture *textureInventory;
        SDL_Rect rectInventory;
        bool oldInventoryState = false;
        Mix_Chunk *scratchsound = NULL;
};

#endif //GUI_HPP