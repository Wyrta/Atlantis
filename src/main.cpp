#define SDL_MAIN_USE_CALLBACKS  // This is necessary for the new callbacks API. To use the legacy API, don't define this. 
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>

#include <vector>

#include "RenderableItem.hpp"
#include "Engine.hpp"
#include "MapItem.hpp"


class AppContext {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_AudioDeviceID audioDevice;
    SDL_AppResult app_quit;

    RenderEngine renderEngine;
    GameEngine gameEngine;

    int mapID;

    AppContext(SDL_Window* window, SDL_Renderer* renderer);
};

AppContext::AppContext(SDL_Window* window, SDL_Renderer* renderer) : renderEngine(renderer), gameEngine() {
    this->window = window;
    this->renderer = renderer;
    this->app_quit = SDL_APP_CONTINUE;
}

uint32_t windowStartWidth = 400;
uint32_t windowStartHeight = 400;

SDL_AppResult SDL_Fail(){
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

Uint32 SDL_AppWorker(void *userdata, SDL_TimerID timerID, Uint32 interval) {
    auto* app = (AppContext*)userdata;

    app->gameEngine.process();

    return interval;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        return SDL_Fail();
    }
    
    // init TTF
    if (not TTF_Init()) {
        return SDL_Fail();
    }
    
    // init Mixer
    if (not MIX_Init()) {
        return SDL_Fail();
    }
    
    // create a window
   
    SDL_Window* window = SDL_CreateWindow("Test window", windowStartWidth, windowStartHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (not window){
        return SDL_Fail();
    }
    
    // create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (not renderer){
        return SDL_Fail();
    }

    // print some information about the window
    SDL_ShowWindow(window);
    {
        int width, height, bbwidth, bbheight;
        SDL_GetWindowSize(window, &width, &height);
        SDL_GetWindowSizeInPixels(window, &bbwidth, &bbheight);
        SDL_Log("Window size: %ix%i", width, height);
        SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
        if (width != bbwidth){
            SDL_Log("This is a highdpi environment.");
        }
    }

    // set up the application data
    *appstate = new AppContext(window, renderer);
    auto* app = (AppContext*)*appstate;
    
    SDL_SetRenderVSync(renderer, SDL_RENDERER_VSYNC_ADAPTIVE);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60"); // set fps

    app->renderEngine.loadTextures();
    app->gameEngine.addItem(new Text("FPS", {0.0, 0.0}));

    int textID = app->gameEngine.addItem(new TextArea("Test TextArea", {0.0, 50.0}));
    GameItem* gameItem = app->gameEngine.getItem(textID);

    SDL_StartTextInput(app->window);

    SDL_AddTimer(10, (SDL_TimerCallback)SDL_AppWorker, app);
    SDL_Log("Application started successfully!");

    Map* map = new Map();
    app->mapID = map->id;
    app->gameEngine.addItem(map);
    
    map->addItem(new Tile(TileType::test, map, {0,0}, {1,1}));
    map->addItem(new Tile(TileType::test, map, {0,1}, {1,1}));
    map->addItem(new Tile(TileType::test, map, {1,1}, {1,1}));
    map->addItem(new Tile(TileType::test, map, {2,2}, {1,1}));

    requestKeybordTarget(map->getRenderableItem()->id);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event* event) {
    auto* app = (AppContext*)appstate;

    switch (event->type) {
        case SDL_EVENT_QUIT:
            app->app_quit = SDL_APP_SUCCESS;
            break;
        case SDL_EVENT_TEXT_INPUT:
            app->renderEngine.onText(event->text);
            break;
        case SDL_EVENT_WINDOW_RESIZED: {
            int width, height;
            SDL_GetCurrentRenderOutputSize(app->renderer, &width, &height);
            SDL_Log("Window size changed %dx%d", width, height);
            
            } break;

        case SDL_EVENT_MOUSE_MOTION:
            app->renderEngine.mouseMotion();
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            app->renderEngine.mouseDown(event->button);
            } break;
        case SDL_EVENT_MOUSE_BUTTON_UP: 
            app->renderEngine.mouseUp(event->button);
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            break;
        case SDL_EVENT_KEY_DOWN: {
            switch (event->key.scancode) {
                case 41: 
                    app->app_quit = SDL_APP_SUCCESS;
                    break;
                case SDL_SCANCODE_F: {
                        Popup* popup = new Popup("Ticks", std::to_string(SDL_GetTicks()), {0.0, 100.0});
                        popup->setDuration(1000);
                        popup->show();
                        app->gameEngine.addItem(popup);
                    } break;
                default: 
                    break;
                }
            app->renderEngine.onKey(event->key);
            } break;

        case SDL_EVENT_KEY_UP: 
            app->renderEngine.onKey(event->key);
            break;
        case SDL_EVENT_USER:
            // SDL_Log("new user event");

            switch (event->user.code)
            {
                case EVENT_NEW_ITEM_REQUEST:
                    app->renderEngine.addItem((RenderableItem*)event->user.data1);
                    break;
                case EVENT_KEYBOARD_REQUEST:
                    app->renderEngine.requestKeybordTarget((Uint64 )event->user.data1);
                    break;
                default:
                    SDL_Log("Unkown event");
                    break;
            }
            break;

        default:
            // SDL_Log("Event: %d", event->type);
            break;
    }

    return app->app_quit;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto* app = (AppContext*)appstate;
    
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);

    Text* text = (Text*)app->gameEngine.getItem(0);
    auto str = std::to_string(SDL_GetTicks());
    if (str.length() < 6)
        str.insert(0, 6 - str.length(), '0');
    text->setText(str + " ms");
    
    app->renderEngine.render();
    
    SDL_RenderPresent(app->renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    SDL_Log("Application quit successfully!");
    SDL_Quit();
}
