#ifndef MAP_MANAGER_HPP
#define MAP_MANAGER_HPP

#include <iostream>
#include <string>

#include "../include/SDL/SDL.h"
//#include "../include/SDL_image/SDL_image.h"
//#include "../include/SDL_mixer/SDL_mixer.h"
//#include "../include/SDL_ttf/SDL_ttf.h"

#include "someFunction.hpp"

#define MAX_SIGN    4

using namespace std;




class MapManager {
    public:
        /**
         * Constuctor of MapManager
        */
        MapManager(SDL_Renderer *render, string mapName);
        ~MapManager();

        /**
         * @return First pos of the player on the map
        */
        SDL_Point initPos();

        /**
         * Print the map on screen 
         * @return True if successful else false
        */
        bool print(SDL_Renderer *render, bool showHitbox);

        /**
         * Print the map on screen 
         * @return true if on a sign, next map name in : string this->nextMapName
        */
        bool isOnSign(SDL_Rect playerHitbox);

        /**
         * next map name
         * default value : "none" 
        */
        string nextMapName = "none";



        //rectangle of the map
        SDL_Rect rectMap;
        //Hitbox surface
        SDL_Surface *hitbox = NULL;
        
        int signNumber = 0;
        SDL_Rect sign[MAX_SIGN];
        string signMapName[MAX_SIGN];
    private:
        //Map texture
        SDL_Texture *textureMap = NULL;

        //Hitbox var
        SDL_Texture *txtHitboxMap = NULL;
        SDL_Rect recthitboxMap;

        int actualDoor = 0;




};

#endif //MAP_MANAGER_HPP