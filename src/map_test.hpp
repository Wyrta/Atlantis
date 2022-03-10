#ifndef MAP_TEST
#define MAP_TEST

#include <iostream>
#include <string>

#include "../include/SDL/SDL.h"
#include "../include/SDL_image/SDL_image.h"
#include "../include/SDL_mixer/SDL_mixer.h"
#include "../include/SDL_ttf/SDL_ttf.h"

#include "someFunction.hpp"

using namespace std;


bool map_test(SDL_Renderer *render, SDL_Window *window, SDL_Rect screen);

#endif //MAP_TEST