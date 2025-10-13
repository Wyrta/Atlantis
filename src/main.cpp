#define SDL_MAIN_USE_CALLBACKS  // This is necessary for the new callbacks API. To use the legacy API, don't define this. 
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>

#include <vector>

#include "RenderableItems.hpp"
#include "Engine.hpp"


struct AppContext {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_AudioDeviceID audioDevice;
    SDL_AppResult app_quit = SDL_APP_CONTINUE;

    RenderEngine renderEngine;
    GameEngine gameEngine;
};

uint32_t windowStartWidth = 400;
uint32_t windowStartHeight = 400;

SDL_AppResult SDL_Fail(){
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

Uint32 SDL_AppWorker(void *userdata, SDL_TimerID timerID, Uint32 interval) {
    auto* app = (AppContext*)userdata;

    // SDL_Log("SDL_AppWorker ! %d", interval);

    return 0;
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
    *appstate = new AppContext{
       .window = window,
       .renderer = renderer,
    };
    
    SDL_SetRenderVSync(renderer, SDL_RENDERER_VSYNC_ADAPTIVE);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60"); // set fps

    SDL_AddTimer(10, (SDL_TimerCallback)SDL_AppWorker, appstate);

    auto* app = (AppContext*)*appstate;
    app->renderEngine.items.push_back(new TextSprite("Hello world !", TTF_OpenFont("Inter-VariableFont.ttf", 16), (SDL_Color){255,0,255,255}));
    app->renderEngine.loadTextures(renderer);

    SDL_Log("Application started successfully!");

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event* event) {
    auto* app = (AppContext*)appstate;

    switch (event->type) {
        case SDL_EVENT_QUIT:
            app->app_quit = SDL_APP_SUCCESS;
            break;

        case SDL_EVENT_WINDOW_RESIZED: {
            int width, height;
            SDL_GetCurrentRenderOutputSize(app->renderer, &width, &height);
            SDL_Log("Window size changed %dx%d", width, height);
            
            } break;

        case SDL_EVENT_MOUSE_MOTION:
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            SDL_FPoint mouse;
            Uint32 mask = SDL_GetMouseState(&mouse.x, &mouse.y);
            bool mouse_left  = (mask & SDL_BUTTON_LMASK);
            bool mouse_right = (mask & SDL_BUTTON_RMASK);
            bool mouse_middle = (mask & SDL_BUTTON_MMASK);
            bool mouse_x1 = (mask & SDL_BUTTON_X1MASK);
            bool mouse_x2 = (mask & SDL_BUTTON_X2MASK);

            // SDL_Log("Mouse position : x%f, y%f %s%s%s%s%s", mouse.x, mouse.y, mouse_left ? "click left " : "", mouse_right ? "click right " : "", mouse_middle ? "click middle " : "", mouse_x1 ? "click x1 " : "", mouse_x2 ? "click x2" : "");
            if (mouse_left == true) {
                std::vector<std::string> test;
                test.push_back("mobTest.png");
                test.push_back("npcTest.png");
                app->renderEngine.items.push_back(new AnimatedSprite(test, 200, mouse));
            }
        } break;
        case SDL_EVENT_MOUSE_WHEEL: {

        } break;
        case SDL_EVENT_KEY_DOWN: {
            SDL_Log("Key pressed: %d", event->key.scancode);
            if (event->key.scancode == 41)
                app->app_quit = SDL_APP_SUCCESS;

            } break;
        case SDL_EVENT_KEY_UP: {
            SDL_Log("Key up: %d", event->key.scancode);

            } break;

        default:
            // SDL_Log("Event: %d", event->type);
            break;
    }

    return app->app_quit;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto* app = (AppContext*)appstate;
    
    SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(app->renderer, NULL);   // clear render
    
    app->renderEngine.render(app->renderer);

    SDL_RenderPresent(app->renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    SDL_Log("Application quit successfully!");
    SDL_Quit();
}