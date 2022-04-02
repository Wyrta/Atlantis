#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <iostream>
#include <string>
#include <cmath>

#include "../include/SDL/SDL.h"
#include "../include/SDL_image/SDL_image.h"
#include "../include/SDL_mixer/SDL_mixer.h"
#include "../include/SDL_ttf/SDL_ttf.h"

#include "someFunction.hpp"

using namespace std;


class Entity {
    public:
        /**
         * Constuctor of Entity
         * @param render the actual render of the screen 
         * @param position position in X and Y of the entity
         * @param entityName the entity name, can be check with isTextureLoad()
        */
        Entity(SDL_Renderer *render, SDL_Point position, const char *entityName);
        ~Entity();
        /**
         * @return True if texture load else false
        */
        bool isTextureLoad(void) { if(this->texture == NULL) return false; else return true; }

        /**
         * Print the entity on the screen
        */
        bool print(SDL_Renderer *render, bool showHitbox);

        /**
         * Follow the targeted point
         * @param targetPosition the point to follow
         * @param rectmap the map rect to not leave
         * @param screen
         * @param hitboxMap
        */
        bool follow(SDL_Point targetPosition, SDL_Rect rectRap, SDL_Rect screen, SDL_Surface *hitboxMap);
/*
        void goToTop(void)      { mPosition.y += speed; }
        void goToBottom(void)   { mPosition.y -= speed; }
        void goToLeft(void)     { mPosition.x += speed; }
        void goToRight(void)    { mPosition.x -= speed; }
        
        void goToTL();
        void goToTR();
        void goToBL();
        void goToBR();
*/


        int health;

        //in px per ticks 
        int speed = 10;

        SDL_Point mPosition;
        SDL_Rect hitbox;

    private:
        //Entity texture
        SDL_Texture *texture = NULL;

        //Entity foot step
        Mix_Chunk *footstep;
        //Last footstep
        int previousFootstep = 0;

        /**
         * Load the entity's texute and set its rect
        */
        void loadTexture(SDL_Renderer *render, const char *entityName);


        #define NONE    0
        #define TOP     1
        #define BOTTOM  2
        #define LEFT    3
        #define RIGHT   4

        Uint8 red, green, blue;

        bool weMove = false;
        int direction = NONE;

};


class Player : public Entity {
    public:
        Player(SDL_Renderer *render, SDL_Point position, const char *entityName);
        void centerCamera(SDL_Rect *rectMap, SDL_Rect screen);

        //in px per ticks 
        int camSpeed = 10;

};

#endif //ENTITY_HPP