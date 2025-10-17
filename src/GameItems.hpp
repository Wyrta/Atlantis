#ifndef GAME_ITEMS
#define GAME_ITEMS

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <iterator>
#include <vector>

class GameItem {
public:
    void onHover(SDL_FPoint position);
    void onClick(SDL_FPoint position);
    void onHold(SDL_FPoint position);
};


#endif // GAME_ITEMS