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

AppOptions options;

Uint32 SDL_AppWorker(void *userdata, SDL_TimerID timerID, Uint32 interval) {
    auto* app = (AppContext*)userdata;

    if (options.getOption("map") == "true")
        app->gameEngine.getItem(app->mapID)->enable();
    else
        app->gameEngine.getItem(app->mapID)->disable();
    

    app->gameEngine.process();

    return interval;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    // set up option
    options.loadOptions("option.json");

    // set up the application data
    *appstate = new AppContext();
    auto* app = (AppContext*)*appstate;

    if (app->initSDL() == SDL_APP_FAILURE)
        return SDL_APP_FAILURE;
    
    SDL_SetRenderVSync(app->renderer, SDL_RENDERER_VSYNC_ADAPTIVE);
	SDL_SetRenderDrawBlendMode(app->renderer, SDL_BLENDMODE_BLEND);

    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, options.getOption("fps").c_str()); // set fps

    app->renderEngine.loadTextures();

    SDL_StartTextInput(app->window);

    SDL_AddTimer(10, (SDL_TimerCallback)SDL_AppWorker, app);

    app->gameEngine.addItem(new Text("FPS", {0.0, 0.0}, WHITE));
    app->gameEngine.addItem(new TextArea("Test TextArea", {0.0, 50.0}, WHITE));
    app->gameEngine.addItem(new Button("Quit", {0.0, 100.0, 100.0, 32.0}, "exit"));
    app->gameEngine.addItem(new Button("Fail", {0.0, 150.0, 100.0, 32.0}, "fail"));
    app->gameEngine.addItem(new Button("Nothing", {125.0, 100.0, 100.0, 32.0}, "nothing"));
    app->gameEngine.addItem(new Button("Display map", {125.0, 100.0, 100.0, 32.0}, "map"));

    Map* map = new Map();
    app->mapID = map->id;
    app->gameEngine.addItem(map);

    map->load("assets/map1.json");
    map->setPosition({0.0, 200.0});

    requestKeybordTarget(map->getRenderableItem()->id);

    SDL_Log("Application started successfully!");
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
                    // app->app_quit = SDL_APP_SUCCESS;
                    requestQuit();
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
            switch (event->user.code)
            {
                case EVENT_NEW_ITEM_REQUEST:
                    app->renderEngine.addItem((RenderableItem*)event->user.data1);
                    break;
                case EVENT_KEYBOARD_REQUEST:
                    app->renderEngine.requestKeybordTarget((Uint64 )event->user.data1);
                    break;
                case EVENT_QUIT:
                    app->app_quit = SDL_APP_SUCCESS;
                    break;
                default:
                    SDL_Log("Unkown event %d", event->user.code);
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

    if (options.getOption("exit") == "true")
        return SDL_APP_SUCCESS;

    if (options.getOption("fail") == "true")
        return SDL_APP_FAILURE;

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    if (result == SDL_APP_SUCCESS)
        SDL_Log("Application quit successfully!");
    else
        SDL_Log("Application quit unsuccessfully!");

    SDL_Quit();
}
