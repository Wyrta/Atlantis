#include "Engine.hpp"
#include <filesystem>
namespace fs = std::filesystem;

SDL_AppResult SDL_Fail(){
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

AppOptions::AppOptions() {

}

void AppOptions::factoryReset(void) {

}

std::string AppOptions::getOption(std::string key) {
    this->mutex.lock();
    for(std::vector<Option>::iterator it = this->options.begin(); it != this->options.end(); ++it)
    {
        if ((*it).key == key) {
            this->mutex.unlock();
            return (*it).value;
        }
    }
    this->mutex.unlock();
    return "";
}

bool AppOptions::setOption(Option option) {
    Option opt;
    this->mutex.lock();
    for(std::vector<Option>::iterator it = this->options.begin(); it != this->options.end(); ++it)
    {
        if ((*it).key == option.key) {
            (*it).value = option.value;
            this->mutex.unlock();
            return true;
        }
    }
    this->mutex.unlock();
    return false;
}

void AppOptions::addOption(Option option) {
    if (this->setOption(option))
        return
    
    this->mutex.lock();
    this->options.push_back(option);
    this->mutex.unlock();
}

/**********************************************************************************************************************/

uint32_t windowStartWidth = 400;
uint32_t windowStartHeight = 400;

AppContext::AppContext() {
    this->window = window;
    this->renderer = renderer;
    this->app_quit = SDL_APP_CONTINUE;
}

int AppContext::initSDL(void) {
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
   
    this->window = SDL_CreateWindow("Test window", windowStartWidth, windowStartHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (not this->window){
        return SDL_Fail();
    }
    
    // create a renderer
    this->renderer = SDL_CreateRenderer(this->window, NULL);
    if (not this->renderer){
        return SDL_Fail();
    }
    this->renderEngine.setRenderer(this->renderer);

    // print some information about the window
    SDL_ShowWindow(this->window);
    {
        int width, height, bbwidth, bbheight;
        SDL_GetWindowSize(this->window, &width, &height);
        SDL_GetWindowSizeInPixels(this->window, &bbwidth, &bbheight);
        SDL_Log("Window size: %ix%i", width, height);
        SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
        if (width != bbwidth){
            SDL_Log("This is a highdpi environment.");
        }
    }

    return SDL_APP_SUCCESS;
}

/**********************************************************************************************************************/

RenderEngine::RenderEngine() {
    this->keyboardTarget = -1;
}

uint32_t RenderEngine::addItem(RenderableItem* item) {
    this->mutex.lock();
    this->items[item->getLevel()].push_back(item);
    this->mutex.unlock();

    return item->id;
}

RenderableItem* RenderEngine::getItem(int id) {
    this->mutex.lock();
    for (int i = 0; i < MAX_LEVEL;i +=1) {
        for(std::vector<RenderableItem*>::iterator it = this->items[i].begin(); it != this->items[i].end(); ++it)
        {
            if ((*it)->id == id) {
                this->mutex.unlock();
                return *it;
            }
        }
    }

    this->mutex.unlock();
    return NULL;
}

void RenderEngine::setRenderer(SDL_Renderer* renderer) {
    this->renderer = renderer;
}

void RenderEngine::render(void) {
    RenderableItem* item = NULL;
    this->mutex.lock();
    for (int i = 0; i < MAX_LEVEL;i +=1) {
        for(std::vector<RenderableItem*>::iterator it = this->items[i].begin(); it != this->items[i].end(); ++it)
        {
            item = *it;

            if (item->isDisabled() == false) {
                item->render(this->renderer);
                
                if (this->debug) {
                    SDL_FRect area = item->getArea();
                    SDL_SetRenderDrawColor(this->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
                    SDL_RenderRect(this->renderer, (const SDL_FRect*)&area);
                }
            }

            if (item->canDelete == true) {
                it = this->items[i].erase(it);
                it--;   // get previous iterator

                delete item;
            }
        }
    }
    this->mutex.unlock();
}

int RenderEngine::loadTextures(void) {
    std::string path = "assets";
    int nbTexture = 0;

    for (const fs::directory_entry& dir_entry : fs::recursive_directory_iterator(path)) {
        std::string filename = dir_entry.path().string();
        
        if (filename.ends_with(".png")) {
            // SDL_Log("File found: %s", filename.c_str());
            
            SDL_FRect size;
            SDL_Texture* texture = createTexture(this->renderer, &size, filename.c_str());
            
            std::string name = filename.substr(filename.find("/")+1);
            // SDL_Log("Texture name: %s", name.c_str());

            RenderableItem::textures.push_back((Texture){texture, name, size});
            
            nbTexture++;
        }
    }

    SDL_Log("Loaded %d textures", nbTexture);

    return nbTexture;
}

void RenderEngine::clearScreen(void) {
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(this->renderer, NULL);   // clear render
}

bool doSendEvent(RenderableItem* item, SDL_FPoint mouse) {
    if (item->isDisabled() == true)
        return false;

    SDL_FRect itemArea = item->getArea();

    if (SDL_PointInRectFloat((const SDL_FPoint*)&mouse, (const SDL_FRect*)&itemArea) == false) 
        return false;

    return true;
}

void RenderEngine::mouseMotion(void) {
    SDL_FPoint mouse;
    SDL_FRect itemArea;

    SDL_MouseButtonFlags flags = SDL_GetMouseState(&mouse.x, &mouse.y);

    for (int i = 0; i < MAX_LEVEL;i +=1) {
        for(std::vector<RenderableItem*>::iterator it = this->items[i].begin(); it != this->items[i].end(); ++it)
        {
            if (doSendEvent(*it, mouse) == false)
                continue;

            (*it)->onHover(mouse, flags);
        }
    }
}

void RenderEngine::mouseDown(SDL_MouseButtonEvent event) {
    SDL_FPoint mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);

    for (int i = 0; i < MAX_LEVEL;i +=1) {
        for(std::vector<RenderableItem*>::iterator it = this->items[i].begin(); it != this->items[i].end(); ++it) {
            if (doSendEvent(*it, mouse) == false)
                continue;

            (*it)->onMouseDown(event);
        }
    }
}

void RenderEngine::mouseUp(SDL_MouseButtonEvent event) {
    SDL_FPoint mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);

    for (int i = 0; i < MAX_LEVEL;i +=1) {
        for(std::vector<RenderableItem*>::iterator it = this->items[i].begin(); it != this->items[i].end(); ++it) {
            if (doSendEvent(*it, mouse) == false)
                continue;

            (*it)->onMouseUp(event);
        }
    }
}

void RenderEngine::requestKeybordTarget(int id) {
    // SDL_Log("requestKeybordTarget %d", id);
    this->keyboardTarget = id;
}

void RenderEngine::onText(SDL_TextInputEvent event) {
    if (this->keyboardTarget < 0)
        return;

    EventEmitter* item = (EventEmitter*)this->getItem(this->keyboardTarget);

    if (item == NULL)
        return;

    item->onTextInput(event);
    
}

void RenderEngine::onKey(SDL_KeyboardEvent event) {
    if (this->keyboardTarget < 0)
        return;

    EventEmitter* item = (EventEmitter*)this->getItem(this->keyboardTarget);

    if (item == NULL)
        return;

    // SDL_Log("scancode: %d", event.scancode, event.scancode);

    if (event.down) {
        if (event.repeat) 
            item->onKeyHold(event);
        else
            item->onKeyDown(event);
    }
    else
        item->onKeyUp(event);
}


GameEngine::GameEngine() {

}


void GameEngine::process(void) {
    Uint64 ticks = SDL_GetTicks();
    GameItem* item = NULL;
    this->mutex.lock();
    for(std::vector<GameItem*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
    {
        item = *it;

        item->process(ticks);

        if (item->canDelete == true) {
            it = this->items.erase(it);
            it--;   // get previous iterator

            delete item;
        }
    }
    this->mutex.unlock();
}

int GameEngine::addItem(GameItem* item) {
    this->mutex.lock();

    this->items.push_back(item);
    int itemID = item->id;

    this->mutex.unlock();
    return itemID;
}

GameItem* GameEngine::getItem(int id) {
    this->mutex.lock();
    for(std::vector<GameItem*>::iterator it = this->items.begin(); it != this->items.end(); ++it)
    {
        if ((*it)->id == id) {
            this->mutex.unlock();
            return *it;
        }
    }

    this->mutex.unlock();
    return NULL;
}

static Uint32 eventType = SDL_RegisterEvents(1);

SDL_Event createEvent(int code) {
    if (eventType == 0)
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "createEvent() %s", SDL_GetError());

    SDL_Event event;
    SDL_zero(event);
    event.type = eventType;
    event.user.code = code;
    event.user.data1 = NULL;
    event.user.data2 = NULL;

    return event;
}

bool newItem(RenderableItem* item) {
    SDL_Event event = createEvent(EVENT_NEW_ITEM_REQUEST);

    event.user.data1 = (void *)item;
    bool res = SDL_PushEvent(&event);

    return (event.type != EVENT_BLANK) && res;
}

bool requestKeybordTarget(Uint64 id) {
    SDL_Event event = createEvent(EVENT_KEYBOARD_REQUEST);
    event.user.data1 = (void *)id;
    bool res = SDL_PushEvent(&event);

    return (event.type != EVENT_BLANK) && res;
}

bool requestQuit(void) {
    SDL_Event event = createEvent(EVENT_QUIT);
    bool res = SDL_PushEvent(&event);

    return (event.type != EVENT_BLANK) && res;
}