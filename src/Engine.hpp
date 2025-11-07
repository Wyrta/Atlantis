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
#include <mutex>

#include "RenderableItem.hpp"
#include "GameItem.hpp"

class AppOptions {
private: 
    typedef struct Option
    {
        std::string key;
        std::string value;
    } Option;
    
    std::vector<Option> options;
    std::mutex mutex;
public:
    AppOptions();
    void factoryReset(void);
    std::string getOption(std::string key);
    bool setOption(Option option);
    void addOption(Option option);
};

class RenderEngine {
private:
    SDL_Renderer* renderer;
    std::vector<RenderableItem*> items;
    std::mutex mutex;
    int keyboardTarget;
public:
    int addItem(RenderableItem* item);
    RenderableItem* getItem(int id);
    void render(void);
    int loadTextures(void);
    void clearScreen(void);
    void setRenderer(SDL_Renderer* renderer);
    RenderEngine(SDL_Renderer* renderer);

    void mouseMotion(void);
    void mouseDown(SDL_MouseButtonEvent event);
    void mouseUp(SDL_MouseButtonEvent event);

    void requestKeybordTarget(int id);
    void onKey(SDL_KeyboardEvent event);
    void onText(SDL_TextInputEvent event);

    bool debug = false;
};

class GameEngine {
private:
    std::vector<GameItem*> items;
    std::mutex mutex;

public:
    GameEngine();

    int addItem(GameItem* item);
    GameItem* getItem(int id);
    void process(void);
};

#define EVENT_BLANK    -1
#define EVENT_NEW_ITEM_REQUEST    1
#define EVENT_KEYBOARD_REQUEST    2
#define EVENT_QUIT    3

/**
 * Send event to add an item to the RenderEngine
 */
bool newItem(RenderableItem* item);

bool requestKeybordTarget(Uint64 id);

bool requestQuit(void);

#endif // ENGINE