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
protected:
    SDL_FPoint position;
    RenderableItem* renderableItem;

public:
    const uint32_t id;
    static uint32_t nbId;

    GameItem(SDL_FPoint position = {0,0});
    ~GameItem();

    virtual void process(Uint64 ticks) {};
    void setPosition(SDL_FPoint position);
    void setRenderableItem(RenderableItem* renderableItem);

    void onHover(SDL_FPoint position);
    void onClick(SDL_FPoint position);
    void onHold(SDL_FPoint position);

    bool canDelete;
};

class Popup : public GameItem {
private:
    std::string title;
    std::string content;
    int duration;   // == 0 : infini duration
    int currentDuration;
    Uint64 startTicks;
    bool doRender;

    std::vector<std::string> choice;
    GameItem* callback;

public:
    Popup(std::string title, std::string content, SDL_FPoint pos = {0.0, 0.0});

    void addChoice(std::string newChoice);

    void show(void);
    void process(Uint64 ticks);

    void setcallback(GameItem* callback);
    void setDuration(int duration);
    int getDuration(void);
};

#endif // GAME_ITEMS