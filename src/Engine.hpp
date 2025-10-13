#ifndef ENGINE
#define ENGINE

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <vector>

#include "RenderableItems.hpp"


class RenderEngine {
private:
public:
    RenderableItems* getItem(int id);
    void render(SDL_Renderer* renderer);
    std::vector<RenderableItems*> items;

    int loadTextures(SDL_Renderer* renderer);

};

class GameEngine {
private:

public:

};

#endif // ENGINE