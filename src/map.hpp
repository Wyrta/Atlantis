#ifndef MAP_TEST
#define MAP_TEST

#include <iostream>
#include <string>

#include "../include/SDL/SDL.h"
#include "../include/SDL_image/SDL_image.h"
#include "../include/SDL_mixer/SDL_mixer.h"
#include "../include/SDL_ttf/SDL_ttf.h"

#include "someFunction.hpp"
#include "debugMenu.hpp"
#include "gameManager.hpp"
#include "GUI.hpp"
#include "Entity.hpp"
#include "mapManager.hpp"


using namespace std;


//bool map_test(SDL_Window *window, SDL_Rect screen, string mapName);

string map(SDL_Window *window, SDL_Rect screen, string mapName);


#endif //MAP_TEST