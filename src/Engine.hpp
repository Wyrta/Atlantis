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
    SDL_Renderer* renderer;
public:
    std::vector<RenderableItems*> items;

    RenderableItems* getItem(int id);
    void render(void);
    int loadTextures(void);
    void clearScreen(void);
    void setRenderer(SDL_Renderer* renderer);
    RenderEngine(SDL_Renderer* renderer);

};

class GameEngine {
private:

public:

};

#endif // ENGINE