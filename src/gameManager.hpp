#ifndef GAME_MANAGER_HPP
#define GAME_MANAGER_HPP

#include <iostream>
#include <string>

#include "../include/SDL/SDL.h"
//#include "../include/SDL_image/SDL_image.h"
//#include "../include/SDL_mixer/SDL_mixer.h"
//#include "../include/SDL_ttf/SDL_ttf.h"

#include "someFunction.hpp"
#include "GUI.hpp"
#include "debugMenu.hpp"
#include "mapManager.hpp"
#include "Entity.hpp"



using namespace std;


/**
 * contain :
 *      music
 *      event handler
 *      
*/
class GameManager {
    public:
        /**
         * Constuctor of gameManager
         * @param musicName the name of the music 
        */
        GameManager(string musicName);
        ~GameManager();

        void eventHandler(SDL_Window *window, SDL_Rect *screen, GUI *gui, DebugMenu *debugMenu, MapManager *map, Player *player);
        void startMusic();
        void stopMusic();
        /**
         * 0(min) to 128(max)
         * default 32
        */ 
        void setMusicVolume(int volume);    

        /**
         * If the game is running 
         *    -false the game stop
        */
        bool running;

        //Position where the player want to go
        SDL_Point targetPosition;

        /**
         * If the screen should follow the player
        */
        bool screenFollowPlayer;


    private:
        //Event var

        SDL_Event event;
        SDL_Point mouse; Uint32 buttons;

        const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
            SDL_Scancode Z_key = SDL_GetScancodeFromKey(SDLK_z);
            SDL_Scancode S_key = SDL_GetScancodeFromKey(SDLK_s);
            SDL_Scancode Q_key = SDL_GetScancodeFromKey(SDLK_q);
            SDL_Scancode D_key = SDL_GetScancodeFromKey(SDLK_d);

            SDL_Scancode up_key    = SDL_GetScancodeFromKey(SDLK_UP);
            SDL_Scancode down_key  = SDL_GetScancodeFromKey(SDLK_DOWN);
            SDL_Scancode left_key  = SDL_GetScancodeFromKey(SDLK_LEFT);
            SDL_Scancode right_key = SDL_GetScancodeFromKey(SDLK_RIGHT);

        //Screen size
        SDL_DisplayMode screenSize;



        //Musiq var

        Mix_Music *ambiantMusic = NULL;



};

#endif //GAME_MANAGER_HPP