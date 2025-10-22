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
#include "RenderableItem.hpp"
#include "EventHandler.hpp"

class RenderableItem;

class GameItem : public EventHandler {
private:
    SDL_FPoint position;
    RenderableItem* renderableItem;

public:
    const uint32_t id;
    static uint32_t nbId;

    GameItem(SDL_FPoint position = {0,0});

    virtual void process(int tick) = 0;
    void setPosition(SDL_FPoint position);
    void setRenderableItem(RenderableItem* renderableItem);

    void onHover(SDL_FPoint position);
    void onClick(SDL_FPoint position);
    void onHold(SDL_FPoint position);
};


#endif // GAME_ITEMS